//cpp
extern int ApproachLinear(int &, int, int);

extern "C" void func_ov004_020b5288(char *c)
{
    ApproachLinear(*(int*)(c + 8), *(int*)(c + 0x10), *(int*)(c + 0x18));
    ApproachLinear(*(int*)(c + 0xc), *(int*)(c + 0x14), *(int*)(c + 0x1c));
}
