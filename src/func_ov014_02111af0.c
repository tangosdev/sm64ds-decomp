extern int func_ov014_02111f54(void*);
extern int Math_Function_0203b14c(void*,int,int,int,int);
extern int DecIfAbove0_Short(void*);
extern int func_ov014_02111ebc(void*,int);
extern int _ZN9Animation7AdvanceEv(void*);
int func_ov014_02111af0(char* c){
  int r = func_ov014_02111f54(c);
  if(r) return r;
  if(Math_Function_0203b14c((char*)c+0x5f8, 0x64000, 0x800, 0x10000, 0x800)) goto adv;
  if(DecIfAbove0_Short((char*)c+0x5fc)) goto adv;
  func_ov014_02111ebc(c, 1);
adv:
  return _ZN9Animation7AdvanceEv((char*)c+0x1a0);
}
