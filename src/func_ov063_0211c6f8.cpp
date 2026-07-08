//cpp
struct M48 { int w[12]; };
extern "C" void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern "C" void _ZN18MovingMeshCollider9TransformERK9Matrix4x3s(void* self, void* mtx, short s);
extern "C" struct M48 MATRIX_SCRATCH_PAPER;
extern "C" void func_ov063_0211c6f8(char* c);
extern "C" void func_ov063_0211c6f8(char* c) {
  Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, *(int*)(c+0x5c), *(int*)(c+0x60), *(int*)(c+0x64));
  Matrix4x3_ApplyInPlaceToRotationXYZExt(&MATRIX_SCRATCH_PAPER, *(short*)(c+0x8c), *(short*)(c+0x8e), *(short*)(c+0x90));
  *(struct M48*)(c+0x324) = MATRIX_SCRATCH_PAPER;
  _ZN18MovingMeshCollider9TransformERK9Matrix4x3s(c+0x15c, c+0x324, *(short*)(c+0x8e));
}
