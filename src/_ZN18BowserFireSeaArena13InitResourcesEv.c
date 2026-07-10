typedef int Fix12;
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern void func_ov060_02117a64(void*);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, Fix12, short, void*);
extern int func_020393d4(void*, void*);
extern int _ZN16MeshColliderBase6EnableEP5Actor(void*, void*);
extern int _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(void);

extern void* data_ov060_0211affc;
extern void* data_ov060_0211aff4;
extern void func_021115bc(void);

int _ZN18BowserFireSeaArena13InitResourcesEv(char* c) {
  void* mdl;
  void* kcl;
  mdl = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov060_0211affc);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0x324, mdl, 1, -1);
  func_ov060_02117a64(c);
  kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov060_0211aff4);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c+0x374, kcl, c+0x2ec, 0x1000, *(short*)(c+0x8e), &func_021115bc);
  func_020393d4(c+0x374, &_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  _ZN16MeshColliderBase6EnableEP5Actor(c+0x374, c);
  *(short*)(c+0x31e) = 0;
  *(short*)(c+0x320) = 0;
  *(short*)(c+0x322) = 0;
  *(int*)(c+0x56c) = 0;
  return 1;
}
