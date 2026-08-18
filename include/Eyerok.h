/* class Eyerok : dBgActor_c. Real C++ form.
 *
 * Base and size from the factory (src/Eyerok_Spawn.cpp): fBase_c::operator
 * new(0x874), then dBgActor_c::dBgActor_c(), then stores _ZTV6Eyerok. Two
 * vtable stores in the destructor -- own, then dBgActor_c's -- confirming a
 * DIRECT dBgActor_c child, no intermediate. The apparent third store some
 * tooling flagged is `extern int _ZN7Vector3D1Ev[];` passed as a callback
 * pointer to `__destroy_arr` (see the D1 body) -- a literal-pool FUNCTION
 * POINTER argument, not a vptr store; it never writes to `this`.
 *
 * dBgActor_c ends at 0x320. Every member below closes exactly on the next
 * (confirmed against src/Eyerok_Spawn.cpp and src/_ZN6EyerokD1Ev.c, which
 * construct/destroy each in this order):
 *
 *     MovingCylinderClsnWithPos  0x320 + 0x40 = 0x360
 *     BlendModelAnim             0x360 + 0x70 = 0x3d0
 *     Model                      0x3d0 + 0x50 = 0x420
 *     ShadowModel                0x420 + 0x28 = 0x448
 *     TextureSequence            0x448 + 0x14 = 0x45c
 *
 * 0x45c..0x4d6 is a run of individually evidenced scalars (unchanged from
 * the AUTO-GENERATED header). At 0x4dc, Vector3[0x14] (0xc == sizeof(Vector3))
 * -- destroyed with __destroy_arr(ptr, 0x14, 0xc, _ZN7Vector3D1Ev), same
 * evidence shape as include/Unagi.h's mStarUniqueID -- ends at 0x5cc, then
 * 0xa8 more unevidenced bytes (only raw `this + 0x4dc` / `+ 0x4e0` /
 * `+ 0x4e4` -- one Vector3 -- are indexed by name in src/, so the array's
 * OWN span is trusted from the destructor call, not the padding after it)
 * before 0x674, a second, class-owned MovingMeshCollider (named by
 * _ZN18MovingMeshColliderD1Ev in the destructor), distinct from
 * dBgActor_c's own at 0x124. The last 0x38 bytes (0x83c..0x874) are unused
 * tail: SIZE IS 0x874, the literal src/Eyerok_Spawn.cpp passes to operator
 * new, not the field span -- same rule as BigBrickBlock.
 */
#ifndef EYEROK_H
#define EYEROK_H
#include "types.h"

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "MovingCylinderClsnWithPos.h"
#include "BlendModelAnim.h"
#include "ShadowModel.h"
#include "TextureSequence.h"

struct Eyerok : dBgActor_c {
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos;  /* 0x320 */
    BlendModelAnim mBlendModelAnim;                        /* 0x360 */
    Model mModel2;                                         /* 0x3d0 */
    ShadowModel mShadowModel;                               /* 0x420 */
    TextureSequence mTextureSequence;                       /* 0x448 */
    u8  unk_45c[0x30];
    u8  unk_48c;            /* 0x48c */
    u8  pad_48d[0xf];
    s32 unk_49c;            /* 0x49c */
    u8  pad_4a0[0x8];
    s32 unk_4a8;            /* 0x4a8 */
    u8  pad_4ac[0x8];
    s32 unk_4b4;            /* 0x4b4 */
    u8  pad_4b8[0x18];
    u8  unk_4d0;            /* 0x4d0 */
    u8  pad_4d1[0x1];
    s16 unk_4d2;            /* 0x4d2 */
    u16 unk_4d4;            /* 0x4d4 */
    u8  pad_4d6[0x6];
    /* The ROM destroys this with __destroy_arr(this + 0x4dc, 0x14, 0xc,
       _ZN7Vector3D1Ev) -- 0x14 elements, 0xc == sizeof(Vector3), same
       evidence shape as include/Unagi.h's mStarUniqueID. Only raw
       `this + 0x4dc` / `+ 0x4e0` / `+ 0x4e4` offsets are read elsewhere
       (one Vector3), so the count is trusted from the destructor call, not
       from any indexed access. */
    Vector3 mUnkVectors[0x14];    /* 0x4dc */
    u8  pad_5cc[0xa8];
    MovingMeshCollider unk_674;            /* 0x674 -- this class's own, not dBgActor_c's */
    /* 0x83c..0x874: unused tail, never read or written by any matched
       function; SIZE IS 0x874, the literal src/Eyerok_Spawn.cpp passes to
       operator new, not the field span -- same rule as BigBrickBlock. */
    u8  pad_83c[0x38];

    /* --- vtable --- */
    virtual ~Eyerok();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    /* Slot 29, dActor_c's egg-aim callback (include/dActor_c.h). Attributed
       by the vtable: _ZTV6Eyerok + 4*29 = 0x0211ad64 + 0x74 = 0x0211ade8;
       config/arm9/overlays/ov066/relocs.txt confirms 0x0211ade8 -> 0x0211a2dc. */
    int OnAimedAtWithEgg();  /* slot 29 */
};

typedef char Eyerok_size_must_be_0x874[sizeof(Eyerok) == 0x874 ? 1 : -1];

#else

#include "Model.h"

/* The C spelling of the same object, flat. Retained for any leftover C
   translation unit, same arrangement as include/ShadowModel.h. */
struct Eyerok {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    u8  mModel1;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  unk_124;            /* 0x124 */
    u8  pad_125[0x1fb];
    u8  mMovingCylinderClsnWithPos;            /* 0x320 */
    u8  pad_321[0x33];
    s32 unk_354;            /* 0x354 */
    s32 unk_358;            /* 0x358 */
    s32 unk_35c;            /* 0x35c */
    u8  mBlendModelAnim;            /* 0x360 */
    u8  pad_361[0x6f];
    Model mModel2;            /* 0x3d0 */
    u8  mShadowModel;            /* 0x420 */
    u8  pad_421[0x27];
    u8  mTextureSequence;            /* 0x448 */
    u8  pad_449[0x43];
    u8  unk_48c;            /* 0x48c */
    u8  pad_48d[0xf];
    s32 unk_49c;            /* 0x49c */
    u8  pad_4a0[0x8];
    s32 unk_4a8;            /* 0x4a8 */
    u8  pad_4ac[0x8];
    s32 unk_4b4;            /* 0x4b4 */
    u8  pad_4b8[0x18];
    u8  unk_4d0;            /* 0x4d0 */
    u8  pad_4d1[0x1];
    s16 unk_4d2;            /* 0x4d2 */
    u16 unk_4d4;            /* 0x4d4 */
    u8  pad_4d6[0x19e];
    u8  unk_674;            /* 0x674 */
};

#endif /* __cplusplus */

#endif
