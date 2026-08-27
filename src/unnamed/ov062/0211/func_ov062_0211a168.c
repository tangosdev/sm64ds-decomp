extern void _Z14ApproachLinearRiii(int* x, int target, int step);
extern int _ZNK9Animation13GetFrameCountEv(char* anim);
extern int _ZNK9Animation12WillHitFrameEi(char* anim, int frame);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* self, void* bca, int a, int fix, unsigned int j);
extern int data_ov062_0211e01c[];

void func_ov062_0211a168(char* r4){
  _Z14ApproachLinearRiii((int*)(r4 + 0x98), 0x3000, 0x1000);
  if (_ZNK9Animation12WillHitFrameEi((char*)(r4 + 0x350),
        (unsigned short)(_ZNK9Animation13GetFrameCountEv((char*)(r4 + 0x350)) - 1)) == 0) return;
  *(int*)(r4 + 0x38c) = 4;
  *(int*)(r4 + 0x98) = 0x3000;
  *(unsigned char*)(r4 + 0x390) = 0;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)(r4 + 0x300), (void*)data_ov062_0211e01c[1], 0x40000000, 0x1000, 0);
}
