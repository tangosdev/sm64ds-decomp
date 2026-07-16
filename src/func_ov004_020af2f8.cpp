//cpp
extern "C" {
extern int _ZN2G212GetBG0ScrPtrEv(void);
extern void MultiStore16(int val, int ptr, int n);
extern int _ZN3G2S12GetBG0ScrPtrEv(void);
extern int _ZN2G212GetBG1ScrPtrEv(void);
extern int _ZN3G2S12GetBG1ScrPtrEv(void);
extern int _ZN2G212GetBG2ScrPtrEv(void);
extern int _ZN3G2S12GetBG2ScrPtrEv(void);
extern int func_02054fb0(void);
extern int _ZN3G2S12GetBG3ScrPtrEv(void);
extern int func_02054efc(void);
extern int _ZN3G2S13GetBG0CharPtrEv(void);
extern int func_02054ea8(void);
extern int func_02054e88(void);
extern int _ZN2G213GetBG2CharPtrEv(void);
extern int func_02054de8(void);
extern int func_02054d88(void);
extern int _ZN3G2S13GetBG3CharPtrEv(void);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void* p, unsigned int a, unsigned int b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void* p, unsigned int a, unsigned int b);

extern char data_ov004_020b9468;

void func_ov004_020af2f8(char* sb, char en, int mode, int base)
{
    volatile unsigned short v0, v1, v2, v3, v4, v5, v6, v7;
    volatile unsigned short w0, w1, w2, w3, w4, w5, w6, w7;
    int val;
    int i;
    int off;
    int t;

    *(unsigned char*)(sb + 0x68) = en;
    i = 0;
    off = i;
    *(int*)(sb + 0x6c) = mode;
    val = i;
    val += base << 12;

    for (; i < 0x300; i++, val++, off += 2) {
        switch (*(int*)(sb + 0x6c)) {
        case 0:
            if (*(unsigned char*)(sb + 0x68)) {
                t = _ZN2G212GetBG0ScrPtrEv();
                t = t + off;
                MultiStore16(v0 = val, t, 2);
            }
            t = _ZN3G2S12GetBG0ScrPtrEv();
            t = t + off;
            MultiStore16(v1 = val, t, 2);
            break;
        case 1:
            if (*(unsigned char*)(sb + 0x68)) {
                t = _ZN2G212GetBG1ScrPtrEv();
                t = t + off;
                MultiStore16(v2 = val, t, 2);
            }
            t = _ZN3G2S12GetBG1ScrPtrEv();
            t = t + off;
            MultiStore16(v3 = val, t, 2);
            break;
        case 2:
            if (*(unsigned char*)(sb + 0x68)) {
                t = _ZN2G212GetBG2ScrPtrEv();
                t = t + off;
                MultiStore16(v4 = val, t, 2);
            }
            t = _ZN3G2S12GetBG2ScrPtrEv();
            t = t + off;
            MultiStore16(v5 = val, t, 2);
            break;
        case 3:
            if (*(unsigned char*)(sb + 0x68)) {
                t = func_02054fb0();
                t = t + off;
                MultiStore16(v6 = val, t, 2);
            }
            t = _ZN3G2S12GetBG3ScrPtrEv();
            t = t + off;
            MultiStore16(v7 = val, t, 2);
            break;
        default:
            return;
        }
    }

    switch (*(int*)(sb + 0x6c)) {
    case 0:
        if (*(unsigned char*)(sb + 0x68)) {
            t = func_02054efc();
            w0 = 0;
            MultiStore16(w0, t, 0x6000);
        }
        t = _ZN3G2S13GetBG0CharPtrEv();
        w1 = 0;
        MultiStore16(w1, t, 0x6000);
        break;
    case 1:
        if (*(unsigned char*)(sb + 0x68)) {
            t = func_02054ea8();
            w2 = 0;
            MultiStore16(w2, t, 0x6000);
        }
        t = func_02054e88();
        w3 = 0;
        MultiStore16(w3, t, 0x6000);
        break;
    case 2:
        if (*(unsigned char*)(sb + 0x68)) {
            t = _ZN2G213GetBG2CharPtrEv();
            w4 = 0;
            MultiStore16(w4, t, 0x6000);
        }
        t = func_02054de8();
        w5 = 0;
        MultiStore16(w5, t, 0x6000);
        break;
    case 3:
        if (*(unsigned char*)(sb + 0x68)) {
            t = func_02054d88();
            w6 = 0;
            MultiStore16(w6, t, 0x6000);
        }
        t = _ZN3G2S13GetBG3CharPtrEv();
        w7 = 0;
        MultiStore16(w7, t, 0x6000);
        break;
    default:
        return;
    }

    _ZN2GX10LoadBGPlttEPKvjj(&data_ov004_020b9468, base << 5, 0x20);
    _ZN3GXS10LoadBGPlttEPKvjj(&data_ov004_020b9468, base << 5, 0x20);
}
}
