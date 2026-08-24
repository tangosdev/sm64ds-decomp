#ifndef DBGW_KC_H
#define DBGW_KC_H

#include "types.h"
#include "dBgW.h"

/* The static-mesh collider, vtable _ZTV7dBgW_Kc at 0x020993dc.
 * Thirteen slots: it overrides Virtual08 and supplies the six collision
 * workers (the three pures at slots 3..5 and the DetectClsn triple at
 * 6..8) from ITCM - 0x01ffd920, 0x01ffd8d8, 0x01ffd890, 0x01ffd3f8,
 * 0x01ffb0fc, 0x01ffb830 - which is why those overrides are declared
 * here but defined nowhere in src/ yet. config/arm9/itcm/symbols.txt
 * already names slot 3's: _ZN7dBgW_Kc14GetSurfaceInfoEsR11SurfaceInfo,
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
    char *unk_0c;          /* 0x0c - the octree, indexed below */

    /* Octree geometry. UpdateFileOffsets relocates only 0x00..0x0c, so nothing
       from here on is a pointer. Positions and this origin are stored at 1/64 of
       a Fix12i unit (`lsl #6` on read) and face normals with 1.0 == 0x400
       (`lsl #2`), so none of these is typed Fix12i. */
    s32 unk_10;
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

extern "C" void _ZN6Memory16operator_delete2EPv(void *);

struct dBgW_Kc : dBgW {
    KCL_File *kclFile;        /* 0x20 */
    u32 clps;                 /* 0x24 - set via func_0203821c, released via func_02038224 */
    /* 0x28..0x30 are ONE Vector3, not three scalars: DetectClsn(dBgCh_SphCrr&)
       hands `this + 0x28` straight to DotVec3 as a vector (0x01ffc278,
       `add r1, sl, #0x28`) and compares the result against a contact angle. It
       is the collider's preferred-contact axis. Left as three fields only
       because retyping it would touch already-matched callers; if you retype it,
       re-verify SetFile and _ZN10dScEntry_c13InitResourcesEv (renamed from
       func_ov075_0211a410). */
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
    /* All four are consumed by DetectClsn(dBgCh_SphCrr&) as one edge-contact
       policy -- see notes/collision-query-classes.md.
         unk_44  the threshold DotVec3(faceNormal, unk_38) is tested against
         unk_48  a SHIFT COUNT, not a value: an edge hit is rejected when the
                 lateral distance outside the edge exceeds faceDot >> unk_48,
                 i.e. a slope tolerance as a fraction of the penetration
         unk_4c  master gate for the edge/vertex regions; clear means only a
                 face contact can register
         unk_4d  selects the tolerant form of the unk_48 test for walls */
    s32 unk_48;               /* 0x48 - init 2 */
    u8 unk_4c;                /* 0x4c - init 1 */
    u8 unk_4d;                /* 0x4d - init 0 */
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

    /* DECLARED, never defined as a method here -- src/_ZN7dBgW_KcC1Ev.cpp
       owns C1 and src/_ZN7dBgW_KcC2Ev.c the base-subobject variant
       (notes/ctor-migration.md section 2). */
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
    void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }

};

typedef char dBgW_Kc_size_must_be_0x50[sizeof(dBgW_Kc) == 0x50 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DBGW_KC_H */
