//cpp
extern "C" {
extern void func_ov060_02118690(char* c);
extern void _ZN12CylinderClsn5ClearEv(void* p);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void* p, void* v);
extern void _ZN12CylinderClsn6UpdateEv(void* p);
extern int data_ov060_0211b1d8[];
struct V3 { int x,y,z; };
int _ZN17BowserSkyPlatform8BehaviorEv(char* c){
  int idx = *(int*)(c+0x170);
  char* ent = (char*)&data_ov060_0211b1d8[idx*2];
  int adj = *(int*)(ent+4);
  char* self = c + (adj>>1);
  void* fn;
  if(adj&1){ void* vt=*(void**)self; fn=*(void**)((char*)vt + *(int*)ent); }
  else fn=*(void**)ent;
  ((void(*)(char*))fn)(self);
  func_ov060_02118690(c);
  _ZN12CylinderClsn5ClearEv(c+0x124);
  struct V3 v;
  v.x = 0;
  v.y = -0x96000;
  v.z = 0;
  _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c+0x124, &v);
  _ZN12CylinderClsn6UpdateEv(c+0x124);
  return 1;
}
}
