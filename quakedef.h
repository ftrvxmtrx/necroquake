// quakedef.h -- primary header for client

#include <setjmp.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define nelem(x) (int)(sizeof(x)/sizeof((x)[0]))
#define USED(x) (void)x
#define MAX(a, b) ((a) < (b) ? (b) : (a))

#define QUAKE_GAME // as opposed to utilities

#undef VERSION
#define VERSION 1.09

//define PARANOID // speed sapping error checking

#define GAMENAME "id1" // directory to look in by default

#define UNALIGNED_OK 0

// !!! if this is changed, it must be changed in d_ifacea.h too !!!
#define CACHE_SIZE 32 // used to align key data structures

#define UNUSED(x) (x = x) // for pesky compiler / lint warnings

#define MINIMUM_MEMORY 0x550000
#define MINIMUM_MEMORY_LEVELPAK (MINIMUM_MEMORY + 0x100000)

#define MAX_NUM_ARGVS 50

// up / down
#define PITCH 0

// left / right
#define YAW 1

// fall over
#define ROLL 2

#define MAX_QPATH 64 // max length of a quake game pathname
#define MAX_OSPATH 128 // max length of a filesystem pathname

#define ON_EPSILON 0.1 // point on plane side epsilon

#define MAX_MSGLEN 8000 // max length of a reliable message
#define MAX_DATAGRAM 1024 // max length of unreliable message

//
// per-level limits
//
#define MAX_EDICTS 600 // FIXME: ouch! ouch! ouch!
#define MAX_LIGHTSTYLES 64
#define MAX_MODELS 256 // these are sent over the net as bytes
#define MAX_SOUNDS 256 // so they cannot be blindly increased

#define SAVEGAME_COMMENT_LENGTH 39

#define MAX_STYLESTRING 64

//
// stats are integers communicated to the client by the server
//
#define MAX_CL_STATS 32
enum {
	STAT_HEALTH,
	STAT_FRAGS,
	STAT_WEAPON,
	STAT_AMMO,
	STAT_ARMOR,
	STAT_WEAPONFRAME,
	STAT_SHELLS,
	STAT_NAILS,
	STAT_ROCKETS,
	STAT_CELLS,
	STAT_ACTIVEWEAPON,
	STAT_TOTALSECRETS,
	STAT_TOTALMONSTERS,
	STAT_SECRETS,
	STAT_MONSTERS,
};

enum {
	IT_SHOTGUN = 1<<0,
	IT_SUPER_SHOTGUN = 1<<1,
	IT_NAILGUN = 1<<2,
	IT_SUPER_NAILGUN = 1<<3,
	IT_GRENADE_LAUNCHER = 1<<4,
	IT_ROCKET_LAUNCHER = 1<<5,
	IT_LIGHTNING = 1<<6,
	IT_SUPER_LIGHTNING = 1<<7,
	IT_SHELLS = 1<<8,
	IT_NAILS = 1<<9,
	IT_ROCKETS = 1<<10,
	IT_CELLS = 1<<11,
	IT_AXE = 1<<12,
	IT_ARMOR1 = 1<<13,
	IT_ARMOR2 = 1<<14,
	IT_ARMOR3 = 1<<15,
	IT_SUPERHEALTH = 1<<16,
	IT_KEY1 = 1<<17,
	IT_KEY2 = 1<<18,
	IT_INVISIBILITY = 1<<19,
	IT_INVULNERABILITY = 1<<20,
	IT_SUIT = 1<<21,
	IT_QUAD = 1<<22,

	IT_SIGIL1 = 1<<28,
	IT_SIGIL2 = 1<<29,
	IT_SIGIL3 = 1<<30,
	IT_SIGIL4 = 1<<31,
};

//===========================================
//rogue changed and added defines

enum {
	RIT_SHELLS = 1<<7,
	RIT_NAILS = 1<<8,
	RIT_ROCKETS = 1<<9,
	RIT_CELLS = 1<<10,
	RIT_AXE = 1<<11,
	RIT_LAVA_NAILGUN = 1<<12,
	RIT_LAVA_SUPER_NAILGUN = 1<<13,
	RIT_MULTI_GRENADE = 1<<14,
	RIT_MULTI_ROCKET = 1<<15,
	RIT_PLASMA_GUN = 1<<16,

	RIT_ARMOR1 = 1<<23,
	RIT_ARMOR2 = 1<<24,
	RIT_ARMOR3 = 1<<25,
	RIT_LAVA_NAILS = 1<<26,
	RIT_PLASMA_AMMO = 1<<27,
	RIT_MULTI_ROCKETS = 1<<28,
	RIT_SHIELD = 1<<29,
	RIT_ANTIGRAV = 1<<30,
	RIT_SUPERHEALTH = 1<<31,
};

//MED 01/04/97 added hipnotic defines
//===========================================
//hipnotic added defines
enum {
	HIT_PROXIMITY_GUN_BIT = 16,
	HIT_MJOLNIR_BIT = 7,
	HIT_LASER_CANNON_BIT = 23,
	HIT_PROXIMITY_GUN = 1<<HIT_PROXIMITY_GUN_BIT,
	HIT_MJOLNIR = 1<<HIT_MJOLNIR_BIT,
	HIT_LASER_CANNON = 1<<HIT_LASER_CANNON_BIT,
	HIT_WETSUIT = 1<<(23+2),
	HIT_EMPATHY_SHIELDS = 1<<(23+3)
};

//===========================================

#define MAX_SCOREBOARD 16
#define MAX_SCOREBOARDNAME 32

#define SOUND_CHANNELS 8

#include "common.h"
#include "bspfile.h"
#include "vid.h"
#include "sys.h"
#include "zone.h"
#include "mathlib.h"

typedef struct
{
	vec3_t origin;
	vec3_t angles;
	int modelindex;
	int frame;
	int colormap;
	int skin;
	int effects;
} entity_state_t;

#include "wad.h"
#include "draw.h"
#include "cvar.h"
#include "screen.h"
#include "net.h"
#include "protocol.h"
#include "cmd.h"
#include "sbar.h"
#include "sound.h"
#include "render.h"
#include "client.h"
#include "progs.h"
#include "server.h"

#include "model.h"
#include "d_iface.h"

#include "input.h"
#include "world.h"
#include "keys.h"
#include "console.h"
#include "view.h"
#include "menu.h"
#include "crc.h"
#include "cdaudio.h"

//=============================================================================

// the host system specifies the base of the directory tree, the
// command line parms passed to the program, and the amount of memory
// available for the program to use

typedef struct
{
	char *basedir;
	char *cachedir; // for development over ISDN lines
	int argc;
	char **argv;
	void *membase;
	int memsize;
} quakeparms_t;

//=============================================================================

extern bool noclip_anglehack;

//
// host
//
extern quakeparms_t host_parms;

extern cvar_t sys_ticrate;
extern cvar_t developer;

extern bool host_initialized; // true if into command execution
extern double host_frametime;
extern uint8_t *host_basepal;
extern uint8_t *host_colormap;
extern int host_framecount; // incremented every frame, never reset
extern double realtime; // not bounded in any way, changed at
										// start of every frame, never reset

void Host_ClearMemory (void);
void Host_ServerFrame (void);
void Host_InitCommands (void);
void Host_Init (quakeparms_t *parms);
void Host_Shutdown(void);
void Host_Error (char *error, ...);
void Host_EndGame (char *message, ...);
void Host_Frame (float time);
void Host_Quit_f (void);
void Host_ClientCommands (char *fmt, ...);
void Host_ShutdownServer (bool crash);

extern bool msg_suppress_1; // suppresses resolution and cache size console output
										// an fullscreen DIB focus gain/loss
extern int current_skill; // skill level for currently loaded level (in case
										// the user changes the cvar while the level is
										// running, this reflects the level actually in use)

extern bool isDedicated;

extern int minimum_memory;

//
// chase
//
extern cvar_t chase_active;

void Chase_Init (void);
void Chase_Reset (void);
void Chase_Update (void);
