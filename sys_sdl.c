#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <execinfo.h>

#include "quakedef.h"

int noconinput = 0;

char *basedir = ".";
char *cachedir = "/tmp";

// =======================================================================
// General routines
// =======================================================================

void Sys_Printf (char *fmt, ...)
{
	va_list argptr;
	char text[1024];

	va_start (argptr,fmt);
	vsprintf (text,fmt,argptr);
	va_end (argptr);
	fprintf(stderr, "%s", text);
}

void Sys_Quit (void)
{
	Host_Shutdown();
	exit(0);
}

void Sys_Init(void)
{
}

void Sys_Error (char *error, ...)
{
	va_list argptr;
	char string[1024];
	void *buf[32];
	int levels;

	levels = backtrace(buf, nelem(buf));
	backtrace_symbols_fd(&buf[1], levels-1, 2);

	va_start (argptr,error);
	vsprintf (string,error,argptr);
	va_end (argptr);
	fprintf(stderr, "Error: %s\n", string);

	Host_Shutdown ();
	exit (1);

}

void Sys_Warn (char *warning, ...)
{
	va_list argptr;
	char string[1024];

	va_start (argptr,warning);
	vsprintf (string,warning,argptr);
	va_end (argptr);
	fprintf(stderr, "Warning: %s", string);
}

/*
===============================================================================

FILE IO

===============================================================================
*/

#define MAX_HANDLES 10
FILE *sys_handles[MAX_HANDLES];

int findhandle (void)
{
	int i;

	for (i=1 ; i<MAX_HANDLES ; i++)
		if (!sys_handles[i])
			return i;
	Sys_Error ("out of handles");
	return -1;
}

/*
================
Qfilelength
================
*/
static int Qfilelength (FILE *f)
{
	int pos;
	int end;

	pos = ftell (f);
	fseek (f, 0, SEEK_END);
	end = ftell (f);
	fseek (f, pos, SEEK_SET);

	return end;
}

int Sys_FileOpenRead (char *path, int *hndl)
{
	FILE *f;
	int i;

	i = findhandle ();

	f = fopen(path, "rb");
	if (!f)
	{
		*hndl = -1;
		return -1;
	}
	sys_handles[i] = f;
	*hndl = i;

	return Qfilelength(f);
}

int Sys_FileOpenWrite (char *path)
{
	FILE *f;
	int i;

	i = findhandle ();

	f = fopen(path, "wb");
	if (!f)
		Sys_Error ("Error opening %s: %s", path,strerror(errno));
	sys_handles[i] = f;

	return i;
}

void Sys_FileClose (int handle)
{
	if ( handle >= 0 ) {
		fclose (sys_handles[handle]);
		sys_handles[handle] = NULL;
	}
}

void Sys_FileSeek (int handle, int position)
{
	if ( handle >= 0 ) {
		fseek (sys_handles[handle], position, SEEK_SET);
	}
}

int Sys_FileRead (int handle, void *dst, int count)
{
	char *data;
	int size, done;

	size = 0;
	if ( handle >= 0 ) {
		data = dst;
		while ( count > 0 ) {
			done = fread (data, 1, count, sys_handles[handle]);
			if ( done == 0 ) {
				break;
			}
			data += done;
			count -= done;
			size += done;
		}
	}
	return size;

}

int Sys_FileWrite (int handle, void *src, int count)
{
	char *data;
	int size, done;

	size = 0;
	if ( handle >= 0 ) {
		data = src;
		while ( count > 0 ) {
			done = fread (data, 1, count, sys_handles[handle]);
			if ( done == 0 ) {
				break;
			}
			data += done;
			count -= done;
			size += done;
		}
	}
	return size;
}

int Sys_FileTime (char *path)
{
	FILE *f;

	f = fopen(path, "rb");
	if (f)
	{
		fclose(f);
		return 1;
	}

	return -1;
}

void Sys_mkdir (char *path)
{
	mkdir (path, 0777);
}

void Sys_DebugLog(char *file, char *fmt, ...)
{
	va_list argptr;
	static char data[1024];
	FILE *fp;

	va_start(argptr, fmt);
	vsprintf(data, fmt, argptr);
	va_end(argptr);
	fp = fopen(file, "a");
	fwrite(data, strlen(data), 1, fp);
	fclose(fp);
}

double Sys_FloatTime (void)
{
	struct timeval tp;
	struct timezone tzp;
	static int secbase;

	gettimeofday(&tp, &tzp);

	if (!secbase)
	{
		secbase = tp.tv_sec;
		return tp.tv_usec/1000000.0;
	}

	return (tp.tv_sec - secbase) + tp.tv_usec/1000000.0;
}

// =======================================================================
// Sleeps for microseconds
// =======================================================================

int main (int c, char **v)
{

	double time, oldtime, newtime;
	quakeparms_t parms;
	extern int vcrFile;
	extern int recording;

	signal(SIGFPE, SIG_IGN);

	memset(&parms, 0, sizeof(parms));
	parms.memsize = 128*1024*1024;
	parms.membase = malloc (parms.memsize);
	parms.basedir = basedir;
	parms.cachedir = cachedir;

	COM_InitArgv(c, v);
	parms.argc = com_argc;
	parms.argv = com_argv;

	Sys_Init();
	Host_Init(&parms);

	oldtime = Sys_FloatTime () - 0.1;
	while (1)
	{
// find time spent rendering last frame
		newtime = Sys_FloatTime ();
		time = newtime - oldtime;

		if (cls.state == ca_dedicated)
		{ // play vcrfiles at max speed
			if (time < sys_ticrate.value && (vcrFile == -1 || recording) )
			{
				SDL_Delay (1);
				continue; // not time to run a server only tic yet
			}
			time = sys_ticrate.value;
		}

		if (time > sys_ticrate.value*2)
			oldtime = newtime;
		else
			oldtime += time;

		Host_Frame (time);
	}
}
