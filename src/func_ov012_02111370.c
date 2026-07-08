// NONMATCHING: base materialization / addressing (div=15). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern int _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
extern int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern int _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,int,int,void*);
extern int data_ov012_021124a8[];
extern int SAVE_DATA[];
extern int data_ov012_021124a0[];
extern int data_ov012_02111cd0[];
int func_ov012_02111370(char* c){
  int m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov012_021124a8);
  _ZN9ModelBase7SetFileEP8BMD_Fileii((char*)c+0xd4, m, 1, -1);
  if(SAVE_DATA[2] & 0x80000){
    *(int*)((char*)c+0x60) -= 0x64000;
    *(char*)((char*)c+0x31e) = 1;
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
  }
  int k = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov012_021124a0);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block((char*)c+0x124, k, (char*)c+0x2ec, 0x199, *(short*)((char*)c+0x8e), (void*)data_ov012_02111cd0);
  return 1;
}
