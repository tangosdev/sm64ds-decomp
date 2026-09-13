//cpp
/* Bob-omb Battlefield Chain Chomp fence -- the barred gate the chomp is
 * tethered in front of. ov014/daObjWanwanShutter_c, profile WANWAN_SHUTTER
 * (CHAIN_CHOMP_FENCE 41). This is the fence, not the chomp (daWanwan_c).
 *
 * A GENUINE TRANSLATION UNIT, ENROLLED AND CANONICAL. It is the whole of the
 * cartridge's contiguous linker run .text 0x02112e0c..0x021130ac, eight
 * functions and nothing else, and the production build links this object rather
 * than eight per-function ones. The filename is tools/tu_names.py's:
 * candidate_stem('daObjWanwanShutter_c') is d_a_obj_wanwan_shutter.
 *
 * THE CLASS IS NAMED FROM THE CARTRIDGE, not coined. ov014 0x02114868 is a
 * __si_class_type_info whose _ZTS at 0x02114874 reads exactly
 * `20daObjWanwanShutter_c`, and whose one base, at subobject offset 0, is
 * ov002 0x021089ec -- _ZTI10dBgActor_c. That is why the header says
 * `struct daObjWanwanShutter_c : dBgActor_c`. The historical spelling was
 * ChainChompFence.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder.
 *
 * THE SIZE ARITHMETIC CLOSES. The factory's literal is 800 = 0x320, which is
 * exactly sizeof(dBgActor_c); this class adds only mDisabled at 0x31e, inside
 * the tail padding the base already had, so the derived size is the base's.
 *
 * THE VTABLE IS 32 SLOTS and diffs against _ZTV10dBgActor_c (ov002 0x0210ae38)
 * at exactly six: 0 InitResources, 3 CleanupResources, 6 Behavior, 9 Render,
 * 16 D1, 17 D0. Slot 31 is dBgActor_c::Kill at 0x020ee55c in both tables --
 * inherited, not overridden.
 *
 */

/* daObjWanwanShutter_c.h FIRST: it pulls in dBgActor_c.h, which must reach
   common.h ahead of Model.h or the wrong Matrix4x3 spelling wins. */
#include "daObjWanwanShutter_c.h"
#include "Sound.h"
#include "SharedFilePtr.h"

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

/* This overlay's file handles and collision-parameter block. This TU claims
   .text only and does not define them. */
extern SharedFilePtr data_ov014_021149c0;   /* the BMD model */
extern SharedFilePtr data_ov014_021149b8;   /* the KCL collision mesh */
extern CLPS_Block    data_ov014_02114558;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjWanwanShutter_c6RenderEv
int daObjWanwanShutter_c::Render()
{
    if (mDisabled != 0)
        return 1;
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjWanwanShutter_c16CleanupResourcesEv
int daObjWanwanShutter_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov014_021149c0.Release();
    data_ov014_021149b8.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
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
    Sound::PlayBank3(0xf, *(Vector3 *)&a->mCamSpacePosX);
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

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
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
