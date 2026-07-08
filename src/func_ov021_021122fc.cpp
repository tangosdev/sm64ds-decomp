//cpp
extern "C" {
extern void Vec3_Asr(void* dst, void* src, int n);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, short a, short b, short c);
extern int _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* thiz, void* sm, void* m, int rad, int h, unsigned u);
struct Mtx { int w[12]; };
extern struct Mtx MATRIX_SCRATCH_PAPER;
int func_ov021_021122fc(char* c){
  int tmp[3];
  Vec3_Asr(tmp, c+0x5c, 3);
  Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, tmp[0], tmp[1], tmp[2]);
  *(struct Mtx*)(c+0x188) = MATRIX_SCRATCH_PAPER;
  Matrix4x3_ApplyInPlaceToRotationXYZExt(&MATRIX_SCRATCH_PAPER, *(short*)(c+0x8c), *(short*)(c+0x8e), *(short*)(c+0x90));
  *(struct Mtx*)(c+0x12c) = MATRIX_SCRATCH_PAPER;
  return _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c+0x160, c+0x188, 0x1f4000, 0x1f4000, 0xf);
}
}
