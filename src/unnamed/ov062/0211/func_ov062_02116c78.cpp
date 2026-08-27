//cpp
typedef int Fix12i;
typedef struct Vector3 { int x, y, z; } Vector3;
extern "C" {
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int a, int fx, unsigned int b);
extern short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern int data_ov062_0211ddf8[];
int func_ov062_02116c78(char *c){
  *(short*)(c+0x3e6) = 0x3c;
  *(int*)(c+0x3f0) = 0;
  *(int*)(c+0x35c) = 0x1000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void*)(c+0x300), (void*)data_ov062_0211ddf8[1], 0, 0x1000, 0);
  *(short*)(c+0x3f4) = Vec3_HorzAngle((Vector3*)(c+0x5c), (Vector3*)(c+0x3c0));
  return 1;
}
}
