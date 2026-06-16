//cpp
extern "C" {
extern void _ZN9Animation7AdvanceEv(char* a);
extern int DecIfAbove0_Byte(char* a);
extern void func_ov070_02120da8(char* c, int x);
int func_ov070_02120cac(char* c){
  _ZN9Animation7AdvanceEv(c+0x124);
  if(DecIfAbove0_Byte(c+0x430)==0){
    func_ov070_02120da8(c, 1);
  }
  return 1;
}
}
