typedef int Fix12;
struct Vector3 { int x, y, z; };
extern int Vec3_Dist(const struct Vector3* a, const struct Vector3* b);
extern void func_ov071_021202ec(char* c, int i);
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* f, int a, Fix12 b, unsigned int g);
extern char data_ov071_02122f88[];

int func_ov071_0211ff84(char* c) {
  if (Vec3_Dist((struct Vector3*)(c+0x5c), (struct Vector3*)(c+0x390)) > 0x5dc000) {
    func_ov071_021202ec(c, 5);
    return 1;
  }
  *(int*)(c+0x9c) = -0x2000;
  *(int*)(c+0xa0) = -0x3c000;
  *(int*)(c+0x98) = 0x4000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, *(void**)(data_ov071_02122f88+4), 0, 0x1000, 0);
  *(int*)(c+0x130) = 0x1000;
  *(int*)(c+0x39c) = 2;
  return 1;
}
