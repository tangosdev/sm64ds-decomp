//cpp
extern "C" {
struct Matrix4x3 { int m[12]; };
int _ZN5Model8LoadFileER13SharedFilePtr(void* f);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* m, int bmd, int a, int b);
void _ZN11ShadowModel10InitCuboidEv(void* s);
void func_ov026_02111308(void* d);
void _ZN8Platform19UpdateClsnPosAndRotEv(void* c);
int _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* f);
void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* mc, int kcl, struct Matrix4x3* mtx, int scale, short s, void* clps);
void func_020393d4(int* p, int v);
void _ZN16MeshColliderBase6EnableEP5Actor(void* mc, void* a);
extern void* data_ov023_02112088;
extern void* _ZN32FloatOnWaterPlatformWdwRectangleD1Ev;
extern void* data_ov064_0211ba4c;
extern void _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(void);

int _ZN8Squasher13InitResourcesEv(char* c) {
  int bmd = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov023_02112088);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, bmd, 1, -1);
  _ZN11ShadowModel10InitCuboidEv(c+0x324);
  func_ov026_02111308(c);
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  int kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&_ZN32FloatOnWaterPlatformWdwRectangleD1Ev);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c+0x124, kcl, (struct Matrix4x3*)(c+0x2ec), 0x1000, *(short*)(c+0x8e), &data_ov064_0211ba4c);
  func_020393d4((int*)(c+0x124), (int)&_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  _ZN16MeshColliderBase6EnableEP5Actor(c+0x124, c);
  *(short*)(c+0x31e) = 0;
  *(short*)(c+0x320) = 0;
  *(char*)(c+0x322) = 0;
  return 1;
}
}
