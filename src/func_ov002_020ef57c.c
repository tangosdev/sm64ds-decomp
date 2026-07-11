extern int func_ov002_020efe68(char* c);
extern int Vec3_HorzDist(const void* a, const void* b);

void func_ov002_020ef57c(char* c){
  int b = (int)(*(unsigned short*)(c+0xc) == 0x82);
  if (b != 0) {
    *(int*)(c+0x9c) = -0x1000;
    *(int*)(c+0xa0) = -0x1e000;
  } else {
    *(int*)(c+0x9c) = -0x2000;
    *(int*)(c+0xa0) = -0x3c000;
  }
  *(unsigned char*)(c+0x42b) = 0xfa;
  *(short*)(c+0x92) = *(short*)(c+0x8c);
  *(short*)(c+0x94) = *(short*)(c+0x8e);
  *(short*)(c+0x96) = *(short*)(c+0x90);
  *(int*)(c+0xa8) = 0;
  if (func_ov002_020efe68(c) == 0) {
    *(unsigned char*)(c+0x448) = 0x3c;
    *(unsigned char*)(c+0x449) = 0;
  } else {
    *(unsigned char*)(c+0x448) = 0x1e;
    *(unsigned char*)(c+0x449) = 0x18;
    *(short*)(c+0x44a) = 0;
    *(int*)(c+0x444) = *(int*)(c+0x60);
  }
  {
    int b2 = (int)(*(unsigned short*)(c+0xc) == 0x82);
    if (b2 == 0) {
      return;
    }
    *(int*)(c+0x98) = Vec3_HorzDist(c+0x5c, c+0x68);
  }
}
