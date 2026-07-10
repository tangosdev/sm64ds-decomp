//cpp
extern "C" {
extern int _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
extern int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern int _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,int,int,void*);
extern void func_020393d4(int* p, int v);
extern int func_01ffb0a4(void*);
extern int func_01ffb07c(void*,void*);
extern void func_020396d0(int* p, int v);
extern void func_ov015_02111fb8(void* self, int idx);
extern int data_ov015_02114534[];
extern int data_ov034_02114538[];
extern int data_ov015_0211453c[];
extern int _ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;
int _ZN14KnockDownPlank13InitResourcesEv(char* c){
  int b = (*(unsigned short*)((char*)c+0xc) == 0x35) ? 1 : 0;
  if(b) *(int*)((char*)c+0x32c) = 1; else *(int*)((char*)c+0x32c) = 0;
  int j0 = *(int*)((char*)c+0x32c) * 0xc;
  int m = _ZN5Model8LoadFileER13SharedFilePtr(*(void**)((char*)data_ov015_02114534 + j0));
  _ZN9ModelBase7SetFileEP8BMD_Fileii((char*)c+0xd4, m, 1, -1);
  _ZN8Platform21UpdateModelPosAndRotYEv(c);
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  int j = *(int*)((char*)c+0x32c) * 0xc;
  int k = _ZN12MeshCollider8LoadFileER13SharedFilePtr(*(void**)((char*)data_ov034_02114538 + j));
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block((char*)c+0x124, k, (char*)c+0x2ec, 0x1000, *(short*)(c+0x8e), *(void**)((char*)data_ov015_0211453c + j));
  func_020393d4((int*)((char*)c+0x124), (int)&_ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  int tmp[3];
  tmp[0] = 0x1000; tmp[1] = 0; tmp[2] = 0;
  func_01ffb0a4((char*)c+0x124);
  func_01ffb07c((char*)c+0x124, tmp);
  func_020396d0((int*)((char*)c+0x124), 0xccd);
  *(int*)((char*)c+0x320) = *(int*)((char*)c+0x5c);
  *(int*)((char*)c+0x324) = *(int*)((char*)c+0x60);
  *(int*)((char*)c+0x328) = *(int*)((char*)c+0x64);
  func_ov015_02111fb8(c, 5);
  return 1;
}
}
