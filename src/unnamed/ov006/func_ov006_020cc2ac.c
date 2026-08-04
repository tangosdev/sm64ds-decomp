typedef short s16;
extern void func_ov006_020c8c78(int a, int b);
extern void _Z14ApproachLinearRiii(int *ref, int target, int step);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int x);
extern void func_ov006_020ccd78(char *c);
extern int data_ov006_0214058c;
extern int data_ov006_0213b0ec;

void func_ov006_020cc2ac(char *c)
{
    s16 *p = (s16 *)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFFULL);
    *p = *p - 1;
    if (*(s16 *)(c + 0x60) == 0) {
        func_ov006_020c8c78(*(s16 *)(c + 0x4e), 0xc0);
        data_ov006_0214058c -= 1;
        _Z14ApproachLinearRiii(&data_ov006_0213b0ec, 0, 1);
        _ZN5Sound12PlayBank2_2DEj(0x130);
        func_ov006_020ccd78(c);
        return;
    }
    *(int *)(c + 0x34) = (int)(((long long)*(int *)(c + 0x34) * 0xc00 + 0x800) >> 12);
    {
        int v = *(int *)(c + 0x1c);
        if (v < -0x6c000) {
            *(int *)(c + 0x1c) = -0x6c000;
            return;
        }
        if (v > 0x6c000)
            *(int *)(c + 0x1c) = 0x6c000;
    }
}
