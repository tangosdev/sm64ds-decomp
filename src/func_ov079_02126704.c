typedef int Fix12;
typedef short s16;
extern void Matrix4x3_FromRotationXYZExt(void* m, int x, int y, int z);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* self, void* sm, void* mtx, Fix12 a, Fix12 b, unsigned int g);

struct M48 { int w[12]; };

void func_ov079_02126704(char* c) {
  Matrix4x3_FromRotationXYZExt(c+0x328, *(s16*)(c+0x8c), *(s16*)(c+0x8e), *(s16*)(c+0x90));
  *(int*)(c+0x34c) = *(int*)(c+0x5c) >> 3;
  *(int*)(c+0x350) = *(int*)(c+0x60) >> 3;
  *(int*)(c+0x354) = *(int*)(c+0x64) >> 3;
  *(struct M48*)(c+0x378) = *(struct M48*)(c+0x328);
  _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c+0x3ac, c+0x328, 0x50000, 0x50000, 0xf);
}
