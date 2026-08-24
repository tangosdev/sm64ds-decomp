#ifndef DADGR_C_H
#define DADGR_C_H

#include "types.h"
#include "dBgW_KcMbg.h"
#include "Model.h"

#ifdef __cplusplus

#include "dBgActor_c.h"

/* No header existed under any name -- confirmed against build/rtti.json (RTTI
 * record ov025:0x02113730, mangled `7daDgr_c`) and against include/decl_common.h,
 * which carried only a bare `extern int _ZTV7daDgr_c[];` placeholder. The RTTI
 * name is used exactly as spelled; no pretty name is coined.
 *
 * BASE: dBgActor_c, direct. The RTTI edge (base_key ov002:0x021089ec, offset 0)
 * agrees with the destructor: func_ov025_021111a0 (D1, vtable slot 16) stores
 * exactly two vtables in sequence -- `data_ov025_02113760` (its own) then
 * `_ZTV10dBgActor_c` -- which is what one intermediate-free inlined base looks
 * like. dBgActor_c's own destructor is declared inline in its class body, so
 * every direct child inlines its body rather than calling it out of line.
 *
 * SIZE IS 0x334 (820 decimal), THE LITERAL THE FACTORY PASSES TO operator new --
 * not the field span a naive read of the destructor gives. The factory,
 * func_ov025_02111898, is a trivial-constructor shape: `fBase_c::operator
 * new(820)`, `dBgActor_c::dBgActor_c()`, store `_ZTV7daDgr_c`. No dedicated
 * "daDgr_c()" constructor exists in the ROM -- the derived class's own ctor
 * body is empty and gets folded into the factory, the same shape
 * include/BigBrickBlock.h's family uses.
 *
 * MEMBERS. dBgActor_c ends at 0x320; daDgr_c adds 0x14 more bytes and destructs
 * none of them (func_ov025_021111a0 tears down only dBgActor_c's own Model and
 * dBgW_KcMbg, then chains to dActor_c -- nothing at 0x320 or later).
 * That means everything from 0x320 to 0x334 is POD, read from InitResources
 * (func_ov025_021117dc) and Behavior (func_ov025_021113f0):
 *
 *   0x320  mBasePosY       -- InitResources copies mPosY (dActor_c +0x60) here;
 *                              Behavior reads it back as the resting height a
 *                              sine offset is added to.
 *   0x324  mAngleXSpeed    -- the per-tick angular step, +-(0x400/n), added to
 *                              mAngleX every frame of a swing; Behavior only.
 *   0x326  mPhaseTimer     -- a phase counter Behavior increments every call and
 *                              compares against several thresholds.
 *   0x328  mSwingStage     -- a stage counter, -1 sentinel, incremented up to
 *                              0x14 and read as `10 - value` to shape the speed.
 *   0x329  mSwingDir       -- toggled with XOR 1 at the end of a swing; it picks
 *                              the sign of mAngleXSpeed and of the Z step.
 *   0x32a  --              -- never dereferenced by any function in the class;
 *                              2 bytes of padding closing the s8/u8 pair up to
 *                              the next s32.
 *   0x32c  mDustParticle1  -- a Particle::System::New() handle (effect 0x2d),
 *                              read back as that call's own first argument
 *                              (handle recycling); positioned 100 units along
 *                              the platform's facing.
 *   0x330  mDustParticle2  -- the same handle shape for the other end, 100
 *                              units the opposite way.
 *
 * 0x320 + 0x14 = 0x334, closing exactly on the factory's literal. Types are
 * proven from the accesses above, not guessed; the per-offset citations are in
 * notes/bgobject-provenance.md.
 */

struct daDgr_c : dBgActor_c {
    s32 mBasePosY;
    s16 mAngleXSpeed;
    u16 mPhaseTimer;
    s8  mSwingStage;
    u8  mSwingDir;
    u8  pad_32a[0x2];
    u32 mDustParticle1;
    u32 mDustParticle2;

    /* --- vtable, in ROM order. Do not reorder. ---
     * A leaf class (no RTTI children), so the destructor is declared OUT OF
     * LINE here rather than inline, matching every other leaf under
     * dBgActor_c (e.g. include/BigBrickBlock.h). Defined as a real method in
     * src/_ZN7daDgr_cD1Ev.cpp / src/_ZN7daDgr_cD0Ev.cpp -- both empty bodies,
     * because this class owns no member with its own destructor; the compiler
     * emits the two inherited vtable stores and the two dBgActor_c member
     * teardowns on its own. */
    virtual ~daDgr_c();

    /* --- overrides of inherited fBase_c slots. Each takes its base's index
     *     (see include/fBase_c.h for the full 32-slot table). --- */
    virtual s32 InitResources();       /* slot 0 */
    virtual s32 CleanupResources();    /* slot 3 */
    virtual s32 Behavior();            /* slot 6 */
    virtual s32 Render();              /* slot 9 */

    /* slot 31 (Kill) is NOT overridden: the vtable word at data_ov025_02113760
       + 0x7c is 0x020ee55c, identical to dBgActor_c's own slot 31 -- confirmed
       by reloc, not assumed. */
};

typedef char daDgr_c_size_must_be_0x334[sizeof(daDgr_c) == 0x334 ? 1 : -1];

#else

/* The same object for a C translation unit, which has no base class to
   inherit dBgActor_c's fields from and so spells the whole layout flat.
   dBgActor_c's own C-side layout is duplicated from include/dBgActor_c.h. */
struct daDgr_c {
    u8  pad_000[0x5c];
    s32 mPosX;                   /* 0x05c */
    s32 mPosY;                   /* 0x060 */
    s32 mPosZ;                   /* 0x064 */
    u8  pad_068[0xc];
    s32 mCamSpacePosX;           /* 0x074 */
    s32 mCamSpacePosY;           /* 0x078 */
    s32 mCamSpacePosZ;           /* 0x07c */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;                 /* 0x08c */
    s16 mAngleY;                 /* 0x08e */
    u8  pad_090[0x4];
    s16 mPrevAngleY;             /* 0x094 */
    u8  pad_096[0x2];
    s32 mHorzSpeed;              /* 0x098 */
    s32 mVertAccel;              /* 0x09c */
    s32 mTerminalVelocity;       /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;              /* 0x0a8 */
    u8  pad_0ac[0x4];
    s32 mFlags;                  /* 0x0b0 */
    s32 mClipOffsetY;            /* 0x0b4 */
    s32 mClipRadius;             /* 0x0b8 */
    u8  pad_0bc[0x18];
    /* Model member. The cartridge's own ~daDgr_c calls _ZN5ModelD1Ev at +0x0d4 (D0/D1),
       a relocation the ROM build checks; recovered by tools/dtor_members.py. D1 and not
       D2, so it is this type and not an inlined base. The marker's pad stopped short
       of the object, so the member also takes over unk_0f0 (+0x1c = mat4x3) and
       unk_114/unk_118/unk_11c (+0x40/+0x44/+0x48 = mat4x3.t.x/.y/.z), which the
       header declared separately inside it. */
    Model mModel;                /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~daDgr_c calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;    /* 0x124 */
    struct Matrix4x3 mClsnMat;   /* 0x2ec */
    u8  unk_31c;                 /* 0x31c */
    u8  unk_31d;                 /* 0x31d */
    /* daDgr_c's own bytes, from the header comment above. */
    s32 mBasePosY;               /* 0x320 */
    s16 mAngleXSpeed;            /* 0x324 */
    u16 mPhaseTimer;             /* 0x326 */
    s8  mSwingStage;             /* 0x328 */
    u8  mSwingDir;               /* 0x329 */
    u8  pad_32a[0x2];
    u32 mDustParticle1;          /* 0x32c */
    u32 mDustParticle2;          /* 0x330 */
};

#endif /* __cplusplus */

#endif
