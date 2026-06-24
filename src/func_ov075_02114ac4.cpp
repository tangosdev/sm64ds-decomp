//cpp
namespace cstd { int fdiv(int a, int b); }
extern "C" void Math_Function_0203b0fc(int *p, int target, int scale, int max);
extern "C" void Math_Function_0203b14c(int *p, int a, int b, int c, int d);

extern "C" int func_ov075_02114ac4(char *c, char *p5, char *p4)
{
    int s = *(int *)(c + 0x110);
    if (s == 5) {
        int q = cstd::fdiv(*(int *)(c + 0x120), 0x19000);
        *(int *)(p4 + 8) = q + 0x50000;
        return 1;
    }
    if (s != 6)
        return 0;
    Math_Function_0203b0fc((int *)(p4 + 4), 0x2bc00, 0x66, 0x1c00);
    Math_Function_0203b14c((int *)(p4 + 8), *(int *)(p5 + 8), 1, 0x4000, 0x100);
    *(int *)(p5 + 4) = *(int *)(p4 + 4);
    return 1;
}
