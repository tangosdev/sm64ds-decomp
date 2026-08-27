struct V3 { int a, b, c; };
extern void func_ov002_020dc174(char* c, struct V3* r1, int r2, int r3, unsigned int a5, unsigned int a6);
void func_ov002_020dc020(char* c){
  struct V3 v;
  int r2, r3;
  if(*(unsigned char*)(c+0x703) == 0){
    v.a = 0;
    v.b = 0xa000;
    v.c = 0x3c000;
    r3 = 0x46000;
    r2 = 0x28000;
  }else{
    v.a = 0;
    v.b = 0x40000;
    v.c = 0xc0000;
    r3 = 0x100000;
    r2 = 0x80000;
  }
  func_ov002_020dc174(c, &v, r2, r3, 0x1000, 0);
}
