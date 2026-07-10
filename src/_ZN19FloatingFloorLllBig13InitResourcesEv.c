typedef int Fix12;
typedef short s16;
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, Fix12, short, void*);
extern int func_020393d4(void*, void*);
extern char data_ov022_02114620[];
extern char data_ov022_02114618[];
extern char data_ov064_0211ba6c[];
extern int _ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(void);

int _ZN19FloatingFloorLllBig13InitResourcesEv(char* c) {
  void* mdl;
  void* kcl;
  mdl = _ZN5Model8LoadFileER13SharedFilePtr(data_ov022_02114620);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, mdl, 1, -1);
  _ZN8Platform21UpdateModelPosAndRotYEv(c);
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov022_02114618);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c+0x124, kcl, c+0x2ec, 0x1000, *(short*)(c+0x8e), data_ov064_0211ba6c);
  func_020393d4(c+0x124, &_ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  *(int*)(c+0x320) = *(int*)(c+0x60);
  *(s16*)(c+0x324) = *(s16*)(c+0x8c);
  return 1;
}
