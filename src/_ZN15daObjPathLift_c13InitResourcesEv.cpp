//cpp
// @symbol _ZN15daObjPathLift_c13InitResourcesEv
/* recovered: shared common types, renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types, renamed to Class_Method, RTTI class fields named */
#include "daObjPathLift_c.h"
// recovered name: daObjPathLift_c_InitResources
/* recovered: shared common types, renamed to Class_Method */
/* daObjPathLift_c::InitResources - name recovered from the vtable slot it fills.
   The body is a decompilation verified against the ROM, not an
   inferred stub. */
struct dActor_c;
struct dBgCh_Gnd {
  char pad[0x44];
  int result;
  char pad2[0xc];
  dBgCh_Gnd();
  ~dBgCh_Gnd();
  void SetObjAndPos(const Vector3& pos, dActor_c* a);
  int DetectClsn();
};
extern "C" {
void* _ZN5Model8LoadFileER13SharedFilePtr(void* p);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* f, int a, int b);
void _ZN11ShadowModel10InitCuboidEv(void* self);
int func_ov100_0214700c(char* c);
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(char* self);
void* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void* p);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* self, void* kcl, const void* mtx, int fix, short s, void* clps);
void func_020393d4(int* p, int v);
void func_ov002_020efaf0(char* c);
}
extern int data_ov002_0210d9f0;
extern "C" {
extern void _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
}
extern unsigned char data_0209f2d8;

extern "C" int _ZN15daObjPathLift_c13InitResourcesEv(char* c) {
    struct daObjPathLift_c *self = (struct daObjPathLift_c *)(void *)c;
  Vector3 pos;
  _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9f0);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov100_02148a54), 1, -1);
  _ZN11ShadowModel10InitCuboidEv(c+0x450);
  func_ov100_0214700c(c);
  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
      c+0x124, _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(&data_ov100_02148a5c),
      c+0x2ec, 0x1000, self->mAngleY, &data_ov002_0210d7d4);
  func_020393d4((int*)(c+0x124), (int)&_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
  /* unk_440/unk_43c/unk_42c are PathLift's own generic tail padding, not this
     class's fields -- see the header comment -- reached by raw offset. */
  *(s32*)(c + 0x440) = 0xa000;
  self->mHorzSpeed = *(s32*)(c + 0x440);
  func_ov002_020efaf0(c);
  *(s32*)(c + 0x43c) = 1;
  pos.x = self->mPosX;
  pos.y = self->mPosY;
  pos.z = self->mPosZ;
  pos.y -= 0x14000;
  {
    dBgCh_Gnd rg;
    int b;
    rg.SetObjAndPos(pos, 0);
    self->unk_4ac = pos.y;
    if (rg.DetectClsn() != 0)
      self->unk_4ac = rg.result;
    *(u8*)(c + 0x42c) = 1;
    b = (data_0209f2d8 == 1);
    if (b)
      self->unk_4b0 = 0xb4;
  }
  return 1;
}
