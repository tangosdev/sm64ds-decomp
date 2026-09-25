//cpp
/**
 * Tall Tall Mountain's ukiki cage (profile HM_BASKET).
 *
 * Hangs from its mesh collider until something gives it downward
 * acceleration; from then on it falls, and the star actor it spawned at
 * InitResources rides 0x3c000 above it. When the falling cage lands,
 * func_ov030_0211124c destroys both it and the star and puffs two particle
 * effects where they were.
 *
 * daObjHmBskt_c_classInit is a reconstructed source-style name: SM64DS proves
 * the RTTI class, the HM_BASKET registry ID, the descriptor/factory
 * relationship, the allocation size and the vtable identity; later EAD lineage
 * supplies the spelling prior. The exact original SM64DS spelling is not
 * preserved. Historical alias: UkikiCage_Spawn. Coined C name; it makes no
 * parameter-type claim.
 *
 * deslop
 * Leftover: func_ov030_0211124c keeps its address-derived name and its
 *   char* parameters. The ROM proves the body, the call site and the class
 *   that owns the call; it proves no name and no parameter types, and
 *   include/decl_common.h already declares this exact spelling.
 * Leftover: dBgActor_c::IsClsnInRangeOnScreen, dBgW_KcMbg::SetFile and
 *   dBgCh_Actr::Init stay mangled -- each takes Fix12<int> by value, the
 *   known mwccarm language-mode wall, and a typed call homes the argument.
 * Leftover: Behavior needs the four-word volatile stack reservation to
 *   reproduce the cartridge's frame; without it the body is 17 words short.
 * Leftover: data_ov030_02115c80 / 02115c88 are this overlay's KCL and BMD
 *   handles (InitResources LoadFile / CleanupResources Release), and
 *   data_ov030_02114ee4 is the CLPS block SetFile is handed. All three live
 *   in overlay .data and .bss this TU does not own.
 * Leftover: g_profile_HM_BASKET lives outside this TU.
 * Leftover: inline destructor (out of line emits D0 before D1).
 */

#include "daObjHmBskt_c.h"
#include "SharedFilePtr.h"

struct KCL_File;
struct CLPS_Block;

extern "C" {
extern SharedFilePtr data_ov030_02115c80; /* the KCL */
extern SharedFilePtr data_ov030_02115c88; /* the BMD */
extern CLPS_Block data_ov030_02114ee4;

int func_ov030_0211124c(char *c, char *mc);

int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(
    dBgActor_c *self, s32 range, s32 offsetY);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat,
    Fix12i scale, s16 angle, CLPS_Block *clps);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    dBgCh_Actr *self, dActor_c *actor, Fix12i radius, Fix12i height,
    Vector3_16 *a, Vector3_16 *b);

void dBgCh_Actr_UpdateContinuous_Veneer(void *);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    unsigned int, int, int, int);
}

// @symbol daObjHmBskt_c_classInit
extern "C" daObjHmBskt_c *daObjHmBskt_c_classInit()
{
    return new daObjHmBskt_c();
}

// @symbol _ZN13daObjHmBskt_c13InitResourcesEv
s32 daObjHmBskt_c::InitResources()
{
    BMD_File *modelFile = (BMD_File *)Model::LoadFile(data_ov030_02115c88);
    mModel.SetFile(modelFile, 1, -1);

    mVertAccel = 0;
    mTerminalVelocity = 0;
    mHorzSpeed = 0;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;

    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *clsnFile = (KCL_File *)dBgW_Kc::LoadFile(data_ov030_02115c80);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, clsnFile, &mClsnMat, 0x199, mAngleY,
        &data_ov030_02114ee4);

    Vector3 starPos;
    starPos.x = mPosX;
    starPos.y = mPosY;
    starPos.z = mPosZ;
    starPos.y += 0x3c000;
    mStarActor = dActor_c::Spawn(
        0xb2, (u8)(param1 & 0xf) | 0x50, starPos, 0, mAreaId, -1);

    return 1;
}

// @symbol _ZN13daObjHmBskt_c8BehaviorEv
s32 daObjHmBskt_c::Behavior()
{
    volatile int dummy[4];
    (void)&dummy;

    _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0);
    mAngleY = mAngleY + mHorzSpeed;

    if (mVertAccel != 0) {
        mFlags &= ~1;
        UpdatePos(0);
        func_ov030_0211124c((char *)this, (char *)&mWithMeshClsn);

        s32 y = mPosY;
        s32 z = mPosZ;
        dActor_c *star = mStarActor;
        s32 x = mPosX;
        star->mPosX = x;
        star->mPosY = y + 0x3c000;
        star->mPosZ = z;
    }

    return 1;
}

// @symbol _ZN13daObjHmBskt_c6RenderEv
s32 daObjHmBskt_c::Render()
{
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mModel.Render(0);
    return 1;
}

// @symbol _ZN13daObjHmBskt_c16CleanupResourcesEv
s32 daObjHmBskt_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov030_02115c88.Release();
    data_ov030_02115c80.Release();
    return 1;
}

// @symbol func_ov030_0211124c
extern "C" int func_ov030_0211124c(char *c, char *mc)
{
    dBgCh_Actr_UpdateContinuous_Veneer(mc);
    int g = ((dBgCh_Actr *)mc)->IsOnGround();
    if (!g) return g;
    ((fBase_c *)*(void **)(c + 0x4dc))->MarkForDestruction();
    int px = *(int *)(c + 0x5c);
    int pz = *(int *)(c + 0x64);
    int py = *(int *)(c + 0x60) + 0x96000;
    Vector3 p = { px, py, pz };
    dActor_c::Spawn(
        0xb2, (unsigned char)((*(unsigned int *)(c + 8)) & 0xf) | 0x20, p, 0,
        *(signed char *)(c + 0xcc), -1);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
        0xb, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
        0xd, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
    ((fBase_c *)c)->MarkForDestruction();
}

/* _ZN13daObjHmBskt_cD1Ev and _ZN13daObjHmBskt_cD0Ev are NOT WRITTEN HERE ON
   PURPOSE, and carry no @symbol marker. The inline destructor in
   include/daObjHmBskt_c.h emits D1 then D0 -- the cartridge's order -- and no
   D2, and tools/tiers.py scores both through that inline definition. */
