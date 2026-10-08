//cpp
/**
 * Snowman's Land sliding ice block, shot and spawner variants.
 *
 * Actor ID 0x5d is the shot: it slides, sinks toward mMinPosY and
 * destroys itself. Any other ID is the spawner: it periodically
 * spawns shot blocks above itself. InitResources loads the shared
 * model and collision files for the shot, or arms the spawn timer
 * for the spawner.
 *
 * deslop
 * Leftover: V3 is a file-local POD triple, not Vector3: Vector3
 *   declares an (empty) destructor, and the stack copy here would
 *   emit a Vector3D1 the ROM does not have (S3).
 * Leftover: dActor_c::Spawn keeps its mangled spelling: it takes
 *   const Vector3 &, which V3 is not.
 * Leftover: dBgW_KcMbg::SetFile keeps its mangled spelling (by-value
 *   Fix12<int> parameters, wall 6az).
 * Leftover: +0x74 (Vector3) and +0x8c (rotation) have no named header
 *   fields; the data homes and the +4 loaded-file words stay
 *   address-named with casts at the use site.
 * Leftover: func_020393d4's second argument is the address of
 *   dBgW::UpdatePosWithVelocity, stored as a callback -- kept verbatim.
 */

#include "daObjSlIceBlock_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dBgActor_c.h"

/* File-local POD triple (see above). */
struct V3 { int x,y,z; };

extern "C" {
extern int DecIfAbove0_Short(void*);
extern int DecIfAbove0_Byte(void*);
extern int _Z14ApproachLinearRiii(int*, int, int);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int, unsigned int, unsigned int, void*, unsigned int);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
void *, int, void *, int, int, void *);
void func_020393d4(int *p, int v);
extern char data_ov027_02113108[];
}

/* Model handles construct through func_02017acc and destroy through
 * func_02017ab4; collision handles construct through func_02017b4c and destroy
 * through SharedFilePtr_Destruct_Clsn. The manifest aliases those undefined
 * member spellings onto the real ROM functions; each handle is the base plus
 * the two words that family actually stores. */
struct SlIceBlockModelFilePtr : SharedFilePtr {
    u32 words[2];

    SlIceBlockModelFilePtr(u32 fileID);
    ~SlIceBlockModelFilePtr();
};

struct SlIceBlockClsnFileHandle : SharedFilePtr {
    u32 words[2];

    SlIceBlockClsnFileHandle(u32 fileID);
    ~SlIceBlockClsnFileHandle();
};

extern SlIceBlockModelFilePtr data_ov027_02113be8;
extern SlIceBlockClsnFileHandle data_ov027_02113be0;

/* Emission order is ROM order: the destructor pair must stay first.
 * Do not reorder. */
#pragma defer_codegen off

// @symbol _ZN17daObjSlIceBlock_cD1Ev
// @symbol _ZN17daObjSlIceBlock_cD0Ev
/* One out-of-line definition; mwccarm emits D1 and D0 from it, in that
 * order: its own vptr, then dBgActor_c's inlined, then dBgActor_c's
 * Model and dBgW_KcMbg, then dActor_c. */
daObjSlIceBlock_c::~daObjSlIceBlock_c()
{
}

// @symbol _ZN17daObjSlIceBlock_c15OnHitByMegaCharER6Player
/* Same idiom as daObjBk_Dossunbar_c/daObjBk_Lift_c/daObjIceBoard_c: the
 * trailing unqualified Kill() dispatches virtually to the inherited
 * base implementation (this class does not override slot 31). */
void daObjSlIceBlock_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    Kill();
}

// @symbol _ZN17daObjSlIceBlock_c16CleanupResourcesEv
int daObjSlIceBlock_c::CleanupResources()
{
  unsigned char ok = (actorID==0x5d);
  if(ok){ mMeshCollider.Disable(); }
  data_ov027_02113be8.Release();
  data_ov027_02113be0.Release();
  return 1;
}

// @symbol _ZN17daObjSlIceBlock_c6RenderEv
int daObjSlIceBlock_c::Render()
{
  int x = actorID==0x5d;
  if(x){
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mModel.Render(0);
  }
  return 1;
}

// @symbol _ZN17daObjSlIceBlock_c8BehaviorEv
int daObjSlIceBlock_c::Behavior()
{
  int isType = (actorID == 0x5d);
  if(isType){
    if(DecIfAbove0_Short(&mDelayTimer) == 0){
      _Z14ApproachLinearRiii(&mHorzSpeed, 0, 0x3000);
      if(_Z14ApproachLinearRiii(&mPosY, mMinPosY, 0xa000) != 0){
        MarkForDestruction();
      }
    }
    UpdatePos(0);
    mSoundID = _ZN5Sound8PlayLongEjjjRK7Vector3s(mSoundID, 3, 0x98, ((char *)this)+0x74, 0);
  } else {
    if(DecIfAbove0_Short(&mDelayTimer) == 0){
      V3 pos;
      pos.x = mPosX;
      pos.y = mPosY;
      pos.z = mPosZ;
      int spawnType = 1;
      if(DecIfAbove0_Byte(&mNumToBigIce) == 0){
        mNumToBigIce = 5;
        spawnType = 2;
      } else {
        pos.y -= 0x50000;
      }
      unsigned char cnt = mNumToBigIce;
      mDelayTimer = (cnt + 1) * 0x14;
      dActor_c::Spawn(0x5d, spawnType, *(Vector3 *)&pos, (Vector3_16 *)(((char *)this)+0x8c), mAreaId, -1);
    }
  }
  return 1;
}

// @symbol _ZN17daObjSlIceBlock_c13InitResourcesEv
int daObjSlIceBlock_c::InitResources()
{
    Model::LoadFile(data_ov027_02113be8);
    dBgW_Kc::LoadFile(data_ov027_02113be0);

    int on = (actorID == 0x5d);
    if (on) {
        if (mModel.SetFile((BMD_File *)data_ov027_02113be8.words[1], 1, -1) == 0)
            return 0;
        UpdateModelPosAndRotY();
        UpdateClsnPosAndRot();
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, *(int *)&data_ov027_02113be0.words[1], &mClsnMat,
            0x1000, mAngleY, data_ov027_02113108);
        func_020393d4((int *)&mMeshCollider,
            (int)&dBgW::UpdatePosWithVelocity);
        mMeshCollider.unk_4c = 0;
        mMeshCollider.Enable(this);
        mHorzSpeed = 0x2d000;
        mDelayTimer = 0x64;
        mPrevAngleY = -0x4000;
        mMinPosY = mPosY - 0xc8000;
    } else {
        mDelayTimer = (u8)mNumToBigIce * 0x14;
        mNumToBigIce = 5;
    }
    return 1;
}

/* Reconstructed source-style name: SM64DS proves daObjSlIceBlock_c through
 * RTTI, allocation size, vtable identity, and the SL_ICEBLOCK_SHOT registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: SlidingIce_Spawn. */
// @symbol daObjSlIceBlock_c_classInit_SL_ICEBLOCK_SHOT
extern "C" daObjSlIceBlock_c *daObjSlIceBlock_c_classInit_SL_ICEBLOCK_SHOT()
{
    return new daObjSlIceBlock_c();
}

/* Reconstructed source-style name: SM64DS proves daObjSlIceBlock_c through
 * RTTI, allocation size, vtable identity, and the SL_ICEBLOCK registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: SlidingIceSpawner_Spawn. */
// @symbol daObjSlIceBlock_c_classInit_SL_ICEBLOCK
extern "C" daObjSlIceBlock_c *daObjSlIceBlock_c_classInit_SL_ICEBLOCK()
{
    return new daObjSlIceBlock_c();
}

/* The ice block's shared file handles: the model, then the collision map.
 * Their constructors and the destructor registrations make mwcc emit this
 * TU's __sinit. */
SlIceBlockModelFilePtr data_ov027_02113be8(1713);
SlIceBlockClsnFileHandle data_ov027_02113be0(1714);
