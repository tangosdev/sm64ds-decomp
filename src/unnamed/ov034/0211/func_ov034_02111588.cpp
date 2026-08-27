//cpp
extern "C" int DecIfAbove0_Byte(void*);
extern "C" void func_ov034_021125b8(void*, int);
extern "C" void func_ov034_02111588(char *c){
  if(DecIfAbove0_Byte(c+0x8da)) return;
  func_ov034_021125b8(c, 0xa);
}
