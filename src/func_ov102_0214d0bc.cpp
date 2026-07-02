//cpp
extern "C" {
int _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void*, void*, int, int, unsigned int, unsigned int);
int func_ov102_0214d0bc(char* c){
  _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c+0x110, c, 0, 0, 0x100002, 0);
  (*(int *)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFF)) &= ~3;
  *(unsigned char*)(c+0x3c6) = 0x64;
  return 1;
}
}
