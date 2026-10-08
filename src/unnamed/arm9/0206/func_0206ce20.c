#pragma optimize_for_size on
extern int func_0206cf7c(int mask);
struct S { unsigned short f0; unsigned short f2; unsigned short f4; };
void func_0206ce20(struct S* s){
  unsigned short v = *(volatile unsigned short*)0x4000204;
  s->f0 = v;
  *(volatile unsigned short*)0x4000204 = (v & 0xffe3) | 0xf;
  if(func_0206cf7c(1) != 0){
    s->f4 = 1;
    s->f2 = *(volatile unsigned short*)0x27fff72;
    *(volatile unsigned short*)0x9fe2ffe = 0x20;
    *(volatile unsigned short*)0x27fff72 = 0x20;
  }
}
