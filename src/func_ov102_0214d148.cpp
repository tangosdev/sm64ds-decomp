//cpp
extern "C" {
int _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void*, void*, int, int, unsigned int, unsigned int);
int func_ov102_0214d148(char* c){
  *(short*)(c+0x94) = *(short*)(c+0x3bc);
  *(short*)(c+0x8e) = *(short*)(c+0x94);
  *(int*)(c+0x98) = 0x1e000;
  *(int*)(c+0x9c) = -0x2000;
  _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c+0x378, c, 0x28000, 0x3c000, 0x102002, 0);
  return 1;
}
}
