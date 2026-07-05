extern void _Z14ApproachLinearRiii(int *v, int target, int rate);
extern void Math_Function_0203b0fc(int *p, int target, int scale, int max);

int func_02007484(char *self)
{
    _Z14ApproachLinearRiii((int *)(self + 0xb8), 0, 0x2000);

    *(int *)(self + 0xb0) = 0;

    char *info = *(char **)(self + 0x110);

    int *sub = (int *)(((long long)(int)(info + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);

    Math_Function_0203b0fc((int *)(self + 0x8c), sub[0], 0x28, 0x7fffffff);
    Math_Function_0203b0fc((int *)(self + 0x94), sub[2], 0x28, 0x7fffffff);

    return 1;
}
