typedef int Fix12;
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, Fix12, short, void*);
extern int data_ov012_021124a8[];
extern int data_0209caa0[];
extern int data_ov012_021124a0[];
extern int data_ov012_02111cd0[];

int func_ov012_02111370(char* c){
  void* mdl;
  void* kcl;
  mdl = _ZN5Model8LoadFileER13SharedFilePtr(data_ov012_021124a8);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, mdl, 1, -1);
  if(data_0209caa0[2] & 0x80000){
    *(int*)(((long long)(int)(c + 0x60)) & 0xFFFFFFFFFFFFFFFFLL) -= 0x64000;
    *(char*)(c+0x31e) = 1;
  }
  _ZN8Platform21UpdateModelPosAndRotYEv(c);
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov012_021124a0);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c+0x124, kcl, c+0x2ec, 0x199, *(short*)(c+0x8e), data_ov012_02111cd0);
  return 1;
}
