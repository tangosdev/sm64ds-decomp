//cpp
extern "C" {
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(void* a, void* sm, void* m, int f1, int f2, int f3, unsigned int j);
void func_ov065_0211bc88(char* c) {
  int d = *(int*)(c + 0x60) - *(int*)(c + 0x330);
  if (d < 0) d = -d;
  if (d > 0x7d0000) return;
  Matrix4x3_FromRotationY(c + 0x35c, *(short*)(c + 0x8e));
  *(int*)(c + 0x380) = *(int*)(c + 0x5c) >> 3;
  *(int*)(c + 0x384) = (*(int*)(c + 0x330) + 0x1000) >> 3;
  *(int*)(c + 0x388) = *(int*)(c + 0x64) >> 3;
  _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(c, c + 0x334, c + 0x35c, 0x1e0000, 0x32000, 0xfa000, 0xf);
}
}
