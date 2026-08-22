/* Hand-written from matched-function evidence:
 * class CastleWater, ov009 0x02111a70-0x02111dc4 (8 functions, no other class
 * in the TU -- tu_map.py).
 *
 * The ROM knows this class by TWO names. Its symbols mangle from `CastleWater`
 * (_ZN11CastleWater8BehaviorEv and friends), which is why the struct is spelled
 * that way here -- the mangling has to come out right. Its RTTI record calls it
 * `daObjMcWater_c`, EAD's own name, and include/daObjMcWater_c.h is the
 * generated view under that name. The two describe the same object; this header
 * is the one methods can be defined against.
 *
 * It is a dBgActor_c: InitResources calls dBgActor_c::UpdateModelPosAndRotY and
 * dBgActor_c::UpdateClsnPosAndRot on itself, and mPos/mAngleY sit at dActor_c's
 * offsets. Written FLAT with the inherited slots restated, as every other
 * generated header here is.
 *
 * Field NAMES are placeholders - renaming cannot change codegen.
 */
#ifndef CASTLEWATER_H
#define CASTLEWATER_H
#include "types.h"
#include "math/Matrix.h"
#include "TextureTransformer.h"
#include "dBgW_KcMbg.h"
#include "Model.h"

struct CastleWater {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    /* The water plane's height, and it is read by more than this object:
       InitResources lowers the GLOBAL data_0209f32c to mPosY - 0x64000 if that
       is lower, so the water publishes a kill/blur plane the rest of the level
       reads. One level flag also drops the water itself to -0x2bc000. */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member. The cartridge's own ~CastleWater calls _ZN5ModelD1Ev at +0x0d4
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~CastleWater calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    /* Real type, not a marker: InitResources hands it to
       dBgW_KcMbg::SetFile through an explicit `Matrix4x3&` cast. */
    Matrix4x3 mMatrix;            /* 0x2ec */
    u8  pad_31c[0x4];
    TextureTransformer mTexTransformer; /* 0x320 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
