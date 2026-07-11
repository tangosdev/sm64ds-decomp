typedef unsigned int u32;

struct W4 { u32 w[4]; };

extern struct W4 data_ov098_0213c4e0;
extern struct W4 data_ov098_0213c4f0;

void func_ov098_02138818(char *self)
{
    struct W4 arr1;
    struct W4 arr2;
    void *p;
    u32 idx;

    *(int *)(self + 0x5f0) = 0;
    *(int *)(self + 0x5f4) = 0;

    arr1 = data_ov098_0213c4e0;
    arr2 = data_ov098_0213c4f0;

    p = *(void **)(self + 0x5e4);
    idx = 0;
    if (p != 0) {
        idx = *(u32 *)((char *)p + 8);
        if (idx > 4) idx = 4;
    }

    *(u32 *)(self + 0x98) = arr1.w[idx];
    *(u32 *)(self + 0xa8) = arr2.w[idx];

    *(void **)(self + 0x5e8) = *(void **)(self + 0x5e4);
    *(void **)(self + 0x5e4) = 0;

    *(int *)(((long long)(int)(self + 0x57c)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x2000;
}
