// @symbol func_ov043_02111320
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjKm1_Ukishima_c.h"
// @emits daObjKm1_Ukishima_c_InitResources
/* recovered: renamed to Class_Method */
/* daObjKm1_Ukishima_c::InitResources - recovered from vtable slot identity */
extern int _ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *p, int file, int a, int b);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *p);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *p);
extern int _ZN12MeshCollider8LoadFileER13SharedFilePtr(void *f);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *p, int kcl, void *mtx, int fix, short s, void *clps);
extern void func_020393d4(void *p, int v);
extern int _ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_[];
int daObjKm1_Ukishima_c_InitResources(char *c){
    struct daObjKm1_Ukishima_c *self = (struct daObjKm1_Ukishima_c *)(void *)c;
  int f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov047_021125e8);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, f, 1, -1);
  _ZN8Platform21UpdateModelPosAndRotYEv(c);
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  f = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov047_021125e0);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    c+0x124, f, c+0x2ec, 0x199, self->unk_08e, data_ov043_02111c00);
  func_020393d4(c+0x124, (int)_ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  self->unk_094 = self->unk_08e;
  self->unk_098 = 0xa000;
  self->unk_31e = 0x3c;
  return 1;
}
