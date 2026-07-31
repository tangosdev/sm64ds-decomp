// @symbol func_ov045_021113ec
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjKm2_Agaru_c.h"
// @emits daObjKm2_Agaru_c_InitResources
/* recovered: renamed to Class_Method */
/* daObjKm2_Agaru_c::InitResources - recovered from vtable slot identity */
extern int _ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *p, int file, int a, int b);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *p);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *p);
extern int _ZN12MeshCollider8LoadFileER13SharedFilePtr(void *f);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *p, int kcl, void *mtx, int fix, short s, void *clps);
extern void func_020393d4(void *p, int v);
extern void func_020393c4(void *p, int v);
extern int _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_[];
int daObjKm2_Agaru_c_InitResources(char *c){
    struct daObjKm2_Agaru_c *self = (struct daObjKm2_Agaru_c *)(void *)c;
  int f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov045_02113188);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, f, 1, -1);
  _ZN8Platform21UpdateModelPosAndRotYEv(c);
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  f = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov045_02113180);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    c+0x124, f, c+0x2ec, 0x199, self->unk_08e, data_ov045_021125d0);
  func_020393d4(c+0x124, (int)_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  func_020393c4(c+0x124, (int)data_ov059_021114c8);
  *(short*)(c+0x300+0x24) = 0;
  self->unk_327 = 0;
  self->unk_320 = self->unk_060;
  return 1;
}
