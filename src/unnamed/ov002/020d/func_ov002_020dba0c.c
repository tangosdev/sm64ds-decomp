extern unsigned char data_0209f2d8;
struct V3 { int a, b, c; };
extern void func_ov002_020dc174(char* c, struct V3* r1, int r2, int r3, unsigned int a5, unsigned int a6);
void func_ov002_020dba0c(char* c){
  struct V3 v;
  int r2, r3;
  unsigned int a5;
  if((int)(data_0209f2d8 == 1) != 0){
    v.c = 0xa0000;
    v.a = 0;
    r2 = 0x78000;
    v.b = 0;
    v.c = 0x78000;
    r3 = 0x50000;
  }else if(*(unsigned char*)(c+0x703) == 0){
    r2 = 0x78000;
    v.c = 0x78000;
    r3 = 0x50000;
    v.a = 0;
    v.b = 0;
    r2 = r3;
    if(*(int*)(c+8) == 2){
      v.c = 0xa0000;
      r2 = 0x78000;
    }
  }else{
    v.a = 0;
    v.b = -0x40000;
    v.c = 0x80000;
    r3 = 0x100000;
    r2 = 0xc0000;
  }
  a5 = 0x1400;
  if(*(int*)(c+8) == 3) a5 = 0x400;
  func_ov002_020dc174(c, &v, r2, r3, a5, 0);
}
