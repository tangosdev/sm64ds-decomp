//cpp
extern "C" {
struct Vec3 { int x,y,z; };
struct M43 { int w[12]; };
extern void Matrix4x3_FromQuaternion(void* q, struct M43* m);
extern void Vec3_Asr(struct Vec3* d, struct Vec3* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void MulMat4x3Mat4x3(struct M43* a, void* b, void* c);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* m, short a);
extern struct M43 data_020a0e68;
void func_ov002_020b6074(char* c){
  struct M43 q;
  struct Vec3 v;
  Matrix4x3_FromQuaternion(c+0x320, &q);
  Vec3_Asr(&v, (struct Vec3*)(c+0x5c), 3);
  Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
  MulMat4x3Mat4x3(&q, &data_020a0e68, &data_020a0e68);
  Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)(c+0x8e));
  *(struct M43*)(c+0xf0) = data_020a0e68;

}
}
