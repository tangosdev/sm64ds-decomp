//cpp
extern "C" {
extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
extern char _ZN6Player8ST_SHELLE[];
extern char _ZN6Player15ST_CRAZED_CRATEE[];
extern char _ZN6Player12ST_BURN_LAVAE[];
int func_ov002_020d5338(char* c){
  if (_ZN6Player7IsStateERNS_5StateE(c, _ZN6Player8ST_SHELLE)
      || _ZN6Player7IsStateERNS_5StateE(c, _ZN6Player15ST_CRAZED_CRATEE)
      || *(unsigned char*)(c+0x6f9) != 0
      || *(unsigned char*)(c+0x709) != 0) return 0;
  _ZN6Player11ChangeStateERNS_5StateE(c, _ZN6Player12ST_BURN_LAVAE);
  return 1;
}
}
