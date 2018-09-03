// upper design bounds

#define MAX_MAP_HULLS 4

#define MAX_MAP_MODELS 256
#define MAX_MAP_BRUSHES 4096
#define MAX_MAP_ENTITIES 1024
#define MAX_MAP_ENTSTRING 65536

#define MAX_MAP_PLANES 65535
#define MAX_MAP_NODES 65535 // because negative shorts are contents
#define MAX_MAP_CLIPNODES 65530 //
#define MAX_MAP_LEAFS 32768
#define MAX_MAP_VERTS 65535
#define MAX_MAP_FACES 65535
#define MAX_MAP_MARKSURFACES 65535
#define MAX_MAP_TEXINFO 4096
#define MAX_MAP_EDGES 256000
#define MAX_MAP_SURFEDGES 512000
#define MAX_MAP_TEXTURES 512
#define MAX_MAP_MIPTEX 0x200000
#define MAX_MAP_LIGHTING 0x100000
#define MAX_MAP_VISIBILITY 0x100000

#define MAX_MAP_PORTALS 65536

// key / value pair sizes

#define MAX_KEY 32
#define MAX_VALUE 1024

//=============================================================================

#define BSPVERSION 29
#define TOOLVERSION 2

typedef struct
{
	int fileofs, filelen;
}lump_t;

enum {
	LUMP_ENTITIES,
	LUMP_PLANES,
	LUMP_TEXTURES,
	LUMP_VERTEXES,
	LUMP_VISIBILITY,
	LUMP_NODES,
	LUMP_TEXINFO,
	LUMP_FACES,
	LUMP_LIGHTING,
	LUMP_CLIPNODES,
	LUMP_LEAFS,
	LUMP_MARKSURFACES,
	LUMP_EDGES,
	LUMP_SURFEDGES,
	LUMP_MODELS,

	HEADER_LUMPS
};

typedef struct
{
	float mins[3], maxs[3];
	float origin[3];
	int headnode[MAX_MAP_HULLS];
	int visleafs; // not including the solid leaf 0
	int firstface, numfaces;
}dmodel_t;

typedef struct
{
	int version;
	lump_t lumps[HEADER_LUMPS];
}dheader_t;

typedef struct
{
	int nummiptex;
	int dataofs[4]; // [nummiptex]
}dmiptexlump_t;

#define MIPLEVELS 4
typedef struct miptex_s {
	char name[16];
	unsigned width, height;
	unsigned offsets[MIPLEVELS]; // four mip maps stored
}miptex_t;

typedef struct {
	float point[3];
}dvertex_t;

enum {
	// 0-2 are axial planes
	PLANE_X,
	PLANE_Y,
	PLANE_Z,

// 3-5 are non-axial planes snapped to the nearest
	PLANE_ANYX,
	PLANE_ANYY,
	PLANE_ANYZ,
};

typedef struct {
	float normal[3];
	float dist;
	int type; // PLANE_X - PLANE_ANYZ ?remove? trivial to regenerate
}dplane_t;

#define CONTENTS_EMPTY -1
#define CONTENTS_SOLID -2
#define CONTENTS_WATER -3
#define CONTENTS_SLIME -4
#define CONTENTS_LAVA -5
#define CONTENTS_SKY -6
#define CONTENTS_ORIGIN -7 // removed at csg time
#define CONTENTS_CLIP -8 // changed to contents_solid

#define CONTENTS_CURRENT_0 -9
#define CONTENTS_CURRENT_90 -10
#define CONTENTS_CURRENT_180 -11
#define CONTENTS_CURRENT_270 -12
#define CONTENTS_CURRENT_UP -13
#define CONTENTS_CURRENT_DOWN -14

typedef struct {
	int planenum;
	short children[2]; // negative numbers are -(leafs+1), not nodes
	short mins[3]; // for sphere culling
	short maxs[3];
	unsigned short firstface;
	unsigned short numfaces; // counting both sides
}dnode_t;

typedef struct {
	int planenum;
	short children[2]; // negative numbers are contents
}dclipnode_t;

typedef struct texinfo_s {
	float vecs[2][4]; // [s/t][xyz offset]
	int miptex;
	int flags;
}texinfo_t;
#define TEX_SPECIAL 1 // sky or slime, no lightmap or 256 subdivision

// note that edge 0 is never used, because negative edge nums are used for
// counterclockwise use of the edge in a face
typedef struct {
	unsigned short v[2]; // vertex numbers
}dedge_t;

#define MAXLIGHTMAPS 4
typedef struct {
	short planenum;
	short side;

	int firstedge; // we must support > 64k edges
	short numedges;
	short texinfo;

// lighting info
	uint8_t styles[MAXLIGHTMAPS];
	int lightofs; // start of [numstyles*surfsize] samples
}dface_t;

enum {
	AMBIENT_WATER,
	AMBIENT_SKY,
	AMBIENT_SLIME,
	AMBIENT_LAVA,

	NUM_AMBIENTS
};

// leaf 0 is the generic CONTENTS_SOLID leaf, used for all solid areas
// all other leafs need visibility info
typedef struct {
	int contents;
	int visofs; // -1 = no visibility info

	short mins[3]; // for frustum culling
	short maxs[3];

	unsigned short firstmarksurface;
	unsigned short nummarksurfaces;

	uint8_t ambient_level[NUM_AMBIENTS];
}dleaf_t;
