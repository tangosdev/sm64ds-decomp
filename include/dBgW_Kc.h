#ifndef DBGW_KC_H
#define DBGW_KC_H

#include "types.h"
#include "dBgW.h"
#include "CLPS_BlockRef.h"

/* The static-mesh collider, vtable _ZTV7dBgW_Kc at 0x020993dc.
 * Thirteen slots: it overrides Virtual08 and supplies the six collision
 * workers (the three pures at slots 3..5 and the DetectClsn triple at
 * 6..8) from ITCM - 0x01ffd920, 0x01ffd8d8, 0x01ffd890, 0x01ffd3f8,
 * 0x01ffb0fc, 0x01ffb830 - the six are defined in
 * src/engine/collision/d_bg_w_kc_itcm.cpp. Slots 9..12 stay on the base
 * implementations.
 *
 * The CLPS_BlockRef member owns its one-word lifetime. Its compiler-generated
 * construction and destruction are the calls visible in all five dBgW_Kc
 * structor variants; the exact original wrapper spelling is not known.
 *
 * LAYOUT: the base ends at 0x20; SetFile re-runs the shared init
 * func_02039624 and then fills everything from 0x20 up, which is where
 * every offset below comes from.
 */

#ifdef __cplusplus

struct SharedFilePtr;

/* Only what the matched code reads is typed. Triangle records are 0x10
   wide; positions are 12-byte s32 vectors read <<6, normals 6-byte s16
   vectors read <<2 (dBgW_KcMbg::GetTriangleOrigin / GetNormal). */
struct KCL_Tri {
    /* The prism's extent along its third edge normal, read as one 32-bit word by
       dBgW_Kc::DetectClsn(dBgCh_Gnd&) at 0x01ffd3f8 (`ldr r0,[r7]`). */
    s32 length;            /* 0x00 */
    u16 posIdx;            /* 0x04 */
    u16 normalIdx;         /* 0x06 */
    /* The three edge normals and the surface attribute, read by the ITCM octree
       walk dBgW_Kc::DetectClsn(dBgCh_Lin&) at 0x01ffb0fc. `attribute` goes
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
    char *octree;          /* 0x0c - indexed below */

    /* Octree geometry. UpdateFileOffsets relocates only 0x00..0x0c, so nothing
       from here on is a pointer. Positions and this origin are stored at 1/64 of
       a Fix12i unit (`lsl #6` on read) and face normals with 1.0 == 0x400
       (`lsl #2`), so none of these is typed Fix12i. prismThickness is the KCL
       header's: how far behind its face a prism still reaches, in the same
       raw x 0x400 units as a face dot. */
    s32 prismThickness;    /* 0x10 */
    Vector3 origin;        /* 0x14 - .y pinned by dBgW_Kc::GetOctreeOriginY;
                              read as one Vector3 by DetectClsn, which materialises
                              file+0x14 once and reads [r0], [r0,#4], [r0,#8] */
    u32 xMask;             /* 0x20 */
    u32 yMask;             /* 0x24 - pinned by dBgW_Kc::GetUnkOctreeY */
    u32 zMask;             /* 0x28 */
    /* index = (z>>coordShift)<<zShift | (y>>coordShift)<<yShift | (x>>coordShift),
       then coordShift is decremented once per level of descent. */
    u32 coordShift;        /* 0x2c */
    u32 yShift;            /* 0x30 */
    u32 zShift;            /* 0x34 */
};

namespace Memory {
void operator_delete2(void *);
}

struct dBgW_Kc : dBgW {
    KCL_File *kclFile;        /* 0x20 */
    CLPS_BlockRef clps;       /* 0x24 - compiler-owned value lifetime */
    /* World up, carried into the collider's own space: dBgW_KcMbg::SetFile
       and Transform rotate (0, 0x1000, 0) through invRotMat into it, and
       SetFile here seeds that same vector unrotated. DetectClsn(dBgCh_SphCrr&)
       dots surface normals against it in the floor edge test and measures the
       wall-reject slab along it (notes/collision-query-classes.md).
       It and depthDir are plain three-word vectors, not Vector3: a Vector3
       member makes d_bg_w_kc.cpp emit an unreferenced ~Vector3() copy, which
       the ROM's object does not have. The type is named because an unnamed
       one takes a compiler @NNN number in every consumer. */
    struct PlainVec {
        Fix12i x, y, z;
    };
    PlainVec upAxis; /* 0x28 */
    /* The penetration-depth policy of DetectClsn(dBgCh_SphCrr&): how far behind a
       face the sphere centre may sit and still be pushed out. By default it is
       the file's prismThickness. shallowOnly caps every face at 5.0 (0x50000
       in face-dot units); depthByDir keeps the prism thickness only for faces
       whose normal dots above depthDirMinDot against depthDir and caps the rest
       at 10.0.
       Set and cleared by func_01ffb0bc/0b0 (shallowOnly), func_01ffb0a4/098
       (depthByDir), func_01ffb07c (depthDir) and func_020396d0 (depthDirMinDot). */
    bool shallowOnly; /* 0x34 */
    bool depthByDir;  /* 0x35 */
    u8 pad_36[2];
    PlainVec depthDir;     /* 0x38 - init the unit X vector */
    Fix12i depthDirMinDot; /* 0x44 - init -0x1000 */
    /* The edge-contact policy of the same query
       (notes/collision-query-classes.md):
         edgeToleranceShift  a SHIFT COUNT, not a value: an edge hit is rejected
                             when the lateral distance outside the edge exceeds
                             faceDot >> edgeToleranceShift, i.e. a slope
                             tolerance as a fraction of the penetration
         edgeContacts        master gate for the edge/vertex regions; clear
                             means only a face contact can register
         tolerantWallEdges   selects the tolerant form of that test for walls */
    s32 edgeToleranceShift; /* 0x48 - init 2 */
    bool edgeContacts;      /* 0x4c - init true */
    bool tolerantWallEdges; /* 0x4d - init false */
    u8 pad_4e[2];

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~dBgW_Kc();                              /* slots 0/1 */
    virtual void Virtual08();                             /* slot 2 */
    virtual void GetSurfaceInfo(s16 triID, SurfaceInfo &res); /* slot 3 - ITCM */
    virtual void GetNormal(s16 triID, Vector3 &res);      /* slot 4 - ITCM */
    virtual void GetTriangleOrigin(s16 triID, Vector3 &res); /* slot 5 - ITCM */
    virtual int DetectClsn(dBgCh_Gnd &ray);           /* slot 6 - ITCM */
    virtual int DetectClsn(dBgCh_Lin &ray);             /* slot 7 - ITCM */
    virtual int DetectClsn(dBgCh_SphCrr &sphere);           /* slot 8 - ITCM */

    /* C1 and the C2 base-subobject variant are defined in
       src/engine/collision/d_bg_w_kc.cpp (notes/ctor-migration.md section 2). */
    dBgW_Kc();

    /* --- non-virtual --- */
    void SetFile(KCL_File *file, CLPS_Block &clps);
    /* Both ITCM, both reading kclFile->origin.y out of the file's 1/64 scale.
       GetUnkOctreeY adds the octree's Y extent, giving its far edge. */
    Fix12i GetOctreeOriginY() const;   /* 0x01ffb0ec */
    Fix12i GetUnkOctreeY() const;      /* 0x01ffb0d0 */

    /* --- static --- */
    static char *LoadFile(SharedFilePtr &ptr);
    static void UpdateFileOffsets(KCL_File &file);

    /* WHAT LETS A REAL `~Class()` REPRODUCE THE ROM'S DELETING DESTRUCTOR.
       The compiler generates D0 as "run the destructor body, then call operator
       delete on the class". Without this it emits the global `_ZdlPv`, which
       exists nowhere in this image, and the D0 comes out one relocated word
       different from the ROM -- a difference build_pin.verify CANNOT SEE,
       because it wildcards relocated words. Only the link catches it.

       This family deallocates through Memory::operator_delete2, not the actor
       heap: every D0 below ends with a call to 0x0203cbcc. dActor_c's copy of this
       member calls Memory::Deallocate instead, which is why each needs its own.

       Inline, and in the IMMEDIATE base -- mwcc inlines it only when it finds it
       in the class or one level up, as include/dActor_c.h records. No layout
       effect: a non-virtual inline member adds no field and no vtable slot. */
    void operator delete(void *ptr) {
        Memory::operator_delete2(ptr);
    }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dBgW_Kc_size_must_be_0x50[sizeof(dBgW_Kc) == 0x50 ? 1 : -1];
typedef char dBgW_Kc_PlainVec_size_must_be_0xc[sizeof(dBgW_Kc::PlainVec) == 0xc ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DBGW_KC_H */
