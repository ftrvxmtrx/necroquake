.PHONY: clean all

CFLAGS=$(shell pkg-config --cflags sdl) -Wall -Wextra -Wno-error=format-security -Wno-missing-field-initializers -rdynamic -g -O1
LDFLAGS=$(shell pkg-config --libs sdl) -rdynamic

srcs=\
	cd_sdl.c \
	chase.c \
	cl_demo.c \
	cl_input.c \
	cl_main.c \
	cl_parse.c \
	cl_tent.c \
	cmd.c \
	common.c \
	console.c \
	crc.c \
	cvar.c \
	d_edge.c \
	d_fill.c \
	d_init.c \
	d_modech.c \
	d_part.c \
	d_polyse.c \
	d_scan.c \
	d_sky.c \
	d_sprite.c \
	d_surf.c \
	d_vars.c \
	d_zpoint.c \
	draw.c \
	host.c \
	host_cmd.c \
	keys.c \
	mathlib.c \
	menu.c \
	model.c \
	net_bsd.c \
	net_dgrm.c \
	net_loop.c \
	net_main.c \
	net_udp.c \
	net_vcr.c \
	pr_cmds.c \
	pr_edict.c \
	pr_exec.c \
	r_aclip.c \
	r_alias.c \
	r_bsp.c \
	r_draw.c \
	r_edge.c \
	r_efrag.c \
	r_light.c \
	r_main.c \
	r_misc.c \
	r_part.c \
	r_sky.c \
	r_sprite.c \
	r_surf.c \
	sbar.c \
	screen.c \
	snd_dma.c \
	snd_mem.c \
	snd_mix.c \
	snd_sdl.c \
	sv_main.c \
	sv_move.c \
	sv_phys.c \
	sv_user.c \
	sys_sdl.c \
	vid_sdl.c \
	view.c \
	wad.c \
	world.c \
	zone.c \

objs=$(srcs:.c=.o)

necroquake: $(objs)
	$(CC) $(LDFLAGS) -o $@ $^

all: necroquake

clean:
	rm -f *.o necroquake
