//cpp
extern "C" {
extern void Matrix4x3_FromRotationZXYExt(void* m, int x, int y, int z);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* a, void* sm, void* m, int f1, int f2, unsigned int j);
void func_ov020_02113240(char* c) {
  Matrix4x3_FromRotationZXYExt(c + 0xf0, *(short*)(c + 0x8c), *(short*)(c + 0x8e), *(short*)(c + 0x90));
  *(int*)(c + 0x114) = *(int*)(c + 0x5c) >> 3;
  *(int*)(c + 0x118) = *(int*)(c + 0x60) >> 3;
  *(int*)(c + 0x11c) = *(int*)(c + 0x64) >> 3;
  *(int*)(c + 0x170) = *(int*)(c + 0x5c) >> 3;
  *(int*)(c + 0x174) = *(int*)(c + 0x384) >> 3;
  *(int*)(c + 0x178) = *(int*)(c + 0x64) >> 3;
  _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0x124, c + 0x14c, 0x32000, 0x1e000, 0xf);
}
}
