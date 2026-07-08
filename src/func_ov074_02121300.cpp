//cpp
extern "C" {
typedef int Fix12;
struct Vec3 { int x,y,z; };
struct M43 { int w[12]; };
extern void Vec3_Asr(struct Vec3* d, struct Vec3* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* m, short a);
extern void Matrix4x3_ApplyInPlaceToScale(void* m, Fix12 x, Fix12 y, Fix12 z);
extern struct M43 MATRIX_SCRATCH_PAPER;
void func_ov074_02121300(char* c){
  struct Vec3 v;
  Vec3_Asr(&v, (struct Vec3*)(c+0x5c), 3);
  Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, v.x, v.y, v.z);
  Matrix4x3_ApplyInPlaceToRotationY(&MATRIX_SCRATCH_PAPER, *(short*)(c+0x8e));
  Matrix4x3_ApplyInPlaceToScale(&MATRIX_SCRATCH_PAPER, *(int*)(c+0x80), *(int*)(c+0x84), *(int*)(c+0x88));
  *(struct M43*)(c+0x37c) = MATRIX_SCRATCH_PAPER;
}
}
