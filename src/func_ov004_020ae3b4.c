#include "types.h"
extern int* func_02054efc(void);
extern int* func_02054ea8(void);
extern int* _ZN2G213GetBG2CharPtrEv(void);
extern int* func_02054d88(void);
extern int* _ZN3G2S13GetBG0CharPtrEv(void);
extern int* _ZN3G2S13GetBG1CharPtrEv(void);
extern int* _ZN3G2S13GetBG2CharPtrEv(void);
extern int* _ZN3G2S13GetBG3CharPtrEv(void);
extern void MultiCopy_Int(void* a, void* b, int n);

extern int data_ov004_020beb64;
extern int data_ov004_020beb6c;

void func_ov004_020ae3b4(char* obj, int cx, int cy, int val, int n)
{
    int oi;
    int x0;
    int ci0;
    int m;
    int xx;
    int yy;
    int ci;
    int yrow;
    int four;
    int half;

    half = n / 2;
    oi = 0;
    if (n <= 0)
        return;
    x0 = cx - half;
    ci0 = oi;
    m = 0xf;
    yrow = cy - half;
    four = 4;
    do {
        ci = ci0;
        if (n > 0) {
            do {
                yy = yrow;
                xx = ci + x0;
                int* base;
                int* cell;
                int sh;

                if (xx < 0)
                    goto inc;
                if (xx >= 0x100)
                    goto inc;
                if (yrow < -0xc0 - data_ov004_020beb6c)
                    goto inc;
                if (yrow < -data_ov004_020beb6c) {
                    if (*(u8*)(obj + 0x68) == 0)
                        goto inc;
                    switch (*(int*)(obj + 0x6c)) {
                    case 0: base = func_02054efc(); break;
                    case 1: base = func_02054ea8(); break;
                    case 2: base = _ZN2G213GetBG2CharPtrEv(); break;
                    case 3: base = func_02054d88(); break;
                    default: return;
                    }
                    yy += data_ov004_020beb6c + 0xc0;
                } else {
                    if (yrow < 0)
                        goto inc;
                    if (yrow >= 0xc0)
                        goto inc;
                    switch (*(int*)(obj + 0x6c)) {
                    case 0: base = _ZN3G2S13GetBG0CharPtrEv(); break;
                    case 1: base = _ZN3G2S13GetBG1CharPtrEv(); break;
                    case 2: base = _ZN3G2S13GetBG2CharPtrEv(); break;
                    case 3: base = _ZN3G2S13GetBG3CharPtrEv(); break;
                    default: return;
                    }
                }
                cell = (int*)((char*)base + ((xx / 8 + (yy / 8) * 32) * 32) + ((yy & 7) * 4));
                MultiCopy_Int(cell, &data_ov004_020beb64, four);
                sh = (xx & 7) << 2;
                data_ov004_020beb64 = (data_ov004_020beb64 & (-1 ^ (m << sh))) | (val << sh);
                MultiCopy_Int(&data_ov004_020beb64, cell, four);
            inc:;
            } while (++ci < n);
        }
        yrow += 1;
        oi += 1;
    } while (oi < n);
}


