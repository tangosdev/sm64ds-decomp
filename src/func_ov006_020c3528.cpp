//cpp
extern "C" {
struct Q{int a,b,c,d;};
struct M43{int w[12];};
struct V3{int x,y,z;};
void Matrix4x3_FromQuaternion(void* q, void* m);
void MulVec3Mat3x3(void* a, void* b, void* d);
void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
void MulMat4x3Mat4x3(void* d, void* a, void* b);
extern struct V3 data_ov006_0212c9e4;
extern struct M43 MATRIX_SCRATCH_PAPER;
void func_ov006_020c3528(char* c){
  struct M43 sp;
  Matrix4x3_FromQuaternion((char*)c+0x30, &sp);
  MulVec3Mat3x3(&data_ov006_0212c9e4, &sp, (char*)c+0x24);
  Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, *(int*)c, *(int*)(c+4), *(int*)(c+8));
  MulMat4x3Mat4x3(&sp, &MATRIX_SCRATCH_PAPER, &MATRIX_SCRATCH_PAPER);
  *(struct M43*)((char*)c+0x60) = MATRIX_SCRATCH_PAPER;
}
}
