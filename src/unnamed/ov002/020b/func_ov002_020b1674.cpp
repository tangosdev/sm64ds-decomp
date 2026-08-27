//cpp
extern "C" {
extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(void*);
extern int func_02012694(int, void*);
extern int GiveCoins(int, int);
extern int _ZN6Player4HealEi(void*, int);
void func_ov002_020b1674(char* c, char* p){
  *(short*)(c+0x3a8)=0;
  _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
  func_02012694(0x1c, c+0x74);
  GiveCoins(*(unsigned char*)(p+0x6d8), 5);
  _ZN6Player4HealEi(p, 0x500);
}
}
