//cpp
extern "C" {
extern void _ZN9Animation7AdvanceEv(void* p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* a, void* b);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void* a, void* b, unsigned int j);
extern void _ZN12CylinderClsn5ClearEv(void* p);
extern void _ZN12CylinderClsn6UpdateEv(void* p);
extern void func_ov062_0211aac0(void* c);
extern int data_ov062_0211e0a4[];
int func_ov062_0211ab88(char* c){
  int idx = *(int*)(c+0x38c);
  char* ent = (char*)&data_ov062_0211e0a4[idx*2];
  int adj = *(int*)(ent+4);
  char* self = c + (adj>>1);
  void* fn;
  if(adj&1){ void* vt=*(void**)self; fn=*(void**)((char*)vt + *(int*)ent); }
  else fn=*(void**)ent;
  ((void(*)(char*))fn)(self);
  _ZN9Animation7AdvanceEv(c+0x350);
  *(short*)(c+0x8e) = *(short*)(c+0x94);
  _ZN5Actor9UpdatePosEP12CylinderClsn(c, c+0x110);
  _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c+0x144, 0);
  _ZN12CylinderClsn5ClearEv(c+0x110);
  _ZN12CylinderClsn6UpdateEv(c+0x110);
  func_ov062_0211aac0(c);
  return 1;
}
}
