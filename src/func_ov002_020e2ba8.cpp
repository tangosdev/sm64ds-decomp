//cpp
extern "C" {
extern int _ZN6Player7ST_JUMPE[];
extern void _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
int func_ov002_020e2ba8(char* c){
  if(*(unsigned char*)(c+0x703)==0) return 0;
  _ZN6Player11ChangeStateERNS_5StateE(c,_ZN6Player7ST_JUMPE);
  return 1;
}
}
