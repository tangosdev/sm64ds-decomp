//cpp
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* f, int a, int b);
extern int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* self, void* kcl, void* mtx, int fix, short s, void* clps);
extern void func_020393d4(int* p, void* v);
extern void _ZN7PathPtr6FromIDEj(void* self, unsigned int id);
extern int _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;
struct SFP { void* a; void* b; void* c; };
extern struct SFP data_ov052_021125a0;
int _ZN14SquarePathLift13InitResourcesEv(char* c){
  void* f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov052_021125a0.a);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, f, 1, -1);
  _ZN8Platform21UpdateModelPosAndRotYEv(c);
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  void* f2 = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov052_021125a0.b);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c+0x124, f2, c+0x2ec, 0x199, *(short*)(c+0x8e), data_ov052_021125a0.c);
  func_020393d4((int*)(c+0x124), &_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  _ZN7PathPtr6FromIDEj(c+0x320, *(int*)(c+8)&0xff);
  *(int*)(c+0x32c) = 1;
  *(int*)(c+0x98) = 0xa000;
  return 1;
}
}
