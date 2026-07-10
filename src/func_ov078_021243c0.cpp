//cpp
extern "C" {
extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(char* self, void* bca, int a, int b, int fix, unsigned short t);
extern short Vec3_HorzAngle(void* a, void* b);
extern int data_ov078_02126f20[];
int func_ov078_021243c0(char* c){
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x2cc, (void*)data_ov078_02126f20[1], 0, 0, 0x1000, 0);
  *(short*)(c+0x100) = 0xc8;
  *(short*)(c+0x4fa) = Vec3_HorzAngle(c+0x5c, c+0x4d4);
  *(int*)(((long long)(int)(c + 0x500)) & 0xFFFFFFFFFFFFFFFFLL) =
    *(int*)(((long long)(int)(c + 0x500)) & 0xFFFFFFFFFFFFFFFFLL) - 1;
  *(char*)(c+0x499) = 0;
  *(int*)(c+0xa8) = 0x28000;
  *(int*)(c+0x98) = 0x5000;
  if (*(int*)(c+0x500) <= 0) *(int*)(c+0xb0) = 0x10000000;
  *(int*)(c+0x9c) = -0x2000;
  return 1;
}
}
