//cpp
extern "C" {
extern int data_ov002_021104cc[];
extern void _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
int func_ov002_020e2b6c(char* c){
  if(*(int*)(c+0x68c) < 0xb000) return 0;
  _ZN6Player11ChangeStateERNS_5StateE(c,data_ov002_021104cc);
  return 1;
}
}
