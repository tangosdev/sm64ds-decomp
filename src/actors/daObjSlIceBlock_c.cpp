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
extern char data_ov027_02113be8[];
extern char data_ov027_02113be0[];
extern int DecIfAbove0_Short(void*);
extern int DecIfAbove0_Byte(void*);
extern int _Z14ApproachLinearRiii(int*, int, int);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int, unsigned int, unsigned int, void*, unsigned int);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
void *, int, void *, int, int, void *);
void func_020393d4(int *p, int v);
/* data_ov027_02113be8 is already declared above as char[]; keeping that FIRST
 * declaration and casting where a word is read. */
extern char data_ov027_02113108[];
}

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
  ((SharedFilePtr *)(data_ov027_02113be8))->Release();
  ((SharedFilePtr *)(data_ov027_02113be0))->Release();
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
    Model::LoadFile(*(SharedFilePtr *)data_ov027_02113be8);
    dBgW_Kc::LoadFile(*(SharedFilePtr *)data_ov027_02113be0);

    int on = (actorID == 0x5d);
    if (on) {
        if (mModel.SetFile(((BMD_File **)data_ov027_02113be8)[1], 1, -1) == 0)
            return 0;
        UpdateModelPosAndRotY();
        UpdateClsnPosAndRot();
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, *(int *)(data_ov027_02113be0 + 4), &mClsnMat,
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
