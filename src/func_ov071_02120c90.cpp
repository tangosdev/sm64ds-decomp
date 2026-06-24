//cpp
extern "C" {
typedef int Fix12;
typedef short s16;
void Matrix4x3_FromRotationXYZExt(void* m, int x, int y, int z);
void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* self, void* sm, void* mtx, Fix12 a, Fix12 b, unsigned int g);

void func_ov071_02120c90(char* c) {
  Matrix4x3_FromRotationXYZExt(c+0xf0, *(s16*)(c+0x8c), *(s16*)(c+0x8e), *(s16*)(c+0x90));
  *(int*)(c+0x114) = *(int*)(c+0x5c) >> 3;
  *(int*)(c+0x118) = *(int*)(c+0x60) >> 3;
  *(int*)(c+0x11c) = *(int*)(c+0x64) >> 3;
  *(int*)(c+0x1d8) = *(int*)(c+0x5c) >> 3;
  *(int*)(c+0x1dc) = *(int*)(c+0x60) >> 3;
  *(int*)(c+0x1e0) = *(int*)(c+0x64) >> 3;
  _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c+0x14c, c+0x1b4, *(int*)(c+0x1f0) * 0xb4, *(int*)(c+0x200), 0xf);
}
}
