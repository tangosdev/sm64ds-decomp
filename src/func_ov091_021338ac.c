typedef int Fix12;
extern int _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
extern int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern int _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,Fix12,short,void*);
extern int data_ov091_02135654[];
extern int data_ov091_0213564c[];
extern int data_ov002_0210d874[];
int func_ov091_021338ac(char* c){
  int m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov091_02135654);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, m, 1, -1);
  _ZN8Platform21UpdateModelPosAndRotYEv(c);
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  int k = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov091_0213564c);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c+0x124, k, c+0x2ec, 0x199, *(short*)(c+0x8e), (void*)data_ov002_0210d874);
  *(unsigned char*)(c+0x31e) = 3;
  return 1;
}
