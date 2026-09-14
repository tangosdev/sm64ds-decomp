//cpp
/* Cannon Shutter (CANNON_SHUTTER 14) -- ov002/daObjCannonShutter_c.
 *
 * ov002 is mixed (yoshi egg / switches / stars / push block / player / ...).
 * RTTI ov002:0x02109cf0 names this class daObjCannonShutter_c; overlay_actors
 * and the debug table name the profile CANNON_SHUTTER. Base is dBgActor_c
 * (RTTI edge ov002:0x021089ec).
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder:
 * tools/rombuild.py refuses the object outright when the emitted order and the
 * ROM's disagree. The header's inline destructor makes mwccarm emit retail's
 * D1-then-D0 pair with no D2.
 *
 * deslop leftovers:
 * - dBgW_KcMbg::SetFile 6az: InitResources passes Fix12<int> by value; the
 *   header method form size-DIFFs (0xb8 against the cartridge's 0xac).
 * - dBgActor_c::IsClsnInRangeOnScreen 6az (Behavior; header method form
 *   refused). No dBgCh::Init in this TU.
 * - (Vector3 *)&mPosX / mHomePosX: dActor_c stores the triples as scalars;
 *   grouping as Vector3 is a shared-header campaign.
 * - data_ov002_0210e12c / 0210e124 SharedFilePtr handles and
 *   data_ov002_0210d7f4 CLPS; this TU consumes them, overlay .data/.bss owns
 *   them. S14: g_profile_CANNON_SHUTTER stays outside the licensed .text.
 * - func_ov002_020bc990: inbound from ov084; the ROM symbol is not a member.
 *   No coined name.
 * - func_0201277c (sound 0x47); data_020a0e68 scratch matrix;
 *   data_0209f2f8 / data_0209f220 scene/level gates.
 * - no Camera.h. Player.h is required for ClosestPlayer()->mIsMega.
 */

/* daObjCannonShutter_c.h FIRST: it pulls in dBgActor_c.h, which must reach
   common.h ahead of Model.h or the wrong Matrix4x3 spelling wins. */
#include "daObjCannonShutter_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "decl_common.h"

extern "C" {
void Matrix4x3_FromRotationY(Matrix4x3 *m, s16 angleY);
void MulVec3Mat4x3(const Vector3 *v, const Matrix4x3 *m, Vector3 *dst);
void AddVec3(const Vector3 *a, const Vector3 *b, Vector3 *dst);
int  Vec3_Dist(const Vector3 *a, const Vector3 *b);
int  Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
extern Matrix4x3 data_020a0e68;

/* dBgActor_c::IsClsnInRangeOnScreen(Fix12<int>, Fix12<int>) -- wall 6az. */
int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *self, int a, int b);

/* dBgW_KcMbg::SetFile -- wall 6az. See the call site in InitResources. */
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 &mat, int scale, s16 angY,
    CLPS_Block &clps);

extern signed char   data_0209f2f8;
extern unsigned char data_0209f220;

extern SharedFilePtr data_ov002_0210e12c;   /* the BMD model */
extern SharedFilePtr data_ov002_0210e124;   /* the KCL collision mesh */
extern CLPS_Block    data_ov002_0210d7f4;
}

/* Runtime actor/process profile descriptor at ov002:0x02109d14. The field
 * roles are established by the recovered fBase_c/dActor_c consumers; their
 * exact original SM64DS member spellings are not preserved. */
struct CannonShutterSpawnInfo {
    daObjCannonShutter_c *(*classInit)();
    s16 profileIDAndExecuteOrder;
    s16 drawOrder;
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};

typedef char CannonShutterSpawnInfo_size_must_be_0x1c[
    sizeof(CannonShutterSpawnInfo) == 0x1c ? 1 : -1];

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjCannonShutter_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjCannonShutter_c through
 * RTTI, allocation size, vtable identity, and the CANNON_SHUTTER registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: daObjCannonShutter_c_Spawn.
 *
 * Leaf operator new(unsigned long) forwards _ZN7fBase_cnwEj until a tree-wide
 * fBase_c::operator new lands. A natural `new daObjCannonShutter_c` then emits
 * the retail allocation/base/vptr seam (dBgActor_c C2, vptr). */
extern "C" daObjCannonShutter_c *daObjCannonShutter_c_classInit()
{
    return new daObjCannonShutter_c();
}

/* Reconstructed source-style names: SM64DS proves the RTTI class,
 * CANNON_SHUTTER registry ID, descriptor/factory relationship, and object
 * shape; later EAD lineage supplies the classInit/g_profile spelling prior.
 * Exact original SM64DS spellings are not preserved. Historical project
 * aliases: daObjCannonShutter_c_Spawn and daObjCannonShutter_c_SpawnInfo. */
extern "C" CannonShutterSpawnInfo g_profile_CANNON_SHUTTER = {
    daObjCannonShutter_c_classInit,
    0x000e,
    0x009e,
    0x00800002,
    0x00000000,
    0x00100000,
    0x01000000,
    0x00000000
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjCannonShutter_c13InitResourcesEv
int daObjCannonShutter_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov002_0210e12c), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    {
        KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov002_0210e124);
        /* 0x199 is 409/4096, i.e. a collider scale of 0.1. Mangled: wall 6az. */
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, mClsnMat, 0x199, mAngleY, data_ov002_0210d7f4);
    }
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    if (IsCannonOpenInCurLevel() != 0) {
        mCannonOpen = 1;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjCannonShutter_c8BehaviorEv
int daObjCannonShutter_c::Behavior()
{
    if (mCannonOpen != 0) {
        if (ClosestPlayer()->mIsMega != 0) {
            mPosX = mHomePosX;
            mPosY = mHomePosY;
            mPosZ = mHomePosZ;
            if (_ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0)) {
                UpdateModelPosAndRotY();
                UpdateClsnPosAndRot();
            }
        } else {
            if (mMeshCollider.IsEnabled()) {
                mMeshCollider.Disable();
            }
        }
        return 1;
    }
    if (mOpening != 0) {
        Vector3 slide;
        Vector3 rotated;
        slide.x = 0;
        slide.y = 0;
        slide.z = 0;
        rotated.x = 0;
        rotated.y = 0;
        rotated.z = 0;
        if (mOpenPhase != 0) {
            if (mOpenPhase == 1) slide.z = 0x2000;      /* +2.0 forward */
        } else {
            slide.y = -0x1000;                          /* -1.0 down */
        }
        Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
        MulVec3Mat4x3(&slide, &data_020a0e68, &rotated);
        AddVec3((Vector3 *)&mPosX, &rotated, (Vector3 *)&mPosX);
        if (Vec3_Dist((Vector3 *)&mHomePosX, (Vector3 *)&mPosX) > 0xa000) {
            mOpenPhase = 1;                             /* past 10.0: go forward */
        }
        if (Vec3_HorzDist((Vector3 *)&mHomePosX, (Vector3 *)&mPosX) > 0xc8000) {
            /* Past 200.0 out: the cannon is clear. Tell every shutter. */
            daObjCannonShutter_c *other =
                (daObjCannonShutter_c *)dActor_c::FindWithActorID(0xe, 0);
            while (other != 0) {
                other->mCannonOpen = 1;
                other = (daObjCannonShutter_c *)dActor_c::FindWithActorID(0xe, other);
            }
        }
        UpdateModelPosAndRotY();
    }
    if (_ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0)) {
        UpdateClsnPosAndRot();
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjCannonShutter_c6RenderEv
int daObjCannonShutter_c::Render()
{
    if (data_0209f2f8 == 6 && data_0209f220 == 1 && (int)(param1 & 0xff) == 1)
        return 1;
    if (mCannonOpen != 0)
        return 1;
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjCannonShutter_c16CleanupResourcesEv
int daObjCannonShutter_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov002_0210e12c.Release();
    data_ov002_0210e124.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bc990
/* Inbound "start opening" trigger from ov084. Plays sound 0x47 and sets
   mOpening. The ROM symbol is not a member; no coined name. Parameter stays
   void * because include/decl_common.h declares it that way. */
extern "C" {
void func_ov002_020bc990(void *actor)
{
    func_0201277c(0x47);
    ((daObjCannonShutter_c *)actor)->mOpening = 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjCannonShutter_cD1Ev
// @symbol _ZN20daObjCannonShutter_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjCannonShutter_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no D2. */
