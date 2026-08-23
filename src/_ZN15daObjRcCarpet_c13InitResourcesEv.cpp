//cpp
// @symbol _ZN15daObjRcCarpet_c13InitResourcesEv
#include "daObjRcCarpet_c.h"
#include "SharedFilePtr.h"

extern "C" {
/* SetAnim and dBgW_KcMbg::SetFile carry by-value Fix12<int> parameters.
   Their faithful member declarations trigger mwccarm's stack-homing ABI and
   do not reproduce these call sites, so keep the verified scalar shims. */
int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,int,int,int,unsigned);
int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,int,int,void*);
void func_020393d4(void* p, void* v);
void func_ov002_020efaf0(void* c);
void func_ov036_021122c0(daObjRcCarpet_c *self);
extern int data_ov002_0210d9f0[];
extern void *data_ov036_02113f58[];
extern int data_ov036_0211419c[];
extern void _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
}

int daObjRcCarpet_c::InitResources() {
  Model::LoadFile(*(SharedFilePtr *)data_ov002_0210d9f0);
  void *modelFile = Model::LoadFile(*(SharedFilePtr *)data_ov036_02113f58[0]);
  mModelAnim.SetFile((BMD_File *)modelFile, 1, -1);
  Animation::LoadFile(*(SharedFilePtr *)data_ov036_0211419c);
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov036_0211419c[1], 0, 0x1000, 0);
  func_ov036_021122c0(this);
  UpdateClsnPosAndRot();
  void *clsnFile = dBgW_Kc::LoadFile(*(SharedFilePtr *)data_ov036_02113f58[1]);
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
      &mMeshCollider, (int)clsnFile, &mClsnMat, 0x199, mAngleY,
      data_ov036_02113f58[2]);
  func_020393d4(&mMeshCollider, (void*)_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
  *(int*)((char*)this + 0x440) = 0xa000;
  mHorzSpeed = *(int*)((char*)this + 0x440);
  func_ov002_020efaf0(this);
  *(int*)((char*)this + 0x43c) = 1;
  *(unsigned char*)((char*)this + 0x42c) = 1;
  return 1;
}
