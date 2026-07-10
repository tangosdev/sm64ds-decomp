//cpp
extern "C" {
int _ZN5Model8LoadFileER13SharedFilePtr(void*);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
int _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,int,int,void*);
void func_020393d4(void* p, void* v);
void func_020393c4(void* p, void* v);
int _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void*,void*,int,int,int,int);
extern int data_ov036_0211408c[];
extern int data_ov036_02114084[];
extern int data_ov036_02112b48[];
extern void _ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_();
extern void func_ov036_02111cc4();
int _ZN8ShipWing13InitResourcesEv(char* c){
  int m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov036_0211408c);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, m, 1, -1);
  _ZN8Platform21UpdateModelPosAndRotYEv(c);
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  int k = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov036_02114084);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c+0x124, k, c+0x2ec, 0x1000, *(short*)(c+0x8e), data_ov036_02112b48);
  func_020393d4(c+0x124, (void*)_ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  func_020393c4(c+0x124, (void*)func_ov036_02111cc4);
  _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c+0x320, c, 0x32000, 0x64000, 0, 0);
  *(int*)(c+0xa0) = -0x1e000;
  *(int*)(c+0x9c) = ~0x198;
  *(int*)(c+0x4dc) = *(int*)(c+0x5c);
  *(int*)(c+0x4e0) = *(int*)(c+0x60);
  *(int*)(c+0x4e4) = *(int*)(c+0x64);
  return 1;
}
}
