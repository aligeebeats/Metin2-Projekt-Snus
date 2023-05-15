#ifndef __TERRAIn_TYPES__
#define __TERRAIn_TYPES__

#include "../eterlib/GrpVertexBuffer.h"
#include "../eterlib/GrpIndexBuffer.h"

#define TERRAIN_PATCHSIZE	16
#define TERRAIN_SIZE		128
#define TERRAIN_PATCHCOUNT	TERRAIN_SIZE/TERRAIN_PATCHSIZE
#define MAXTERRAINTEXTURES	256

typedef struct
{
	long					Active;
	long					NeedsUpdate;
	LPDIRECT3DTEXTURE8		pd3dTexture;
} TTerainSplat;

typedef struct
{
 	DWORD			TileCount[MAXTERRAINTEXTURES];
	DWORD			PatchTileCount[TERRAIN_PATCHCOUNT*TERRAIN_PATCHCOUNT][MAXTERRAINTEXTURES];
	TTerainSplat 	Splats[MAXTERRAINTEXTURES];
	bool			m_bNeedsUpdate;
} TTerrainSplatPatch;

typedef struct
{
	char					used;
	short					mat;
	
	CGraphicVertexBuffer	vb;
	CGraphicIndexBuffer		ib;
	long					VertexSize;
	
	short					NumIndices;
	
	float					minx, maxx;
	float					miny, maxy;
	float					minz, maxz;
} TERRAIN_VBUFFER;

typedef struct
{
	char name[19];
	float ambi_r, ambi_g, ambi_b, ambi_a;
	float diff_r, diff_g, diff_b, diff_a;
	float spec_r, spec_g, spec_b, spec_a;
	float spec_power;
} PR_MATERIAL;

typedef struct
{
	float			PageUVLength;
	long			SquaresPerTexture;
	long			SplatTilesX;
	long			SplatTilesY;
	long			DisableWrapping;
	long			DisableShadow;
	long			ShadowMode;
	long			OutsideVisible;
	D3DXVECTOR3		SunLocation;
} TTerrainGlobals;

#define PR_FLOAT_TO_INTASM __asm	\
{									\
	__asm fld PR_FCNV				\
	__asm fistp PR_ICNV				\
}

#define PR_FLOAT_TO_FIXED(inreal, outint)	\
{											\
	PR_FCNV = (inreal) * 65536.0f;			\
    PR_FLOAT_TO_INTASM;						\
	(outint) = PR_ICNV;						\
}

#define PR_FLOAT_TO_INT(inreal, outint)							\
{																\
	PR_FCNV = (inreal);											\
    PR_FLOAT_TO_INTASM;											\
	(outint) = PR_ICNV > PR_FCNV ? PR_ICNV - 1 : PR_ICNV;		\
}

#define PR_FLOAT_ADD_TO_INT(inreal, outint)	\
{											\
	PR_FCNV = (inreal);						\
    PR_FLOAT_TO_INTASM;						\
	(outint) += PR_ICNV;					\
}

#endif