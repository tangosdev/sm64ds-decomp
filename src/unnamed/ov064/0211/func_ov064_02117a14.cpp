//cpp
extern "C" {
typedef struct { int x, y, z; } Vector3;
extern void Vec3_Sub(Vector3* dst, Vector3* a, Vector3* b);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
void func_ov064_02117a14(char* c, Vector3* a, Vector3* b){
  Vector3 v;
  Vec3_Sub(&v, a, b);
  *(short*)(c+0x94)=(short)_ZN4cstd5atan2E5Fix12IiES1_(v.x, v.z);
}
}
