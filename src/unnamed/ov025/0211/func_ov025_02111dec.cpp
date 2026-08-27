//cpp
extern "C" void Matrix4x3_FromRotationY(void*, short);
extern "C" void _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(char*, void*, short);
extern "C" void func_ov025_02111dec(char *c){
  Matrix4x3_FromRotationY(c+0x374, *(short*)(c+0x8e));
  *(int*)(c+0x398)=*(int*)(c+0x5c);
  *(int*)(c+0x39c)=*(int*)(c+0x60);
  *(int*)(c+0x3a0)=*(int*)(c+0x64);
  _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(c+0x124, c+0x374, *(short*)(c+0x8e));
}
