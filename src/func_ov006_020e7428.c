typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct Oam {
    u32 attr01;
    u16 attr2;
    u16 aff;
} Oam;

extern Oam data_ov006_02141a54[];
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(void *, u32);
extern void func_02056674(const void *, u32, u32);

void func_ov006_020e7428(void)
{
    int i;
    int x;
    int y;
    int k;
    int ty;
    int tx;
    u32 reg;

    reg = *(volatile u32 *)0x4001000;
    *(volatile u32 *)0x4001000 = (reg & 0xffbfff9f) | 0x20;

    k = 0;
    for (i = 0; i < 0x80; i++) {
        data_ov006_02141a54[i].attr01 = 0;
        *(u32 *)&data_ov006_02141a54[(int)(((long long)i) & 0xFFFFFFFFFFFFFFFFLL)].attr2 = 0;
    }

    y = 0;
    ty = 0;
    for (; y < 0xc0; y += 0x40) {
        x = 0;
        tx = 0;
        for (; x < 0x100; x += 0x40) {
            data_ov006_02141a54[k].attr01 = ((y & 0xff) | 0xc0000c00) | ((x & 0x1ff) << 16);
            data_ov006_02141a54[k].attr2 = (u16)(((int)(((long long)tx) & 0xFFFFFFFFFFFFFFFFLL) + ((int)(((long long)ty) & 0xFFFFFFFFFFFFFFFFLL) << 5)) | 0xf000);
            tx += 8;
            k++;
        }
        ty += 8;
    }

    _ZN4CP1527FlushAndInvalidateDataCacheEjj(data_ov006_02141a54, 0x400);
    func_02056674(data_ov006_02141a54, 0, 0x400);
}
