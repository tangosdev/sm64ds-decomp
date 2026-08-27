#include "types.h"
extern void MultiStore_Int(int val, void *dst, int len);
extern void func_02031b84(int c);

extern u8 data_0209fc7c;
extern u8 data_0209fc90;
extern u8 data_0209fccc;
extern int data_0209fd1c;
extern u8 data_0209fcd8;
extern u8 data_0209fc88;
extern int data_0209fd5c;
extern u8 *data_0209fd0c;
extern u8 data_0209fcc4;
extern u8 data_0209fcc8;
extern u8 data_0209fc70;
extern u8 data_0209fcbc;
extern u8 data_0209fca4;
extern u8 data_0209fcd0;
extern u8 *data_0209fd00;
extern void (*data_02092830[])(void);
extern int data_0209fd18;

void func_02031e00(void)
{
    int flag;
    u8 *p;
    u8 op;
    u8 m;

    data_0209fccc = data_0209fc7c & 7;
    flag = 0;
    if (data_0209fc90 == 0) {
        volatile int zero = 0;
        data_0209fd1c = 0;
        data_0209fcd8 = 0;
        data_0209fc88 = 0;
        MultiStore_Int(zero, &data_0209fd5c, 0xf00);
    }

    for (;;) {
        p = data_0209fd0c;
        op = *p;
        switch (op) {
        case 0xff:
            if (flag != 0) {
                if (data_0209fcc4 == 0) {
                    data_0209fcc8 = 2;
                } else {
                    data_0209fcc8 = 3;
                    data_0209fc70 = 0;
                }
                data_0209fcbc++;
                return;
            } else {
                if (data_0209fcc4 != 0) {
                    u8 *s = data_0209fd00;
                    u8 cc = data_0209fcbc;
                    if (cc < s[6]) {
                        data_0209fcbc = cc + 1;
                        data_0209fc70 = 0;
                        data_0209fcc8 = 3;
                        flag = 0;
                        return;
                    }
                    m = data_0209fca4;
                    if (m == 1) { data_0209fcc8 = 4; data_0209fcd0 = 0; }
                    else if (m == 2) { data_0209fcc8 = 6; data_0209fcd0 = 0; }
                    else if (m == 3) { data_0209fcc8 = 5; data_0209fcd0 = 0; }
                    else { data_0209fcc8 = 8; }
                    return;
                } else {
                    m = data_0209fca4;
                    if (m == 1) { data_0209fcc8 = 4; data_0209fcd0 = 0; }
                    else if (m == 2) { data_0209fcc8 = 6; data_0209fcd0 = 0; }
                    else if (m == 3) { data_0209fcc8 = 5; data_0209fcd0 = 0; }
                    else { data_0209fcc8 = 8; }
                    return;
                }
            }
        case 0xfe: {
            u8 idx = p[2];
            data_0209fd18 = idx;
            (*(void (**)(void))((char *)data_02092830 + (idx << 2) - 4))();
            p = data_0209fd0c;
            data_0209fd0c = p + p[1];
            break;
        }
        case 0xfd:
            if (data_0209fcc4 != 0) {
                u8 *s = data_0209fd00;
                u8 cc = data_0209fcbc;
                if (cc != s[6]) {
                    data_0209fcbc = cc + 1;
                    data_0209fcc8 = 3;
                    data_0209fc70 = 0;
                    if (data_0209fcbc != s[6])
                        data_0209fd0c = p + 1;
                } else {
                    data_0209fcc8 = 7;
                }
                return;
            } else {
                u8 *s = data_0209fd00;
                u8 cc = data_0209fcbc;
                if (cc != s[6]) {
                    data_0209fcbc = cc + 1;
                    if (data_0209fcbc != s[6])
                        data_0209fd0c = p + 1;
                    data_0209fcc8 = 2;
                } else {
                    data_0209fcc8 = 7;
                }
                return;
            }
        default:
            func_02031b84(op);
            data_0209fd0c++;
            flag = 1;
            break;
        }
    }
}
