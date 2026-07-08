//cpp
extern "C" {
extern int func_ov002_020d98b4(void*);
extern int _ZN6Player7ST_WALKE[];
extern int _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
int _ZN6Player18St_DizzyStars_MainEv(char* c){
  func_ov002_020d98b4(c);
  if(*(short*)(c+0x6a4) > 0) return 1;
  _ZN6Player11ChangeStateERNS_5StateE(c, _ZN6Player7ST_WALKE);
  return 1;
}
}
