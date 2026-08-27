//cpp
extern "C" {
void Matrix4x3_FromRotationY(void* m, short angle);
void _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(void* self, void* m, short s);
void func_ov024_02111480(char* c) {
  Matrix4x3_FromRotationY(c+0x370, *(short*)(c+0x8e));
  *(int*)(c+0x394) = *(int*)(c+0x5c);
  *(int*)(c+0x398) = *(int*)(c+0x60);
  *(int*)(c+0x39c) = *(int*)(c+0x64);
  _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(c+0x124, c+0x370, *(short*)(c+0x8e));
}
}
