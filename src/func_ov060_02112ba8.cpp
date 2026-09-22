//cpp
extern "C" {
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);
int func_ov060_02112ba8(char* c){
  int s = *(int*)(c+0x40c);
  if(s != 2 && s != 0x13){
    if(*(int*)(c+0x60) < *(int*)(c+0x3b4) - 0x3e8000) return 1;
    _ZNK10dBgCh_Actr10IsOnGroundEv((char*)c+0x14c);
  }
  return 0;
}
}
