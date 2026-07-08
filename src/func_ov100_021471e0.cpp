//cpp
struct Vector3 { int x, y, z; };
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
extern int SNUFIT_BULLET_MODEL_PTR;
extern int data_ov100_02148a54;
extern int data_ov100_02148a5c;
extern int data_ov002_0210d7d4;
extern void _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_();
extern unsigned char CURRENT_GAMEMODE;

extern "C" int func_ov100_021471e0(char* c) {
  Vector3 pos;
  _ZN5Model8LoadFileER13SharedFilePtr(&SNUFIT_BULLET_MODEL_PTR);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov100_02148a54), 1, -1);
  _ZN11ShadowModel10InitCuboidEv(c+0x450);
  func_ov100_0214700c(c);
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
      c+0x124, _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov100_02148a5c),
      c+0x2ec, 0x1000, *(short*)(c+0x8e), &data_ov002_0210d7d4);
  func_020393d4((int*)(c+0x124), (int)&_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  *(int*)(c+0x440) = 0xa000;
  *(int*)(c+0x98) = *(int*)(c+0x440);
  func_020efaf0(c);
  *(int*)(c+0x43c) = 1;
  pos.x = *(int*)(c+0x5c);
  pos.y = *(int*)(c+0x60);
  pos.z = *(int*)(c+0x64);
  pos.y -= 0x14000;
  {
    RaycastGround rg;
    int b;
    rg.SetObjAndPos(pos, 0);
    *(int*)(c+0x4ac) = pos.y;
    if (rg.DetectClsn() != 0)
      *(int*)(c+0x4ac) = rg.result;
    *(unsigned char*)(c+0x42c) = 1;
    b = (CURRENT_GAMEMODE == 1);
    if (b)
      *(unsigned char*)(c+0x4b0) = 0xb4;
  }
  return 1;
}
