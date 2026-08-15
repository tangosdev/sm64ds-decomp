//cpp
// @symbol func_ov100_021471e0
/* recovered: shared common types, renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types, renamed to Class_Method, RTTI class fields named */
#include "daObjPathLift_c.h"
// recovered name: daObjPathLift_c_InitResources
/* recovered: shared common types, renamed to Class_Method */
/* daObjPathLift_c::InitResources - name recovered from the vtable slot it fills.
   The body is a decompilation verified against the ROM, not an
   inferred stub. */
struct Actor;
struct RaycastGround {
  char pad[0x44];
  int result;
  char pad2[0xc];
  RaycastGround();
  ~RaycastGround();
  void SetObjAndPos(const Vector3& pos, Actor* a);
  int DetectClsn();
};
extern "C" {
void* _ZN5Model8LoadFileER13SharedFilePtr(void* p);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* f, int a, int b);
void _ZN11ShadowModel10InitCuboidEv(void* self);
int func_ov100_0214700c(char* c);
void _ZN8Platform19UpdateClsnPosAndRotEv(char* self);
void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* p);
void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* self, void* kcl, const void* mtx, int fix, short s, void* clps);
void func_020393d4(int* p, int v);
void func_020efaf0(char* c);
}
extern int data_ov002_0210d9f0;
extern void _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_();
extern unsigned char data_0209f2d8;

extern "C" int func_ov100_021471e0(char* c) {
    struct daObjPathLift_c *self = (struct daObjPathLift_c *)(void *)c;
  Vector3 pos;
  _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9f0);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov100_02148a54), 1, -1);
  _ZN11ShadowModel10InitCuboidEv(c+0x450);
  func_ov100_0214700c(c);
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
      c+0x124, _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov100_02148a5c),
      c+0x2ec, 0x1000, self->unk_08e, &data_ov002_0210d7d4);
  func_020393d4((int*)(c+0x124), (int)&_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  self->unk_440 = 0xa000;
  self->unk_098 = self->unk_440;
  func_020efaf0(c);
  self->unk_43c = 1;
  pos.x = self->unk_05c;
  pos.y = self->unk_060;
  pos.z = self->unk_064;
  pos.y -= 0x14000;
  {
    RaycastGround rg;
    int b;
    rg.SetObjAndPos(pos, 0);
    self->unk_4ac = pos.y;
    if (rg.DetectClsn() != 0)
      self->unk_4ac = rg.result;
    self->unk_42c = 1;
    b = (data_0209f2d8 == 1);
    if (b)
      self->unk_4b0 = 0xb4;
  }
  return 1;
}
