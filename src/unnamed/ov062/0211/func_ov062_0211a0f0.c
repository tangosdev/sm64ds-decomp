extern int _Z14ApproachLinearRiii(int* val, int target, int step);
extern int _ZN9Animation8FinishedEv(void* anim);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* m, void* file, int a, int b, unsigned int u);
extern void* data_ov062_0211e034[2];

void func_ov062_0211a0f0(char* c)
{
    _Z14ApproachLinearRiii((int*)(c + 0x98), 0, 0x4000);
    if (!_ZN9Animation8FinishedEv(c + 0x350)) return;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov062_0211e034[1], 0, 0x1000, 0);
    *(int*)(c + 0x38c) = 5;
    *(unsigned char*)(c + 0x390) = 0;
    *(short*)(c + 0x100) = 0;
}
