#ifndef OV075_ENTRY_MESH_H
#define OV075_ENTRY_MESH_H

#include "common.h"

struct BMD_File;

/* The entry-menu grid mesh, the 0xa8-byte sub-object dEntObj_c keeps at
 * +0xe80 (dEntObj_c::mMesh). The cartridge gives it no class name. A square
 * grid of mGridDim[0] x mGridDim[1] vertices, drawn as triangle strips and
 * rippled outward from a touched point. Its methods are ov075
 * 0x0211a948..0x0211b524, in src/game/stages/d_s_entry_entry.cpp. */

/* Vector3- and Matrix4x3-layout twins. Not those types themselves:
 * Vector3 declares a destructor, so a Vector3 member (or the Vector3 t of
 * math/Matrix.h's Matrix4x3) makes this unit emit an unreferenced
 * ~Vector3() copy and gives EntryMesh a destructor dEntObj_c's object would
 * emit; the ROM has neither (the same reason as dBgW_Kc's PlainVec). */
struct EntryMeshVec {
    s32 x, y, z;
};
struct EntryMeshMtx {
    s32 m[12];
};

/* One grid vertex. InitGrid lays the grid out, CalcNormals packs the
 * normals and DrawGrid sends each vertex to the geometry engine. */
typedef struct EntryMeshVertex {
    EntryMeshVec pos; /* 0x00 -- 1.19.12, sent as VTX_16; z is the ripple height */
    s32 dist;         /* 0x0c -- CalcDistances: distance from mRippleCenter */
    /* 0x10: the packed 10-bit x/y/z normal CalcNormals builds; DrawGrid
       sends it to G3_NORMAL (0x4000484). */
    u32 normal;
    u32 texCoord; /* 0x14 -- sent to G3_TEXCOORD (0x4000488) */
} EntryMeshVertex;

/* The mesh's material block, filled by LoadTexture and pushed to the
 * geometry engine by SendMaterial: the same five words, in the same order,
 * as daPicGate_c's record (include/private/Ov080Mat.h), with that header's
 * names. */
typedef struct EntryMeshMaterial {
    u32 texAddr; /* 0x00 */
    u32 pltAddr; /* 0x04 */
    u32 difAmb;  /* 0x08 */
    u32 speEmi;  /* 0x0c */
    u32 param;   /* 0x10 -- TEXIMAGE_PARAM format, size and pltt0 bits */
} EntryMeshMaterial;

struct EntryMesh;
/* One handler of the {init, behavior, render} row mState points at. */
typedef void (EntryMesh::*EntryMeshHandler)();

struct EntryMesh {
    EntryMeshVec mPos; /* 0x00 -- CalcMatrix translates mMtx by it */
    s16 mAngleX;       /* 0x0c -- only zeroed, by Init (medium confidence) */
    s16 mAngleY;       /* 0x0e -- CalcMatrix rotates mMtx by it */
    s16 mAngleZ;       /* 0x10 -- only zeroed, by Init (medium confidence) */
    u8 pad_12[2];
    /* 0x14: the model matrix CalcMatrix builds and DrawGrid concatenates
       with the camera. */
    EntryMeshMtx mMtx;
    /* 0x44: mMtx's inverse, which CalcMatrix stores; nothing in this
       overlay reads it back (medium confidence). */
    EntryMeshMtx mInvMtx;
    /* 0x74: the point in grid space a ripple starts at (StartRipple;
       TouchAt maps the touch into grid space). */
    EntryMeshVec mRippleCenter;
    EntryMeshVertex *mVertices; /* 0x80 -- mNumVertices of them */
    /* 0x84: a {init, behavior, render} row of handlers. Init runs [0],
       Behavior [1] and Render [2]. */
    EntryMeshHandler *mState;
    EntryMeshMaterial mMaterial; /* 0x88 */
    u8 pad_9c[4];
    /* 0xa0: the ripple's sine phase; UpdateRipple advances it by 0x1111 a
       frame and RipplePhase measures from it. */
    s16 mRipplePhase;
    /* 0xa2: frames left in the ripple, from 100. UpdateRipple counts it
       down; RippleHeight displaces a vertex only while it is nonzero,
       within a radius that grows with 100 - mRippleTimer. */
    u16 mRippleTimer;
    u16 mNumVertices; /* 0xa4 -- mGridDim[0] * mGridDim[1] */
    /* 0xa6: the grid is square (Init sets both to 0x14) and DrawGrid and
       CalcNormals both use mGridDim[1] as the row stride, so the two bytes
       are not split into named axes. */
    u8 mGridDim[2];

    void SendMaterial();
    void CalcMatrix();
    int LoadTexture(BMD_File *);
    void TouchAt(Vector3 *);
    void CalcNormals();
    void PinBorder();
    int RippleHeight(int);
    int RippleAmplitude(int);
    short RipplePhase(int);
    void CalcDistances();
    void StartRipple(int, int);
    void DrawGrid();     /* render handler */
    void UpdateRipple(); /* behavior handler */
    void InitGrid();     /* init handler */
    int Destroy();
    int Render();
    int Behavior();
    int Init(const Vector3 *, int);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char EntryMeshVertex_size_must_be_0x18[sizeof(EntryMeshVertex) == 0x18 ? 1 : -1];
typedef char EntryMeshMaterial_size_must_be_0x14[sizeof(EntryMeshMaterial) == 0x14 ? 1 : -1];
typedef char EntryMesh_size_must_be_0xa8[sizeof(EntryMesh) == 0xa8 ? 1 : -1];
typedef char EntryMesh_mMtx_must_be_at_0x14[(int)&((EntryMesh *)0)->mMtx == 0x14 ? 1 : -1];
typedef char
    EntryMesh_mVertices_must_be_at_0x80[(int)&((EntryMesh *)0)->mVertices == 0x80 ? 1 : -1];
typedef char
    EntryMesh_mRippleTimer_must_be_at_0xa2[(int)&((EntryMesh *)0)->mRippleTimer == 0xa2 ? 1 : -1];
#endif

#endif
