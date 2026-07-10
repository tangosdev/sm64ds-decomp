//cpp
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* c, void* file, int a, int b);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void* c);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* c);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* f);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* c, void* kcl, void* m, int fix, short s, void* clps);
extern void func_020393d4(int* p, void* v);
extern int _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_();
extern void* data_ov015_02114a8c;
extern void* data_ov015_02114a84;
extern void* data_ov015_02113654;
int _ZN9TowerStep13InitResourcesEv(char* c) {
  void* f;
  f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov015_02114a8c);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, f, 1, -1);
  _ZN8Platform21UpdateModelPosAndRotYEv(c);
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  f = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov015_02114a84);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
      c + 0x124, f, c + 0x2ec, 0x199, *(short*)(c + 0x8e), &data_ov015_02113654);
  func_020393d4((int*)(c + 0x124), (void*)&_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  *(unsigned char*)(c + 0x31e) = 0x3c;
  return 1;
}
}
