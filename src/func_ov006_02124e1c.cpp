//cpp
extern "C" int ApproachLinear(int& v, int a, int b);

extern "C" void func_ov006_02124e1c(char* c)
{
    int a = ApproachLinear(((int*)(c + 0x51ac))[*(int*)(c + 0x51c0) * 2], 0x90000, 0x4000) & 1;
    int b = ApproachLinear(((int*)(c + 0x51ac))[*(int*)(c + 0x51c4) * 2], 0x30000, 0x4000) & 1;

    if (a == 0) return;
    if (b == 0) return;

    *(int*)(c + 0x51bc) = 0x1e;
    (*(int *)(((int)c + 0x51b8) & 0xFFFFFFFFFFFFFFFF)) += 1;
}
