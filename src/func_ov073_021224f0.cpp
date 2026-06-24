//cpp
extern "C" {
extern void Matrix4x3_FromRotationXYZExt(void* m, int x, int y, int z);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* c);
int func_ov073_021224f0(char* c){
  void* o = *(void**)(c+0x320);
  if(*(int*)((char*)o+8)){
    char* base = (char*)o+8;
    int adj = *(int*)(base+4);
    char* self = c + (adj>>1);
    void* fn;
    if(adj&1){ void* vt=*(void**)self; fn=*(void**)((char*)vt + *(int*)base); }
    else fn=*(void**)base;
    ((void(*)(char*))fn)(self);
  }
  Matrix4x3_FromRotationXYZExt(c+0xf0, *(short*)(c+0x8c), *(short*)(c+0x8e), *(short*)(c+0x90));
  *(int*)(c+0x114) = *(int*)(c+0x5c) >> 3;
  *(int*)(c+0x118) = *(int*)(c+0x60) >> 3;
  *(int*)(c+0x11c) = *(int*)(c+0x64) >> 3;
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  return 1;
}
}
