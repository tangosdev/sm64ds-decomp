//cpp
extern "C" {
extern int func_ov002_020d82f0(void*);
extern int _ZN6Player12ST_BURN_FIREE[];
extern void _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
int _ZN6Player4BurnEv(char* c){
  if(func_ov002_020d82f0(c)==0) return 0;
  if(*(unsigned char*)(c+0x6f9) || *(unsigned char*)(c+0x706)) return 0;
  _ZN6Player11ChangeStateERNS_5StateE(c,_ZN6Player12ST_BURN_FIREE);
  return 1;
}
}
