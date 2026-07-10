//cpp
extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void* a, void* b);
int _ZN9RabbitKey8BehaviorEv(char* c){
  DecIfAbove0_Short((unsigned short*)(c+0x100));
  void* o = *(void**)(c+0x188);
  if(*(int*)((char*)o+8)){
    char* base = (char*)o+8;
    int adj = *(int*)(base+4);
    char* self = c + (adj>>1);
    void* fn;
    if(adj&1){ void* vt=*(void**)self; fn=*(void**)((char*)vt + *(int*)base); }
    else fn=*(void**)base;
    ((void(*)(char*))fn)(self);
  }
  int s = *(int*)(c+0xa8) + *(int*)(c+0x9c);
  int lim = *(int*)(c+0xa0);
  if(s >= lim) lim = s;
  int t = *(int*)(c+0xac);
  *(int*)(c+0xa8) = lim;
  *(int*)(c+0xac) = t;
  _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(c, 0);
  return 1;
}
}
