typedef struct { int f0; void *f4; } AnimData;
extern AnimData data_ov071_02123048;

extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int a, int speed, unsigned int d);
extern void _ZN9Animation8SetFlagsEi(void *self, int flags);
extern void func_0201267c(int a, void *b);

int func_ov071_0212110c(char *self)
{
    short diff = *(short*)(self + 0x8e) - *(short*)(self + 0x20c);
    *(int*)(self + 0x98) = diff;
    if (*(int*)(self + 0x98) > 0)
        *(short*)(self + 0x20e) = 0x2500;
    else
        *(short*)(self + 0x20e) = -0x2500;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0xd4, data_ov071_02123048.f4, 0, 0x1000, 0);
    _ZN9Animation8SetFlagsEi(self + 0x124, 0x40000000);
    *(int*)(self + 0x130) = 0x2800;
    *(int*)(self + 0x12c) = 0;
    *(unsigned char*)(self + 0x214) = 0;
    *(unsigned char*)(self + 0x215) = 0x2e;
    *(short*)(self + 0x210) = 0;
    func_0201267c(0x119, self + 0x74);
    *(int *)(((int)self + 0xb0) & 0xFFFFFFFFFFFFFFFF) &= ~1;
    *(int*)(self + 0x1e8) = 2;
    *(int*)(self + 0x1fc) = 0x500;
    return 1;
}
