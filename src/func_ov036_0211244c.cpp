//cpp
extern "C" {
int _ZN5Model8LoadFileER13SharedFilePtr(void*);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
int _ZN9Animation8LoadFileER13SharedFilePtr(void*);
int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,int,int,int,unsigned);
void func_ov036_021122c0(char* c);
int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
int _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,int,int,void*);
void func_020393d4(void* p, void* v);
void func_020efaf0(void* c);
extern int data_ov002_0210d9f0[];
extern int data_ov036_02113f58[];
extern int data_ov036_0211419c[];
extern void _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_();
int func_ov036_0211244c(char* c){
  _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210d9f0);
  int m = _ZN5Model8LoadFileER13SharedFilePtr((void*)data_ov036_02113f58[0]);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0x450, m, 1, -1);
  _ZN9Animation8LoadFileER13SharedFilePtr(data_ov036_0211419c);
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x450, data_ov036_0211419c[1], 0, 0x1000, 0);
  func_ov036_021122c0(c);
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  int k = _ZN12MeshCollider8LoadFileER13SharedFilePtr((void*)data_ov036_02113f58[1]);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c+0x124, k, c+0x2ec, 0x199, *(short*)(c+0x8e), (void*)data_ov036_02113f58[2]);
  func_020393d4(c+0x124, (void*)_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  *(int*)(c+0x440) = 0xa000;
  *(int*)(c+0x98) = *(int*)(c+0x440);
  func_020efaf0(c);
  *(int*)(c+0x43c) = 1;
  *(unsigned char*)(c+0x42c) = 1;
  return 1;
}
}
