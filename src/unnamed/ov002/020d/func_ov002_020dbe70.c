extern unsigned char data_0209f2d8;
struct V3 { int a, b, c; };
extern void func_ov002_020dc174(char* c, struct V3* r1, int r2, int r3, unsigned int a5, unsigned int a6);
void func_ov002_020dbe70(char* c){
  struct V3 v;
  int r2, r3;
  if((int)(data_0209f2d8 == 1) != 0){
    v.c = 0x78000;
    v.a = 0;
    v.b = 0xa000;
    v.c = 0x5a000;
    r3 = 0x82000;
    r2 = 0x64000;
  }else if(*(unsigned char*)(c+0x703) == 0){
    v.a = 0;
    v.b = 0xa000;
    v.c = 0x5a000;
    r2 = 0x46000;
    r3 = 0x82000;
    if(*(int*)(c+8) == 2){
      v.c = 0x78000;
      r2 = 0x64000;
    }
  }else{
    v.a = 0;
    v.b = 0x40000;
    v.c = 0xe0000;
    r3 = 0x140000;
    r2 = 0xa0000;
  }
  func_ov002_020dc174(c, &v, r2, r3, 0x40, 0xc0);
}
