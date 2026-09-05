//cpp
/* Production translation unit for ov002/daObjCannonShutter_c, hand-curated.
 * 8 function(s), .text 0x020bc8f4..0x020bccfc.
 *
 * ENROLLED AND CANONICAL. config/arm9/overlays/ov002/delinks.txt licenses that
 * whole run to this one path, so the ROM build compiles this file in place of
 * the eight legacy one-function sources it replaces.
 *
 * The shutter that seals a cannon. It sits still until something opens it,
 * then slides -- down first, then forward once it has travelled far enough --
 * and when it has cleared the cannon it tells every other shutter in the level
 * that the cannon is open. After that it only follows the player: while the
 * player is Mega it snaps back to its spawn position and keeps its collider
 * live, and otherwise it switches the collider off entirely.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder:
 * tools/rombuild.py refuses the object outright when the emitted order and the
 * ROM's disagree.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x020bc8f4  src/_ZN20daObjCannonShutter_cD1Ev.cpp
 *   [1] 0x020bc938  src/_ZN20daObjCannonShutter_cD0Ev.cpp
 *   [2] 0x020bc990  src/func_ov002_020bc990.c
 *   [3] 0x020bc9b0  src/_ZN20daObjCannonShutter_c16CleanupResourcesEv.cpp
 *   [4] 0x020bc9f4  src/_ZN20daObjCannonShutter_c6RenderEv.cpp
 *   [5] 0x020bca78  src/_ZN20daObjCannonShutter_c8BehaviorEv.cpp
 *   [6] 0x020bcc20  src/_ZN20daObjCannonShutter_c13InitResourcesEv.cpp
 *   [7] 0x020bcccc  src/daObjCannonShutter_c_Spawn.c
 */

/* daObjCannonShutter_c.h FIRST: it pulls in dBgActor_c.h, which must reach
   common.h ahead of Model.h or the wrong Matrix4x3 spelling wins. */
#include "daObjCannonShutter_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "decl_common.h"
#include "decl_ActorBase.h"
#include "decl_Platform.h"

extern "C" {
/* Free math helpers, still under their image-wide names. Typed against what
   this file actually hands them. */
void Matrix4x3_FromRotationY(Matrix4x3 *m, s16 angleY);
void MulVec3Mat4x3(const Vector3 *v, const Matrix4x3 *m, Vector3 *dst);
void AddVec3(const Vector3 *a, const Vector3 *b, Vector3 *dst);
int  Vec3_Dist(const Vector3 *a, const Vector3 *b);
int  Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
/* The shared scratch matrix this actor builds its slide direction in. */
extern Matrix4x3 data_020a0e68;

/* dBgActor_c::IsClsnInRangeOnScreen(Fix12<int>, Fix12<int>) -- reached through
   the mangled name because the two by-value Fix12<int> parameters are exactly
   the shape include/dBgActor_c.h records as wall 6az. */
int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *self, int a, int b);

/* dBgW_KcMbg::SetFile -- same wall: include/dBgW_KcMbg.h declares the real
   signature, but its by-value Fix12<int> makes a member call cost stack the
   ROM does not spend. See the call site in InitResources. */
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 &mat, int scale, s16 angY,
    CLPS_Block &clps);

/* Render's two global gates: the current scene and level state words. */
extern signed char   data_0209f2f8;
extern unsigned char data_0209f220;

/* The shutter's own file handles and collision-parameter block in ov002. */
extern SharedFilePtr data_ov002_0210e12c;   /* the BMD model */
extern SharedFilePtr data_ov002_0210e124;   /* the KCL collision mesh */
extern CLPS_Block    data_ov002_0210d7f4;
}

/* Runtime actor/process profile descriptor at ov002:0x02109d14. The field
 * roles are established by the recovered fBase_c/dActor_c consumers; their
 * exact original SM64DS member spellings are not preserved. */
struct CannonShutterSpawnInfo {
    int *(*classInit)();
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
/* ROM ordinal 7 -- class initializer, 0x020bcccc, size 0x30                   */
/* -------------------------------------------------------------------------- */
// @symbol daObjCannonShutter_c_classInit
/* The actor factory the level tables call. It allocates through
   fBase_c::operator new, runs dBgActor_c's base-subobject constructor over the
   storage and then stamps this class's vtable address point over the vptr --
   the shape every hand-written Spawn in this family has, and the reason the
   allocation size IS the class's sizeof. Kept extern "C" and untyped past the
   vptr store because the ROM's own code is the base C2 variant, which C++ has
   no spelling for. */
extern "C" {
int *daObjCannonShutter_c_classInit(void)
{
    int *self = (int *)_ZN7fBase_cnwEj(sizeof(daObjCannonShutter_c));  /* 0x330 */
    if (self) {
        _ZN10dBgActor_cC2Ev(self);
        self[0] = (int)(_ZTV20daObjCannonShutter_c + 2);   /* address point */
    }
    return self;
}
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
/* ROM ordinal 6 -- _ZN20daObjCannonShutter_c13InitResourcesEv, 0x020bcc20, 0xac */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjCannonShutter_c13InitResourcesEv
/* Vtable slot 0. Loads the shutter's model and collision mesh, plants both at
   the actor's position, remembers that position as the one to snap back to,
   and -- if this level's cannon has already been opened -- comes up in the
   already-open state so the shutter never plays its slide. */
int daObjCannonShutter_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov002_0210e12c), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    {
        KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov002_0210e124);
        /* MEASURED: this one call has to keep the mangled spelling. Its third
           parameter is a by-value Fix12<int> -- wall 6az, the case
           include/dBgW_KcMbg.h already records -- and Fix12<int> is a bare
           aggregate with no constructor, so materialising one costs 12 bytes of
           stack traffic the ROM does not have. Written as a real member call
           this function came out 0xb8 against the cartridge's 0xac, and because
           ov002 is the overlay every later overlay loads behind, those 12 bytes
           moved 69 modules (106/106 -> 20/106). 0x199 is 409/4096, i.e. a
           collider scale of 0.1. */
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
/* ROM ordinal 5 -- _ZN20daObjCannonShutter_c8BehaviorEv, 0x020bca78, 0x1a8    */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjCannonShutter_c8BehaviorEv
/* The per-frame update, in three states.
 *
 * Already open: the shutter is invisible and only exists as collision. While
 * the closest player is Mega it is pinned to its spawn position with a live
 * collider (a Mega player can smash it); otherwise the collider is switched
 * off and the shutter stops interacting entirely.
 *
 * Opening: it slides along its own facing. Phase 0 drops it straight down
 * (-1.0 in Y); once it is more than 10.0 from home it flips to phase 1 and
 * slides forward instead (+2.0 in Z). Both offsets are rotated by mAngleY
 * through the shared scratch matrix, so the slide follows whichever way the
 * cannon points. Once its horizontal distance from home passes 200.0 the
 * cannon counts as open, and every actor with ID 0xe -- every cannon shutter
 * in the level -- is told so.
 *
 * Neither: it does nothing but keep its collider in step with itself. */
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
/* ROM ordinal 4 -- _ZN20daObjCannonShutter_c6RenderEv, 0x020bc9f4, size 0x84  */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjCannonShutter_c6RenderEv
/* Vtable slot 2. Two skips before the draw: one scene and level combination in
   which the low byte of the spawn parameter selects a shutter that is never
   drawn, and the open state, after which the shutter is gone. */
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
/* ROM ordinal 3 -- _ZN20daObjCannonShutter_c16CleanupResourcesEv, 0x020bc9b0, 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjCannonShutter_c16CleanupResourcesEv
/* Vtable slot 3, the teardown half of InitResources: drop the collider out of
   the collision world, then release this actor's share of the two files. */
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
/* ROM ordinal 2 -- func_ov002_020bc990, 0x020bc990, size 0x20                 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bc990
/* The "start opening" trigger: plays sound 0x47 and sets mOpening, after which
   Behavior takes over the slide. Nothing in this overlay calls it -- it is
   reached from elsewhere in the image through its address, which is why it is
   still a free extern "C" function under a placeholder name rather than a
   member. The parameter stays void * because include/decl_common.h -- which
   C translation units also read -- declares it that way; the cast at the store
   is what turns the raw +0x32c into a named field. */
extern "C" {
void func_ov002_020bc990(void *actor)
{
    func_0201277c(0x47);
    ((daObjCannonShutter_c *)actor)->mOpening = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN20daObjCannonShutter_cD0Ev, 0x020bc938, size 0x58       */
/* ROM ordinal 0 -- _ZN20daObjCannonShutter_cD1Ev, 0x020bc8f4, size 0x44       */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjCannonShutter_cD1Ev
// @symbol _ZN20daObjCannonShutter_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjCannonShutter_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no D2. Writing the body
   out of line here instead flips them to D0-before-D1 and the isolation step
   rejects the object.

   Their bodies are two vptr stores and three destructor calls, every one a
   consequence of `daObjCannonShutter_c : dBgActor_c`: this class's vptr, then
   dBgActor_c's -- inlined, because that destructor is defined in its class
   body -- then dBgActor_c's dBgW_KcMbg and Model, then dActor_c. This class
   adds no member with a destructor of its own. */
