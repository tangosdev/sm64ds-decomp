typedef unsigned short u16;
typedef unsigned char u8;
typedef short s16;

extern s16 data_02082214[];
extern void SetSubBg2Offset(int a, int b);
extern void SetSubBg3Offset(int a, int b);
extern void func_ov004_020afdd0(void* a0, int a1, int a2, int a3, int a4);
extern void func_ov004_020af770(void* a0, int a1, int a2, int a3, int a4, int a5, u16 a6);
extern void func_ov006_020c3b2c(void* p);
extern void func_ov006_020c3bf4(void* self);

extern int* data_ov006_0213ab94[];
extern int* data_ov006_0213abe0;

struct S2 { int a, b; };

int func_ov006_0212aacc(char* self)
{
    int i;
    u8 (*barr)[0x20] = (u8 (*)[0x20])self;
    int (*iarr)[8] = (int (*)[8])self;
    u16 (*harr)[0x10] = (u16 (*)[0x10])self;

    *(u16*)(((long long)(int)(self + 0x5ff4)) & 0xFFFFFFFFFFFFFFFFLL) += 0xc0;
    {
        int v = data_02082214[(*(u16*)(self + 0x5ff4) >> 4) << 1];
        int t = v + 0x80;
        int off = (t + (int)((unsigned)(t >> 7) >> 24)) >> 8;
        SetSubBg2Offset(off, off + 8);
    }
    {
        int v = data_02082214[(*(u16*)(self + 0x5ff4) >> 4) << 1];
        int t = 0x80 - v;
        int off = (t + (int)((unsigned)(t >> 7) >> 24)) >> 8;
        SetSubBg3Offset(off, off);
    }
    func_ov004_020afdd0(data_ov006_0213ab94[*(int*)(self + 0x5fec)], 0x80, 0x60, -1, 1);

    for (i = 0; i < 0x16; i++) {
        if (barr[i][0x4f38] != 0) {
            struct S2 loc = *(struct S2*)data_ov006_0213abe0;
            int a4 = 1;
            loc.a = (loc.a & 0xc1fffcff) | 0x100;
            if (*(int*)(self + 0x5fc8) == i)
                a4 = 0;
            func_ov004_020af770(
                &loc,
                iarr[i][0x13cf] >> 12,
                iarr[i][0x13d0] >> 12,
                -1,
                a4,
                0x1000,
                (u16)(s16)(harr[i][0x27aa] + 0x8000));
        }
    }

    func_ov006_020c3b2c(self + 0x4660);
    func_ov006_020c3bf4(self + 0x51f8);
    return 1;
}
