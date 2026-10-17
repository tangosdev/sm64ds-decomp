//cpp
/* leftovers:
 * - dBgW_KcMbg::SetFile, dBgActor_c::IsClsnInRange and
 *   Particle::System::NewSimple stay mangled. Each takes Fix12<int>
 *   by value.
 * - The model and collision handles are local SharedFilePtr
 *   subclasses. The manifest aliases them to the ROM symbols.
 * - The destructor stays inline in the header.
 * - dBgActor_c.h is included first so common.h wins over Model.h.
 *
 * daObjWanwanShutter_c, the Bob-omb Battlefield Chain Chomp fence
 * (WANWAN_SHUTTER). This is the fence, not the chomp.
 */
/* daObjWanwanShutter_c.h FIRST: it pulls in dBgActor_c.h, which must reach
   common.h ahead of Model.h or the wrong Matrix4x3 spelling wins. */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_wanwan_shutter_AssetId {
    ASSET_B_WAN_SHUTTER_BMD = 0x05a7,
    ASSET_B_WAN_SHUTTER_KCL = 0x05a8
};

#include "daObjWanwanShutter_c.h"
#include "Sound.h"
#include "SharedFilePtr.h"

/* 8-byte file handles. The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct WanwanShutterModelFilePtr : SharedFilePtr {
    unsigned int words[2];

    WanwanShutterModelFilePtr(unsigned int fileID);
    ~WanwanShutterModelFilePtr();
};

struct WanwanShutterClsnFilePtr : SharedFilePtr {
    unsigned int words[2];

    WanwanShutterClsnFilePtr(unsigned int fileID);
    ~WanwanShutterClsnFilePtr();
};

typedef char WanwanShutterModelFilePtr_size_must_be_8[
    sizeof(WanwanShutterModelFilePtr) == 8 ? 1 : -1];
typedef char WanwanShutterClsnFilePtr_size_must_be_8[
    sizeof(WanwanShutterClsnFilePtr) == 8 ? 1 : -1];

extern "C" {
/* dBgActor_c::IsClsnInRange(Fix12<int>, Fix12<int>) -- reached through the
   mangled name because the two by-value Fix12<int> parameters are wall 6az
   on ov014/daObjWanwanShutter_c::Behavior. */
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);

/* dBgW_KcMbg::SetFile -- same wall on ov014/daObjWanwanShutter_c::InitResources:
   its by-value Fix12<int> scale makes a member call cost stack the ROM does
   not spend. */
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, Matrix4x3 *mat, int scale, s16 angY,
    void *clps);

/* Particle::System::NewSimple -- same wall on func_ov014_02112ea8: three
   by-value Fix12<int> coordinates. */
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    unsigned int t, int x, int y, int z);

/* This overlay's file handles and collision-parameter block. The two handles
   are constructed by this TU's static initializer. */
extern WanwanShutterModelFilePtr data_ov014_021149c0;   /* the BMD model */
extern WanwanShutterClsnFilePtr data_ov014_021149b8;   /* the KCL collision mesh */
extern CLPS_Block    data_ov014_02114558;
}

// @symbol daObjWanwanShutter_c_classInit
/* Reconstructed source-style name. Historical alias: ChainChompFence_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * 0x02113080 loads 800 = 0x320 -- the class's own size -- into the header's
 * inline operator new; 0x02113090 calls dBgActor_c's C2 and the store at
 * 0x0211309c lays down this class's vptr. The null check is the one `new`
 * itself emits. */
extern "C" daObjWanwanShutter_c *daObjWanwanShutter_c_classInit(void)
{
    return new daObjWanwanShutter_c();
}

// @symbol _ZN20daObjWanwanShutter_c13InitResourcesEv
/* Vtable slot 0. Loads the fence's model and collision mesh and plants both at
   the actor's position. Scale 0x1000 is 1.0. */
int daObjWanwanShutter_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov014_021149c0), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    {
        KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov014_021149b8);
        /* MEASURED on ov014/daObjWanwanShutter_c::InitResources: this one call
           has to keep the mangled spelling. Its third parameter is a by-value
           Fix12<int> -- wall 6az -- and materialising one costs stack traffic
           the ROM does not have. */
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY,
            &data_ov014_02114558);
    }
    return 1;
}

// @symbol _ZN20daObjWanwanShutter_c8BehaviorEv
int daObjWanwanShutter_c::Behavior()
{
    if (mDisabled != 0)
        return 1;
    /* MEASURED on ov014/daObjWanwanShutter_c::Behavior: the two by-value
       Fix12<int> parameters are wall 6az. */
    _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0);
    return 1;
}

// @symbol _ZN20daObjWanwanShutter_c6RenderEv
int daObjWanwanShutter_c::Render()
{
    if (mDisabled != 0)
        return 1;
    mModel.Render(0);
    return 1;
}

// @symbol _ZN20daObjWanwanShutter_c16CleanupResourcesEv
int daObjWanwanShutter_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov014_021149c0.Release();
    data_ov014_021149b8.Release();
    return 1;
}

// @symbol func_ov014_02112ea8
/* The chomp's "break the fence" trigger, reached from daWanwan_c when the
   chomp finishes its lunge at the gate. Plays bank-3 sound 0xf at camera-space
   position, spawns particle 0x1e 300.0 above the fence, poofs, sets mDisabled
   so Behavior/Render go idle, and drops the collider out of the world.
 *
 * Keeps its placeholder label: daWanwan_c calls it as a free function, and
 * coining a method name is out of scope for this TU.
 *
 * Particle::System::NewSimple stays mangled: its three by-value Fix12<int>
 * parameters are wall 6az on this helper. */
extern "C" void func_ov014_02112ea8(daObjWanwanShutter_c *a)
{
    Vector3 v[2];
    Sound::PlayBank3(0xf, a->mCamSpacePos);
    {
        int ty = a->mPosY;
        int tz = a->mPosZ;
        int tx = a->mPosX;
        v[0].x = tx;
        v[0].y = ty + 0x12c000;
        v[0].z = tz;
    }
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x1e, v[0].x, v[0].y, v[0].z);
    v[1].x = v[0].x;
    v[1].y = v[0].y;
    v[1].z = v[0].z;
    a->PoofDustAt(v[1]);
    a->mDisabled = 1;
    if (a->mMeshCollider.IsEnabled())
        a->mMeshCollider.Disable();
}

// @symbol _ZN20daObjWanwanShutter_cD1Ev
// @symbol _ZN20daObjWanwanShutter_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjWanwanShutter_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no D2. Writing the body
   out of line here instead flips them to D0-before-D1 and the isolation step
   rejects the object.

   Their bodies are two vptr stores and three destructor calls, every one a
   consequence of `daObjWanwanShutter_c : dBgActor_c`: this class's vptr, then
   dBgActor_c's -- inlined, because that destructor is defined in its class
   body -- then dBgActor_c's dBgW_KcMbg and Model, then dActor_c. This class
   adds no member with a destructor of its own. */

/* Source order is construction order: model file 1447, then the collision
 * file 1448. The compiler registers each destructor beside the object. */
WanwanShutterModelFilePtr data_ov014_021149c0(ASSET_B_WAN_SHUTTER_BMD);
WanwanShutterClsnFilePtr data_ov014_021149b8(ASSET_B_WAN_SHUTTER_KCL);
