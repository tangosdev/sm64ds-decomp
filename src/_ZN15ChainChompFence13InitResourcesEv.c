extern int _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
extern int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern int _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,int,int,void*);
extern int data_ov021_021149c0[];
extern int data_ov021_021149b8[];
extern int data_ov022_02114558[];
int _ZN15ChainChompFence13InitResourcesEv(char* c){
  int m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov021_021149c0);
  _ZN9ModelBase7SetFileEP8BMD_Fileii((char*)c+0xd4, m, 1, -1);
  _ZN8Platform21UpdateModelPosAndRotYEv(c);
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  int k = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov021_021149b8);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block((char*)c+0x124, k, (char*)c+0x2ec, 0x1000, *(short*)(c+0x8e), (void*)data_ov022_02114558);
  return 1;
}
