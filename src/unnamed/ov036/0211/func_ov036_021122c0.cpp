//cpp
extern "C" {
extern void Matrix4x3_FromRotationXYZExt(void*, short, short, short);
void func_ov036_021122c0(char *c){
  Matrix4x3_FromRotationXYZExt(c+0x46c, *(short*)(c+0x8c), *(short*)(c+0x8e), *(short*)(c+0x90));
  *(int*)(c+0x490) = *(int*)(c+0x5c) >> 3;
  *(int*)(c+0x494) = (*(int*)(c+0x60) + *(int*)(c+0x4bc)) >> 3;
  *(int*)(c+0x498) = *(int*)(c+0x64) >> 3;
}
}
