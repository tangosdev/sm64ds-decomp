//cpp
extern "C" void* _ZN5Actor13ClosestPlayerEv(void* self);
extern "C" int Vec3_HorzDist(const void* a, const void* b);
extern "C" void _ZN6Player5ShockEj(void* p, unsigned int x);
extern "C" void _ZN9Animation7AdvanceEv(void* a);
extern "C" void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern "C" int _ZN9Animation8FinishedEv(void* a);
extern "C" void _ZN9ActorBase18MarkForDestructionEv(void* a);
extern int data_ov060_0211ab20[];

extern "C" int func_ov060_02118eb8(char* self){
  char* pl;
  unsigned int n;
  int d;
  int s;
  int m0, m1, m2, m3;
  ((*(unsigned short *)(((int)self + 0x214) & 0xFFFFFFFFFFFFFFFF)))++;
  pl = (char*)_ZN5Actor13ClosestPlayerEv(self);
  n = *(unsigned short*)(self+0x200+0x14);
  s = n * 0x22;
  if(pl != 0 && n < 0x46 && *(unsigned char*)(pl+0x6de) == 0){
    d = Vec3_HorzDist(self+0x5c, pl+0x5c);
    m0 = s*data_ov060_0211ab20[0];
    m1 = s*data_ov060_0211ab20[1];
    m2 = s*data_ov060_0211ab20[2];
    m3 = s*data_ov060_0211ab20[3];
    if((m0 < d && d < m1) || (m2 < d && d < m3)){
      _ZN6Player5ShockEj(pl, 1);
    }
  }
  _ZN9Animation7AdvanceEv(self+0x138);
  _ZN9Animation7AdvanceEv(self+0x1d8);
  _ZN9Animation7AdvanceEv(self+0x14c);
  _ZN9Animation7AdvanceEv(self+0x1ec);
  _ZN9Animation7AdvanceEv(self+0x160);
  _ZN9Animation7AdvanceEv(self+0x200);
  _ZN9Animation7AdvanceEv(self+0x124);
  _ZN9Animation7AdvanceEv(self+0x1c4);
  Matrix4x3_FromTranslation(self+0xf0, *(int*)(self+0x5c)>>3, *(int*)(self+0x60)>>3, *(int*)(self+0x64)>>3);
  Matrix4x3_FromTranslation(self+0x190, *(int*)(self+0x5c)>>3, *(int*)(self+0x60)>>3, *(int*)(self+0x64)>>3);
  if(_ZN9Animation8FinishedEv(self+0x124)){
    _ZN9ActorBase18MarkForDestructionEv(self);
  }
  return 1;
}
