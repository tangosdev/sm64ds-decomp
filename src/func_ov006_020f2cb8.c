typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

int RandomIntInternal(int *seed);
char *_ZN2G212GetBG3ScrPtrEv(void);
void MultiStore16(u16 val, char *dst, int nbytes);

extern int data_0209d4b8;
extern u8 data_ov006_0213cec0[];
extern u16 data_ov006_0213cdec[];

#pragma opt_strength_reduction off

void func_ov006_020f2cb8(char *c)
{
    u8 *pb;
    s32 n;
    s32 i;
    s32 k;
    u16 *dst;
    u16 idx;
    u16 t;
    s32 j;
    s32 off;
    volatile u16 v;

    t = *(u16 *)(c + 0x5172);
    k = 0;
    if (t >= 0x14) {
        k = 0x14;
    } else if (t >= 0xa) {
        k = 0xa;
    }

    *(u8 *)(c + 0x545a) = (data_ov006_0213cec0 + k)[((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 0xa) >> 15];

    t = *(u16 *)(c + 0x5174);
    if (t == 9) {
        *(u8 *)(c + 0x545a) = 1;
    }
    if (t == 0xe || t == 0x10 || t == 0x11 || t == 0x13) {
        if (*(u8 *)(c + 0x545a) == 3) {
            *(u8 *)(c + 0x545a) = (u8)((((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3)) >> 15);
        }
    }

    n = 2;
    for (i = 0, off = 0; i < 8; i++, off += 0x20) {
        pb = (u8 *)(c + 0x5000);
        dst = (u16 *)(_ZN2G212GetBG3ScrPtrEv() + 0x1d8) + off;
        idx = data_ov006_0213cdec[pb[0x45a]];
        idx += off;
        for (j = 0; j < 8; j++) {
            v = ((u16 *)_ZN2G212GetBG3ScrPtrEv())[idx];
            MultiStore16(v, (char *)dst, n);
            idx++;
            dst++;
        }
    }
}
