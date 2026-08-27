//cpp
// @symbol _ZN7SkiLift13InitResourcesEv
#include "SkiLift.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjSm_Lift_c.h"
// recovered name: daObjSm_Lift_c_InitResources
/* recovered: renamed to Class_Method */
/* daObjSm_Lift_c::InitResources - recovered from vtable slot identity */
struct PathPtr { char b[8]; };
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* th, void* f, int a, int b);
extern void func_ov018_02111278(void* c);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void* c);
extern void* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* th, void* kf, void* mtx, int fx, short s, void* clps);
extern void func_020393d4(int* p, int v);
extern void func_020393c4(int* p, int v);
extern void _ZN7PathPtrC1Ev(PathPtr* p);
extern void _ZN7PathPtr6FromIDEj(PathPtr* p, unsigned int id);
extern int _ZNK7PathPtr8NumNodesEv(PathPtr* p);
extern int data_ov018_02113bc8[];
extern int data_ov018_02113bc0[];
extern int data_ov018_02112f48[];
extern int _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_[];
extern int func_ov018_02111804[];

s32 SkiLift::InitResources() {
    char* c = (char*)this;
    struct daObjSm_Lift_c *self = (struct daObjSm_Lift_c *)(void *)c;
  self->mPathId = *(int*)(c+8) & 0xff;
  if(self->mPathId == 0xff) return 0;
  void* f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov018_02113bc8);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, f, 1, -1);
  func_ov018_02111278(c);
  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
  void* kf = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov018_02113bc0);
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c+0x124, kf, c+0x2ec, 0x1000, self->mAngleY, data_ov018_02112f48);
  func_020393d4((int*)(c+0x124), (int)_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
  func_020393c4((int*)(c+0x124), (int)func_ov018_02111804);
  self->unk_331 = 0x3c;
  PathPtr p;
  self->mBasePosX = self->mPosX;
  self->mBasePosY = self->mPosY;
  self->mBasePosZ = self->mPosZ;
  {
    short* ang = (short*)(c + 0x8e);
    *ang = *ang - 0x4000;
  }
  _ZN7PathPtrC1Ev(&p);
  _ZN7PathPtr6FromIDEj(&p, self->mPathId);
  self->mNodeCount = _ZNK7PathPtr8NumNodesEv(&p);
  self->mNodeStep = 1;
  {
    int* ip = (int*)(c + 0x33c);
    *ip = *ip + self->mNodeStep;
  }
  self->unk_320 = 0;
  return 1;
}
}
