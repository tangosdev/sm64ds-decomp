/* GENERATED from include/dBgW.h by tmp/crash9/mkshadow.py, run link100 lane CRASH9.
 * DO NOT EDIT BY HAND -- regenerate, so the diff against include/dBgW.h stays
 * exactly the two changes named below and nothing drifts in.
 *
 * WHY THIS FILE EXISTS. The cartridge's dBgW vtable has THIRTEEN slots and the
 * three DetectClsn overloads sit at 6/7/8 in the order Gnd, Lin, SphCrr. Read
 * straight out of extracted/arm9_dec.bin (base 0x02004000), _ZTV7dBgW_Kc at
 * arm9 0x020993dc:
 *
 *     slot 6  +0x18  _ZN7dBgW_Kc10DetectClsnER9dBgCh_Gnd
 *     slot 7  +0x1c  _ZN7dBgW_Kc10DetectClsnER9dBgCh_Lin
 *     slot 8  +0x20  _ZN7dBgW_Kc10DetectClsnER12dBgCh_SphCrr
 *
 * MSVC built a TWELVE-slot table for the same declaration and put Lin at +0x18,
 * for two independent reasons, both of which port/hal/clsn_vtable.cpp already
 * records in writing:
 *
 *   - it spends ONE vtable slot on the destructor where the ROM's Itanium
 *     layout spends two (D1 then D0), so everything below shifts up one;
 *   - it emits a run of ADJACENT same-name overloads in REVERSE declaration
 *     order, so Gnd/Lin/SphCrr came out SphCrr/Lin/Gnd.
 *
 * Twelve call sites in the link dispatch these slots by ROM INDEX through
 * hand-written opaque stand-ins (src/_ZN9dBgCh_Gnd10DetectClsnEv.cpp's
 * `struct ClsnActor { virtual int v0(); ... v6(void *); }` and five siblings),
 * so a ground raycast was reaching DetectClsn(dBgCh_Lin &). That body treats
 * the object as a dBgCh_Lin, which is 0x68 bytes against dBgCh_Gnd's 0x50, and
 * writes a Vector3 at +0x54..+0x5f. SignPost::InitResources keeps its
 * dBgCh_Gnd at ebp-0x6c with the stack cookie at ebp-0x10 == ray+0x5c, so the
 * last word of that copy landed on the cookie and Windows killed the process
 * with STATUS_STACK_BUFFER_OVERRUN before the level finished spawning.
 *
 * NEITHER CHANGE MOVES A FIELD. A virtual adds no data member and the vptr is
 * already there, so sizeof(dBgW) stays 0x20 and every offset in this header is
 * untouched. And mwccarm never sees this file, so the NDS build is unaffected:
 * the ROM-side declaration in include/dBgW.h is the one that compiles there,
 * and it is already correct.
 *
 * THE THREE REAL C++ VIRTUAL CALLS into dBgW slots (GetSurfaceInfo,
 * GetNormal, GetVelocity) need no adjustment: they are recompiled against this
 * same declaration, so they move to slots 3, 4 and 12, which is exactly where
 * the cartridge's table carries them.
 */
#ifndef DBGW_H
#define DBGW_H
#define DBGW_H_HOST_ABI_SHADOW 1

#include "types.h"

/* The root of the mesh-collision hierarchy, vtable _ZTV4dBgW
 * at 0x02099388. The chain: dBgW -> dBgW_Kc (static KCL
 * mesh) and dBgW -> ... -> dBgW_KcMbg ->
 * dBgW_KcMbgSclY.
 *
 * VTABLE, 13 slots, read out of the ROM:
 *
 *   slot  0/1  ~dBgW (see THE TWO DESTRUCTOR BODIES below)
 *   slot  2    Virtual08()                      - no-op here
 *   slot  3    null - pure; derived tables hold GetSurfaceInfo in ITCM
 *              (0x01ffd920, named in config/arm9/itcm/symbols.txt)
 *   slot  4    null - pure; GetNormal(s16, Vector3 &) in derived tables
 *   slot  5    null - pure; GetTriangleOrigin(s16, Vector3 &) in derived
 *   slot  6    DetectClsn(dBgCh_Gnd &)      - returns false here
 *   slot  7    DetectClsn(dBgCh_Lin &)        - returns false here
 *   slot  8    DetectClsn(dBgCh_SphCrr &)         - returns false here
 *   slot  9    BeforeClsn(...)                  - invokes the callback
 *   slot 10    TransformPos(const Vector3 &, Vector3 &) - false here
 *   slot 11    GetAngularVelY()                 - 0 here
 *   slot 12    GetVelocity(Vector3 &)           - zeroes it here
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND NEVER DEFINED AS A METHOD -- the
 * key-function arrangement from include/ModelBase.h.
 *
 * THE TWO DESTRUCTOR BODIES. Both are `ldr r1,[pc,#4]; str r1,[r0]; bx lr'
 * storing the vtable word, and they are byte-identical -- but the compiler
 * emitted BOTH, so this is not a one-body alias:
 *
 *   0x0203968c  sits in vtable slot 0, and nothing bl's it   -> D1
 *   0x02039658  bl'd by dBgW_Kc's D0/D1/D2, never in a
 *               vtable (currently named _ZN4dBgWD2Ev)       -> D2
 *
 * By the #774 rule (a class vtable carries [D1, D0] and never D2; D2 is
 * reached by direct call alone) the symbol names are CORRECT as they stand:
 * symbols.txt calls 0x0203968c `_ZN4dBgWD1Ev' and 0x02039658
 * `_ZN4dBgWD2Ev', which is exactly what the two bullets above
 * derive. #1203 settled it.
 *
 * Re-verified 2026-08-19 against the cartridge: the word in vtable slot 0
 * (0x02099388) is 0x0203968c; that address has ONE reference in
 * config/arm9/relocs.txt -- the vtable itself -- and 0x02039658 has THREE,
 * the direct calls. D1-in-vtable, D2-direct-called, as the rule requires.
 *
 * This paragraph used to conclude the opposite ("the symbol names are on the
 * wrong bodies") while the two bullets directly above it said otherwise, and
 * it asked for a symbols.txt rename. Do not make that rename; nothing here
 * is misnamed.
 *
 * dBgW_Kc next door emits a byte-identical D1/D2 pair too (0x02039864
 * in the vtable, 0x020397fc direct-called), so "only one body is ever
 * emitted" does not hold for this family.
 *
 * LAYOUT is pinned by the shared init func_02039624 (called from C2) and
 * Enable/Disable/IsEnabled: 0x18 in the slot byte means "not enabled",
 * and enabled colliders live in the 0x18-entry table at data_020a0c80.
 */

#ifdef __cplusplus

struct dActor_c;
struct dBgPi;
struct dBgCh_Gnd;
struct dBgCh_Lin;
struct dBgCh_SphCrr;
struct SurfaceInfo;

#ifndef VECTOR3_16_DEFINED
#define VECTOR3_16_DEFINED
/* The game's own name for the s16 vector, as its mangled symbols spell it. */
struct Vector3_16 { s16 x, y, z; };
#endif

extern "C" void _ZN6Memory16operator_delete2EPv(void *);

struct dBgW {
    /* 0x00 is the vptr, placed implicitly by the first virtual declaration. */
    dActor_c *owner;             /* 0x04 - set by Enable, null when free */
    s32 ownerUniqueID;        /* 0x08 - owner->uniqueID, -1 when free */
    Fix12i unk_0c;            /* 0x0c - init -0x1000 */
    s32 unk_10;               /* 0x10 */
    u8 slotIdx;               /* 0x14 - index into data_020a0c80; 0x18 = disabled */
    u8 pad_15[3];
    void (*beforeClsnCallback)(dBgW *self, dActor_c *actor,
                               dBgPi *res, Vector3 *pos,
                               Vector3_16 *motionAng, Vector3_16 *ang); /* 0x18 */
    s32 unk_1c;               /* 0x1c */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~dBgW();                          /* slots 0/1 */
    /* HOST ONLY, ADDED BY dBgW_HostAbi.h. The ROM's vtable spends slot 1 on
       the deleting destructor D0; MSVC spends no slot on it at all. Without a
       filler here every slot below sits one early and the twelve ROM-index
       dispatch sites read the wrong function. Inline and no-op: it adds a slot
       and nothing else. It is never dispatched by name -- only ROM code would
       reach slot 1, and only to delete a collider, which the port does through
       the MSVC destructor in slot 0. */
    virtual void HostRomD0SlotPad() {}     /* slot 1 - the ROM's D0 */
    virtual void Virtual08();                             /* slot 2 */
    virtual void GetSurfaceInfo(s16 triID, SurfaceInfo &res) = 0; /* slot 3 - ITCM in derived */
    virtual void GetNormal(s16 triID, Vector3 &res) = 0;  /* slot 4 */
    virtual void GetTriangleOrigin(s16 triID, Vector3 &res) = 0; /* slot 5 */
    virtual int DetectClsn(dBgCh_SphCrr &sphere);           /* slot 8 */
    virtual int DetectClsn(dBgCh_Lin &ray);             /* slot 7 */
    virtual int DetectClsn(dBgCh_Gnd &ray);           /* slot 6 */
    virtual void BeforeClsn(dBgPi &res, dActor_c *actor, Vector3 &pos,
                            Vector3_16 *motionAng, Vector3_16 *ang); /* slot 9 */
    virtual int TransformPos(const Vector3 &pos, Vector3 &res);      /* slot 10 */
    virtual s16 GetAngularVelY();                         /* slot 11 */
    virtual void GetVelocity(Vector3 &res);               /* slot 12 */

    /* DECLARED, never defined as a method here -- src/_ZN4dBgWC2Ev.cpp owns
       the one out-of-line definition, and declaring it is what makes derived
       constructors emit `bl _ZN4dBgWC2Ev' instead of synthesising the base
       step inline (notes/ctor-migration.md section 2). */
    dBgW();

    /* --- non-virtual --- */
    int Enable(dActor_c *actor);
    int Disable();
    int IsEnabled();

    /* --- static: the stock BeforeClsn callbacks. Call sites take their
       addresses and store them via func_020393d4; BeforeClsn invokes the
       stored one as (collider, actor, res, pos, motionAng, ang). They
       update POS from the collider's motion; the dBgPi rides along
       untouched. --- */
    static void UpdatePosAndAngs(dBgW &clsn, dActor_c *clsnActor,
                                 dBgPi &res, Vector3 &pos,
                                 Vector3_16 *motionAng, Vector3_16 *ang);
    static void UpdatePosWithVelocity(dBgW &clsn, dActor_c *clsnActor,
                                      dBgPi &res, Vector3 &pos,
                                      Vector3_16 *motionAng, Vector3_16 *ang);
    static void UpdatePosWithTransform(dBgW &clsn, dActor_c *clsnActor,
                                       dBgPi &res, Vector3 &pos,
                                       Vector3_16 *motionAng, Vector3_16 *ang);
    static void UpdateAngsWithAngularVelY(dBgW &clsn, dActor_c *clsnActor,
                                          dBgPi &res, Vector3 &pos,
                                          Vector3_16 *motionAng, Vector3_16 *ang);

    /* This family deallocates through Memory::operator_delete2 (0x0203cbcc),
       not the actor heap. CW inlines operator delete into the deleting
       destructor (D0) only when it finds one on the class or its immediate
       base, so it has to be spelt here. Without it D0 calls the global
       _ZdlPv (0x0203cbf0) and still BYTE-MATCHES -- match.compare wildcards
       every relocated word -- while calling the wrong function; only the
       reloc-destination check sees it. No layout effect: a non-virtual
       inline member adds no field and no vtable slot. */
    void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dBgW_size_must_be_0x20[sizeof(dBgW) == 0x20 ? 1 : -1];
#endif

#else

struct dBgW {
    void **vtable;            /* 0x00 */
    struct dActor_c *owner;      /* 0x04 */
    s32 ownerUniqueID;        /* 0x08 */
    s32 unk_0c;               /* 0x0c */
    s32 unk_10;               /* 0x10 */
    u8 slotIdx;               /* 0x14 */
    u8 pad_15[3];
    void *beforeClsnCallback; /* 0x18 */
    s32 unk_1c;               /* 0x1c */
};

#endif /* __cplusplus */

#endif /* DBGW_H */
