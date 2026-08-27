#include "types.h"
extern u8 data_0209d65c;
extern u8 data_0209d6b8;
extern u8 data_0209d678;
extern int data_0209d704;
extern u8 data_0209d6a4;
extern u8 data_0209d6b0;
extern int data_0209d74c;
extern u8* data_0209d6f4;
extern u8 data_0209d69c;
extern u8 data_0209d6bc;
extern u8 data_0209d6a0;
extern u8 data_0209d6b4;
extern u8* data_0209d6f0;
extern u8 data_0209d694;
extern u8 data_0209d68c;
extern int data_0209d700;
extern void (*MSG_GEN_TEXT_FUNCS[])(void);
extern void MultiStore_Int(int val, int* dst, int len);
extern void _ZN7Message7AddCharEc(int c);

void func_0201b7cc(void)
{
    int added = 0;

    data_0209d678 = data_0209d65c & 7;
    if (data_0209d6b8 == 0) {
        volatile int fill = 0;
        data_0209d704 = 0;
        data_0209d6a4 = 0;
        data_0209d6b0 = 0;
        MultiStore_Int(fill, &data_0209d74c, 0xf00);
    }

    while (1) {
        u8* cur;
        u8 c;
        cur = data_0209d6f4;
        c = *cur;
        if (c == 0xfd) goto Lfd;
        if (c == 0xfe) goto Lfe;
        if (c != 0xff) goto Ldefault;

        /* 0xff */
        if (added != 0) {
            if (data_0209d69c == 0) {
                data_0209d6bc = 2;
            } else {
                data_0209d6bc = 3;
                data_0209d6a0 = 0;
            }
            data_0209d6b4 = data_0209d6b4 + 1;
        } else {
            if (data_0209d69c != 0) {
                u8* p = data_0209d6f0;
                u8 v = data_0209d6b4;
                if (v < p[6]) {
                    data_0209d6b4 = v + 1;
                    data_0209d6a0 = 0;
                    data_0209d6bc = 3;
                } else {
                    u8 m = data_0209d694;
                    if (m == 1) { data_0209d6bc = 4; data_0209d68c = 0; }
                    else if (m == 2) { data_0209d6bc = 6; data_0209d68c = 0; }
                    else if (m == 3) { data_0209d6bc = 5; data_0209d68c = 0; }
                    else { data_0209d6bc = 8; }
                }
            } else {
                u8 m = data_0209d694;
                if (m == 1) { data_0209d6bc = 4; data_0209d68c = 0; }
                else if (m == 2) { data_0209d6bc = 6; data_0209d68c = 0; }
                else if (m == 3) { data_0209d6bc = 5; data_0209d68c = 0; }
                else { data_0209d6bc = 8; }
            }
        }
        return;

    Lfe:
        {
            u8 idx = cur[2];
            data_0209d700 = idx;
            (MSG_GEN_TEXT_FUNCS + idx)[-1]();
            data_0209d6f4 = data_0209d6f4 + data_0209d6f4[1];
        }
        continue;

    Lfd:
        if (data_0209d69c != 0) {
            u8* p = data_0209d6f0;
            u8 v = data_0209d6b4;
            if (v == p[6]) goto Lset7a;
            data_0209d6b4 = v + 1;
            data_0209d6bc = 3;
            data_0209d6a0 = 0;
            if (data_0209d6b4 != p[6]) {
                data_0209d6f4 = cur + 1;
            }
            return;
        Lset7a:
            data_0209d6bc = 7;
            return;
        } else {
            u8* p = data_0209d6f0;
            u8 v = data_0209d6b4;
            if (v == p[6]) goto Lset7b;
            data_0209d6b4 = v + 1;
            if (data_0209d6b4 != p[6]) {
                data_0209d6f4 = cur + 1;
            }
            data_0209d6bc = 2;
            return;
        Lset7b:
            data_0209d6bc = 7;
            return;
        }

    Ldefault:
        _ZN7Message7AddCharEc(c);
        added = 1;
        data_0209d6f4 = data_0209d6f4 + 1;
    }
}
