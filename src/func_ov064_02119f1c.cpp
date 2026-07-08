//cpp
extern "C" {
struct Vec3 { int x,y,z; };
struct M43 { int w[12]; };
extern void Vec3_Asr(struct Vec3* d, struct Vec3* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void _ZN9ModelBase12ApplyOpacityEj(void* m, unsigned int j, int k);
extern struct M43 MATRIX_SCRATCH_PAPER;
void func_ov064_02119f1c(char* c){
  struct Vec3 v;
  Vec3_Asr(&v, (struct Vec3*)(c+0x5c), 3);
  Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, v.x, v.y, v.z);
  Matrix4x3_ApplyInPlaceToRotationXYZExt(&MATRIX_SCRATCH_PAPER, *(short*)(c+0x8c), *(short*)(c+0x8e), *(short*)(c+0x90));
  _ZN9ModelBase12ApplyOpacityEj(c+0x30c, *(unsigned char*)(c+0x380), 1);
  *(struct M43*)(c+0x328) = MATRIX_SCRATCH_PAPER;
}
}
