#ifndef MESHCOLLIDER_H
#define MESHCOLLIDER_H

#include "types.h"
#include "MeshColliderBase.h"

/* The static-mesh collider, vtable _ZTV12MeshCollider at 0x020993dc.
 * Thirteen slots: it overrides Virtual08 and supplies the six collision
 * workers (the three pures at slots 3..5 and the DetectClsn triple at
 * 6..8) from ITCM - 0x01ffd920, 0x01ffd8d8, 0x01ffd890, 0x01ffd3f8,
 * 0x01ffb0fc, 0x01ffb830 - which is why those overrides are declared
 * here but defined nowhere in src/ yet. config/arm9/itcm/symbols.txt
 * already names slot 3's: _ZN12MeshCollider14GetSurfaceInfoEsR11SurfaceInfo,
 * i.e. GetSurfaceInfo(s16, SurfaceInfo &). Slots 9..12 stay on the base
 * implementations.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND NEVER DEFINED AS A METHOD -- see
 * include/ModelBase.h. The structors stay C files.
 *
 * LAYOUT: the base ends at 0x20; SetFile re-runs the shared init
 * func_02039624 and then fills everything from 0x20 up, which is where
 * every offset below comes from.
 */

#ifdef __cplusplus

struct CLPS_Block;
struct SharedFilePtr;

/* Only what the matched code reads is typed. Triangle records are 0x10
   wide; positions are 12-byte s32 vectors read <<6, normals 6-byte s16
   vectors read <<2 (MovingMeshCollider::GetTriangleOrigin / GetNormal). */
struct KCL_Tri {
    u16 unk_00;
    u16 unk_02;
    u16 posIdx;            /* 0x04 */
    u16 normalIdx;         /* 0x06 */
    /* The three edge normals and the surface attribute, read by the ITCM octree
       walk MeshCollider::DetectClsn(RaycastLine&) at 0x01ffb0fc. `attribute` goes
       to the CLPS lookup as a RAW INDEX with no masking, so in this game the KCL
       attribute word is the CLPS index. */
    u16 edgeNormal1Idx;    /* 0x08 */
    u16 edgeNormal2Idx;    /* 0x0a */
    u16 edgeNormal3Idx;    /* 0x0c */
    u16 attribute;         /* 0x0e */
};

struct KCL_File {
    s32 (*positions)[3];   /* 0x00 - file-relative, fixed up on load */
    s16 (*normals)[3];     /* 0x04 */
    KCL_Tri *tris;         /* 0x08 */
    char *unk_0c;          /* 0x0c - the octree, indexed below */

    /* Octree geometry. UpdateFileOffsets relocates only 0x00..0x0c, so nothing
       from here on is a pointer. Positions and this origin are stored at 1/64 of
       a Fix12i unit (`lsl #6` on read) and face normals with 1.0 == 0x400
       (`lsl #2`), so none of these is typed Fix12i. */
    s32 unk_10;
    Vector3 origin;        /* 0x14 - .y pinned by MeshCollider::GetOctreeOriginY;
                              read as one Vector3 by DetectClsn, which materialises
                              file+0x14 once and reads [r0], [r0,#4], [r0,#8] */
    u32 xMask;             /* 0x20 */
    u32 yMask;             /* 0x24 - pinned by MeshCollider::GetUnkOctreeY */
    u32 zMask;             /* 0x28 */
    /* index = (z>>coordShift)<<zShift | (y>>coordShift)<<yShift | (x>>coordShift),
       then coordShift is decremented once per level of descent. */
    u32 coordShift;        /* 0x2c */
    u32 yShift;            /* 0x30 */
    u32 zShift;            /* 0x34 */
};

struct MeshCollider : MeshColliderBase {
    KCL_File *kclFile;        /* 0x20 */
    u32 clps;                 /* 0x24 - set via func_0203821c, released via func_02038224 */
    Fix12i unk_28;            /* 0x28 - init 0 */
    Fix12i unk_2c;            /* 0x2c - init 0x1000 */
    s32 unk_30;               /* 0x30 - init 0 */
    u8 unk_34;                /* 0x34 - init 0 */
    u8 unk_35;                /* 0x35 - init 0 */
    u8 pad_36[2];
    Fix12i unk_38;            /* 0x38 - init 0x1000 */
    s32 unk_3c;               /* 0x3c - init 0 */
    s32 unk_40;               /* 0x40 - init 0 */
    Fix12i unk_44;            /* 0x44 - init -0x1000 */
    s32 unk_48;               /* 0x48 - init 2 */
    u8 unk_4c;                /* 0x4c - init 1 */
    u8 unk_4d;                /* 0x4d - init 0 */
    u8 pad_4e[2];

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~MeshCollider();                              /* slots 0/1 */
    virtual void Virtual08();                             /* slot 2 */
    virtual void GetSurfaceInfo(s16 triID, SurfaceInfo &res); /* slot 3 - ITCM */
    virtual void GetNormal(s16 triID, Vector3 &res);      /* slot 4 - ITCM */
    virtual void GetTriangleOrigin(s16 triID, Vector3 &res); /* slot 5 - ITCM */
    virtual int DetectClsn(RaycastGround &ray);           /* slot 6 - ITCM */
    virtual int DetectClsn(RaycastLine &ray);             /* slot 7 - ITCM */
    virtual int DetectClsn(SphereClsn &sphere);           /* slot 8 - ITCM */

    /* --- non-virtual --- */
    void SetFile(KCL_File *file, CLPS_Block &clps);
    /* Both ITCM, both reading kclFile->origin.y out of the file's 1/64 scale.
       GetUnkOctreeY adds the octree's Y extent, giving its far edge. */
    Fix12i GetOctreeOriginY() const;   /* 0x01ffb0ec */
    Fix12i GetUnkOctreeY() const;      /* 0x01ffb0d0 */

    /* --- static --- */
    static char *LoadFile(SharedFilePtr &ptr);
    static void UpdateFileOffsets(KCL_File &file);
};

typedef char MeshCollider_size_must_be_0x50[sizeof(MeshCollider) == 0x50 ? 1 : -1];

#endif /* __cplusplus */

#endif /* MESHCOLLIDER_H */
