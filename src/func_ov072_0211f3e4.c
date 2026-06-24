typedef int Fix12;
typedef short s16;
extern void Matrix4x3_FromRotationXYZExt(void* m, int x, int y, int z);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* self, void* sm, void* mtx, Fix12 a, Fix12 b, unsigned int g);

void func_ov072_0211f3e4(char* c) {
  Matrix4x3_FromRotationXYZExt(c+0xf0, *(s16*)(c+0x8c), *(s16*)(c+0x8e), *(s16*)(c+0x90));
  *(int*)(c+0x114) = *(int*)(c+0x5c) >> 3;
  *(int*)(c+0x118) = (*(int*)(c+0x60) + *(int*)(c+0x398)) >> 3;
  *(int*)(c+0x11c) = *(int*)(c+0x64) >> 3;
  *(int*)(c+0x374) = *(int*)(c+0x5c) >> 3;
  *(int*)(c+0x378) = (*(int*)(c+0x60) + *(int*)(c+0x398)) >> 3;
  *(int*)(c+0x37c) = *(int*)(c+0x64) >> 3;
  _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c+0x124, c+0x350, *(int*)(c+0x398) << 1, *(int*)(c+0x398) << 1, 0xf);
}
