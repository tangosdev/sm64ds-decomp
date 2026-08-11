//cpp
// @symbol _ZN7HeaveHo13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "HeaveHo.h"
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void*);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void*, void*, int, int, unsigned int, unsigned int);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void*, void*, void*, int, int, unsigned int, unsigned int);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void*, void*, int, int, void*, int);
extern void func_ov077_02126d5c(void*, void*);
struct V3 { int x, y, z; };
extern struct V3 data_ov077_02127a5c;
}

int HeaveHo::InitResources()
{
  struct V3 v;
  void* f;
  f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov077_02127c88);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x340, f, 1, -1);
  _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov077_02127ca0);
  _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov077_02127c90);
  _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov077_02127c98);
  mVertAccel = -0x1000;
  mTerminalVelocity = -0x1e000;
  _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char*)this) + 0x110, ((char*)this), 0x52000, 0x52000, 0x800004, 0);
  v.x = data_ov077_02127a5c.x;
  v.y = data_ov077_02127a5c.y;
  v.z = data_ov077_02127a5c.z;
  _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(((char*)this) + 0x144, ((char*)this), &v, 0x54000, 0x32000, 0x200004, 0);
  mAngleY = mPrevAngleY;
  _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char*)this) + 0x184, ((char*)this), 0x64000, 0x64000, 0, 0);
  unk_400 = 0;
  unk_404 = mPosX;
  unk_408 = mPosY;
  unk_40c = mPosZ;
  mModelAnim.speed = 0x1000;
  unk_410 = mPosX;
  unk_414 = mPosY;
  unk_418 = mPosZ;
  func_ov077_02126d5c(((char*)this), &data_ov077_02127ce8);
  return 1;
}
