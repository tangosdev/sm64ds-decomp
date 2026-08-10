//cpp
// @symbol func_ov018_021116b4
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
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* c);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* th, void* kf, void* mtx, int fx, short s, void* clps);
extern void func_020393d4(int* p, int v);
extern void func_020393c4(int* p, int v);
extern void _ZN7PathPtrC1Ev(PathPtr* p);
extern void _ZN7PathPtr6FromIDEj(PathPtr* p, unsigned int id);
extern int _ZNK7PathPtr8NumNodesEv(PathPtr* p);
extern int data_ov018_02113bc8[];
extern int data_ov018_02113bc0[];
extern int data_ov018_02112f48[];
extern int _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_[];
extern int func_ov018_02111804[];

int func_ov018_021116b4(char* c){
    struct daObjSm_Lift_c *self = (struct daObjSm_Lift_c *)(void *)c;
  self->unk_334 = *(int*)(c+8) & 0xff;
  if(self->unk_334 == 0xff) return 0;
  void* f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov018_02113bc8);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, f, 1, -1);
  func_ov018_02111278(c);
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  void* kf = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov018_02113bc0);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c+0x124, kf, c+0x2ec, 0x1000, self->unk_08e, data_ov018_02112f48);
  func_020393d4((int*)(c+0x124), (int)_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  func_020393c4((int*)(c+0x124), (int)func_ov018_02111804);
  self->unk_331 = 0x3c;
  PathPtr p;
  self->unk_324 = self->unk_05c;
  self->unk_328 = self->unk_060;
  self->unk_32c = self->unk_064;
  {
    short* ang = (short*)(c + 0x8e);
    *ang = *ang - 0x4000;
  }
  _ZN7PathPtrC1Ev(&p);
  _ZN7PathPtr6FromIDEj(&p, self->unk_334);
  self->unk_338 = _ZNK7PathPtr8NumNodesEv(&p);
  self->unk_34c = 1;
  {
    int* ip = (int*)(c + 0x33c);
    *ip = *ip + self->unk_34c;
  }
  self->unk_320 = 0;
  return 1;
}
}
