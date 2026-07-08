//cpp
extern "C" {
struct Vector3 { int x,y,z; };
struct Matrix4x3 { int m[12]; };
void InvMat4x3(struct Matrix4x3* d, struct Matrix4x3* s);
void MulVec3Mat4x3(struct Vector3* v, struct Matrix4x3* m, struct Vector3* out);
void Quaternion_FromVector3(void* q, struct Vector3* axis, struct Vector3* v);
void Quaternion_Normalize(void* q);
extern int data_02092768[];
extern struct Matrix4x3 MATRIX_SCRATCH_PAPER;

void func_ov021_02112024(char* c, char* a) {
  *(char*)(c+0xc7e) = 1;
  *(short*)(c+0xc74) = 0x12c;
  if (*(unsigned char*)(c+0xc7d)) {
    *(int*)(c+0xc5c) = data_02092768[0];
    *(int*)(c+0xc60) = data_02092768[1];
    *(int*)(c+0xc64) = data_02092768[2];
    *(int*)(c+0xc68) = data_02092768[3];
    return;
  }
  MATRIX_SCRATCH_PAPER = *(struct Matrix4x3*)(c+0x2ec);
  InvMat4x3(&MATRIX_SCRATCH_PAPER, &MATRIX_SCRATCH_PAPER);
  struct Vector3 t;
  MulVec3Mat4x3((struct Vector3*)(a+0x5c), &MATRIX_SCRATCH_PAPER, &t);
  t.y = t.y * 0x30;
  struct Vector3 axis;
  axis.x = 0;
  axis.y = 0x1000;
  axis.z = 0;
  Quaternion_FromVector3(c+0xc5c, &axis, &t);
  Quaternion_Normalize(c+0xc5c);
  *(char*)(c+0xc7c) = 0xa;
}
}
