//cpp
extern "C" {
extern int _ZN6Player12FinishedAnimEv(void* c);
extern int func_ov002_020c44c4(void* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* st);
extern int _ZN6Player6ST_FLYE[];
void func_ov002_020c7194(char* c){
  if(_ZN6Player12FinishedAnimEv(c) == 0) return;
  *(unsigned char*)(c+0x6e3) = 1;
  if(func_ov002_020c44c4(c) != 0) return;
  _ZN6Player11ChangeStateERNS_5StateE(c, _ZN6Player6ST_FLYE);
}
}
