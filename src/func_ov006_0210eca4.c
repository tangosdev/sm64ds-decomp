typedef unsigned char u8;
typedef unsigned short u16;

extern char *_ZN3G2S12GetBG3ScrPtrEv(void);
extern void MultiStore16(u16 val, char *dst, int nbytes);

extern int data_ov006_02142c1c[];

void func_ov006_0210eca4(char *c, int idx, int *pos)
{
    int cx;
    int cy;
    int base;
    int i;
    int j;
    int k;
    int layer;
    char *p;
    u16 tile;
    int jj;
    int b0;
    int b1;
    int kk;
    volatile u16 t1;
    volatile u16 t2;
    volatile u16 t3;
    volatile u16 t4;
    volatile u16 t5;

    cx = (pos[0] >> 12) / 8;
    cy = (pos[1] >> 12) / 8;

    j = -1;
    k = -2;
    layer = 2;
    b1 = 0x31a;
    b0 = 0x314;

    for (i = -1; i <= 1; i++) {
        int row = (cy + i) << 6;
        jj = j;
        kk = k;
        do {
            if (idx == 0) {
                tile = (u16)(jj + b0);
            } else {
                tile = (u16)(jj + b1);
            }
            p = _ZN3G2S12GetBG3ScrPtrEv();
            t1 = tile | 0xf000;
            MultiStore16(t1, p + row + cx * 2 + kk, layer);
            kk += 2;
            jj++;
        } while (jj <= 0);
        b0 += 0x20;
        b1 += 0x20;
    }

    if (idx == 0 || *(u8 *)(c + data_ov006_02142c1c[idx - 1] + 0x31) == 1) {
        base = 0x33f;
    } else {
        base = 0x33d;
    }

    {
        int lay2 = 2;
        int i2;
        int bas2 = base;
        for (i2 = -1; i2 <= 1; i2++) {
            p = _ZN3G2S12GetBG3ScrPtrEv() + ((cy + i2) << 6);
            t2 = (bas2 + 0x20 * i2) | 0xf000;
            MultiStore16(t2, p + cx * 2 - 4, lay2);
        }
    }

    if (idx == 8) {
        int lay3 = 2;
        int i3;
        int bas3 = 0x336;
        for (i3 = -1; i3 <= 0; i3++) {
            p = _ZN3G2S12GetBG3ScrPtrEv() + ((cy + i3) << 6);
            t3 = (bas3 + 0x20 * i3) | 0xf000;
            MultiStore16(t3, p + cx * 2 + 2, lay3);
        }
        return;
    }

    if (*(u8 *)(c + data_ov006_02142c1c[idx + 1] + 0x31) == 1) {
        int lay4 = 2;
        int i4;
        int bas4 = 0x33f;
        for (i4 = -1; i4 <= 1; i4++) {
            p = _ZN3G2S12GetBG3ScrPtrEv() + ((cy + i4) << 6);
            t4 = (bas4 + 0x20 * i4) | 0xf000;
            MultiStore16(t4, p + cx * 2 + 2, lay4);
        }
        return;
    }

    {
        int lay5 = 2;
        int i5;
        int bas5 = 0x33e;
        for (i5 = -1; i5 <= 1; i5++) {
            p = _ZN3G2S12GetBG3ScrPtrEv() + ((cy + i5) << 6);
            t5 = (bas5 + 0x20 * i5) | 0xf000;
            MultiStore16(t5, p + cx * 2 + 2, lay5);
        }
    }
}
