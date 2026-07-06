typedef short s16;
typedef unsigned char u8;

extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];

extern void func_02012718(void *a, int b);
extern void func_ov004_020ad79c(int a, int b);

void func_ov006_020f5f0c(char *self, int idx)
{
    unsigned int e;
    unsigned int i;
    int ok;
    int n;
    int v, w;

    e = *(u8*)(self + 0x5406);
    if (e >= 2) return;

    i = data_020a0e40;
    ok = 0;
    if (data_020a0de8[i * 4] != 0) {
        if (data_020a0de9[i * 4] != 0) {
            ok = 1;
        }
    }
    if (ok == 0) return;

    n = idx * 0x18;
    v = data_020a0dea[data_020a0e40 * 4] - (*(int*)(self + 0x51a8 + n) >> 12);
    w = data_020a0deb[data_020a0e40 * 4] - (*(int*)(self + 0x51ac + n) >> 12);

    if (v < -0x10) return;
    if (v > 0x10) return;
    if (w < -0x16) return;
    if (w > 0x16) return;

    *(u8*)(self + 0x53ee + e) = *(u8*)(self + 0x51b8 + n);
    *(u8*)(self + 0x53f0 + *(u8*)(self + 0x5406)) = (u8)idx;
    {
        u8 *pc = (u8*)((long long)(int)(self + 0x5406) & 0xFFFFFFFFFFFFFFFFLL);
        *pc = *pc + 1;
    }
    *(u8*)(self + 0x51bc + n) = 3;
    func_02012718((void*)0x143, *(int*)(self + 0x51a8 + n));

    if (*(u8*)(self + 0x540c) != 0) return;
    {
        u8 *pd = (u8*)((long long)(int)(self + 0x540c) & 0xFFFFFFFFFFFFFFFFLL);
        *pd = *pd + 1;
    }
    func_ov004_020ad79c(*(int*)(self + 0xa8), *(int*)(self + 0xb4));
}
