//cpp
typedef short s16;
struct Mtx43 { int w[12]; };
extern "C" {
void Matrix4x3_FromTranslation(Mtx43* m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(Mtx43* m, s16 a);
void Matrix4x3_ApplyInPlaceToRotationX(Mtx43* m, s16 a);
void Matrix4x3_ApplyInPlaceToRotationZ(Mtx43* m, s16 a);
extern Mtx43 MATRIX_SCRATCH_PAPER;
}

struct Vtbl {
  virtual void v0();
  virtual void v1();
  virtual void v2();
  virtual void v3();   // offset 0xc
};

struct Sub { char pad[0x120]; Mtx43 m; };

extern "C" void func_ov006_020c1f4c(char* c){
  Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, *(int*)(c+0x8c), *(int*)(c+0x90), *(int*)(c+0x94));
  Matrix4x3_ApplyInPlaceToRotationY(&MATRIX_SCRATCH_PAPER, *(s16*)(c+0x9a));
  *(Mtx43*)(c+0x2c) = MATRIX_SCRATCH_PAPER;
  ((Vtbl*)(c+0x10))->v3();
  MATRIX_SCRATCH_PAPER = (*(Sub**)(c+0x24))->m;
  Matrix4x3_ApplyInPlaceToRotationX(&MATRIX_SCRATCH_PAPER, *(s16*)(c+0xa4));
  Matrix4x3_ApplyInPlaceToRotationZ(&MATRIX_SCRATCH_PAPER, *(s16*)(c+0xa2));
  (*(Sub**)(c+0x24))->m = MATRIX_SCRATCH_PAPER;
}
