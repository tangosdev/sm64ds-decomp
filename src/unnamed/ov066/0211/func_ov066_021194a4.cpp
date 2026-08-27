//cpp
extern "C" void Matrix4x3_FromRotationY(void*, short);
extern "C" void _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(void*, void*, short);
extern "C" void func_ov066_021194a4(char *c) {
  Matrix4x3_FromRotationY(c+0x83c, *(short*)(c+0x8e));
  *(int*)(c+0x860) = *(int*)(c+0x5c);
  *(int*)(c+0x864) = *(int*)(c+0x60);
  *(int*)(c+0x868) = *(int*)(c+0x64);
  _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(c+0x674, c+0x83c, *(short*)(c+0x8e));
}
