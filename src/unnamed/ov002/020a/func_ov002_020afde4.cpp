//cpp
extern "C" {
extern char* _ZN8dActor_c13ClosestPlayerEv(char*);
extern short Vec3_HorzAngle(void*, void*);
extern void func_ov002_020af3a8(char*);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(char*);
extern int _ZN8dActor_c15IsPlayerInRangeEi(char*, int);
void func_ov002_020afde4(char* c){
  char* p = _ZN8dActor_c13ClosestPlayerEv(c);
  if(p){
    *(short*)(c+0x94) = Vec3_HorzAngle(c+0x5c, p+0x5c) + 0x8000;
  }
  func_ov002_020af3a8(c);
  if(_ZNK10dBgCh_Actr8IsOnWallEv(c+0x144)) *(int*)(c+0x388)=2;
  if(_ZN8dActor_c15IsPlayerInRangeEi(c, 0xbb8)==0) *(int*)(c+0x388)=2;
}
}
