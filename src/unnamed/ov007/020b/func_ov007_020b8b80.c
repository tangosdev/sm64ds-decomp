#include "types.h"
extern int data_ov007_02103464;
extern u8 data_ov007_02102df8[];
extern int data_ov007_02103468;

extern void func_ov007_020b8d48(int a, int c);

void func_ov007_020b8b80(int a, u8 *s, int mode)
{
    u8 buf[0x100];
    int newline;
    int x0;
    int n;
    int width;
    int i;
    int c;
    int cont;
    int flush;

    newline = 1;
    cont = 1;
    flush = 0;
    x0 = data_ov007_02103464;

    do {
        c = *s;
        if (newline) {
            width = 0;
            n = 0;
            newline = 0;
        }
        switch (c) {
        case 0xFD:
            newline = 1;
            flush = 1;
            break;
        case 0xFF:
            cont = 0;
            flush = 1;
            break;
        default:
            buf[n] = (u8)c;
            n++;
            width += (c == 0x4D) ? 4 : data_ov007_02102df8[c];
            break;
        }
        if (flush) {
            switch (mode) {
            case 1:
                data_ov007_02103464 = x0;
                break;
            case 0:
                data_ov007_02103464 = x0 - width / 2;
                break;
            case 2:
                data_ov007_02103464 = x0 - width;
                break;
            }
            for (i = 0; i < n; i++)
                func_ov007_020b8d48(a, buf[i]);
            flush = 0;
        }
        if (newline) {
            data_ov007_02103464 = x0;
            data_ov007_02103468 += 2;
        }
        s++;
    } while (cont);
}
