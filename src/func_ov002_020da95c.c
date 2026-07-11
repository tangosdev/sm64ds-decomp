#define ADDR(p) ((int *)(((long long)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL))

#pragma opt_propagation off
int func_ov002_020da95c(char *c)
{
    int p = *(int *)(c + 0x358);
    int t = (p != 0);
    if (t == 0) return 0;
    *ADDR(p + 0xb0) &= ~0x800;
    *ADDR(*(volatile int *)(c + 0x358) + 0xb0) &= ~0x4000;
    *ADDR(*(volatile int *)(c + 0x358) + 0xb0) |= 0x1000;
    *ADDR(*(volatile int *)(c + 0x358) + 0xb0) &= ~0x100;
    *(int *)(c + 0x358) = 0;
    return 1;
}
