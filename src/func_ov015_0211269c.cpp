//cpp
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef long long s64;

extern "C" {
void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* f, int a, int b);
void _ZN11ShadowModel10InitCuboidEv(void* self);
void _ZN8Platform21UpdateModelPosAndRotYEv(void* self);
void _ZN8Platform19UpdateClsnPosAndRotEv(void* self);
void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* fp);
void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* self, void* f, void* m, int fx, short s, void* b);
void func_020393d4(int* p, int v);
void func_020393c4(int* p, int v);
void _ZN13RaycastGroundC1Ev(void* self);
void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void* self, void* pos, void* actor);
int _ZN13RaycastGround10DetectClsnEv(void* self);
void _ZN13RaycastGroundD1Ev(void* self);
void func_ov015_021123c8(char* c);
int IsStarCollectedInCurLevel(int a);
}

extern void* data_ov015_02114a64;
extern void* data_ov015_02114a5c;
extern void* data_ov015_02113594;
extern void _ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_();
extern void func_ov015_021128f8();
extern s16 SINE_TABLE[];
extern signed char LEVEL_ID;
extern u8 STAR_ID;

struct V3 { int x, y, z; };
struct RG { char b[0x4c]; };

extern "C" int func_ov015_0211269c(char* self) {
  RG rc;
  V3 v;
  int r2;

  _ZN9ModelBase7SetFileEP8BMD_Fileii(self+0xd4, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov015_02114a64), 1, -1);
  _ZN11ShadowModel10InitCuboidEv(self+0x320);
  _ZN8Platform21UpdateModelPosAndRotYEv(self);
  _ZN8Platform19UpdateClsnPosAndRotEv(self);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
      self+0x124, _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov015_02114a5c),
      self+0x2ec, 0x199, *(short*)(self+0x8e), &data_ov015_02113594);
  func_020393d4((int*)(self+0x124), (int)&_ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  func_020393c4((int*)(self+0x124), (int)&func_ov015_021128f8);

  v.x = *(int*)(self+0x5c);
  v.y = *(int*)(self+0x60);
  v.z = *(int*)(self+0x64);
  v.y -= 0x14000;

  _ZN13RaycastGroundC1Ev(&rc);
  _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rc, &v, 0);
  *(int*)(self+0x378) = v.y;
  if (_ZN13RaycastGround10DetectClsnEv(&rc) != 0) {
    *(int*)(self+0x378) = *(int*)((char*)&rc + 0x44);
  }

  {
    int s0 = SINE_TABLE[(*(unsigned short*)(self+0x8e) >> 4) * 2];
    *(int*)(self+0x384) = (int)(((s64)s0 * 0xc8000 + 0x800) >> 12);
    int s1 = SINE_TABLE[(*(unsigned short*)(self+0x8e) >> 4) * 2 + 1];
    *(int*)(self+0x388) = (int)(((s64)s1 * 0xc8000 + 0x800) >> 12);
  }

  func_ov015_021123c8(self);

  *(short*)(self+0x94) = *(short*)(self+0x8e) + 0x8000;
  *(u8*)(self+0x390) = 0x87;

  r2 = *(int*)(self+8) & 0xff;
  if (r2 != 2) {
    *(int*)(self+0x98) = 0x3000;
  } else {
    *(int*)(self+0x37c) = *(int*)(self+0x60);
    *(int*)(self+0x380) = *(int*)(self+0x37c) + 0x1f4000;
    *(int*)(self+0xa0) = -0x3c000;
    *(int*)(self+0xa8) = 0xa000;
  }

  if (LEVEL_ID == 7 && (STAR_ID == 1 || IsStarCollectedInCurLevel(1) == 0) && *(int*)(self+0x60) >= 0xdac000) {
    _ZN13RaycastGroundD1Ev(&rc);
    return 0;
  }

  _ZN13RaycastGroundD1Ev(&rc);
  return 1;
}
