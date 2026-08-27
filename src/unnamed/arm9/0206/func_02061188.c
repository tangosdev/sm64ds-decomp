#include "types.h"
typedef void (*Handler)(void *msg);

extern void _ZN4CP1519InvalidateDataCacheEjj(u32 addr, u32 len);
extern void Crash(void);
extern void MultiCopyHalf(const void *src, void *dst, u32 count);
extern int func_02061674(void);
extern void func_0206116c(void);

extern u32 *data_020a89ac;

typedef struct {
    u16 w0;      /* 0x00 */
    u16 w1;      /* 0x02 */
    u16 w2;      /* 0x04 */
    u16 w6;      /* 0x06 */
    u32 d8;      /* 0x08 */
    u32 dc;      /* 0x0c */
    u16 w10;     /* 0x10 */
    u16 w12;     /* 0x12 */
    char _14[6]; /* 0x14 */
    u16 w1a;     /* 0x1a */
    u32 d1c;     /* 0x1c */
} BufT;

extern BufT data_020a89ec;
extern char data_020a8a00;

void func_02061188(int unused, u16 *msg, int flag)
{
    u16 op;

    if (flag != 0)
        return;

    _ZN4CP1519InvalidateDataCacheEjj(data_020a89ac[4], 0x100);
    _ZN4CP1519InvalidateDataCacheEjj(data_020a89ac[1], 0xa00);

    op = msg[0];
    if (op >= 0x25) {
        if (op == 0x80) {
            if (msg[1] == 0x13)
                Crash();
            if ((Handler)data_020a89ac[0x2a] != 0)
                ((Handler)data_020a89ac[0x2a])(msg);
        } else if (op == 0x82) {
            if ((data_020a89ac + msg[3])[0x2b] != 0) {
                *(u32 *)((char *)msg + 0x1c) = (data_020a89ac + msg[3])[0x3b];
                _ZN4CP1519InvalidateDataCacheEjj(*(u32 *)(msg + 4),
                    *(u16 *)((char *)data_020a89ac[1] + 0x46));
                ((Handler)(data_020a89ac + msg[3])[0x2b])(msg);
            }
        } else if (op == 0x81) {
            msg[0] = 0xf;
            if (*(Handler *)((char *)msg + 0x14) != 0)
                (*(Handler *)((char *)msg + 0x14))(msg);
        }
    } else {
        if (op == 0xe) {
            if ((u16)(msg[2] + 0xfff5) <= 1 && msg[1] == 0) {
                _ZN4CP1519InvalidateDataCacheEjj(*(u32 *)(msg + 4),
                    *(u16 *)((char *)data_020a89ac[1] + 0x46));
            }
        }

        {
            u16 t = msg[0];
            if ((Handler)(data_020a89ac + t)[5] != 0)
                ((Handler)(data_020a89ac + t)[5])(msg);
        }

        {
            u16 op2 = msg[0];
            if (op2 == 8 || op2 == 0xc) {
                u16 objType;
                u16 v5;
                void *p;

                if (op2 == 8) {
                    p = &msg[5];
                    objType = msg[4];
                    v5 = msg[8];
                } else if (op2 == 0xc) {
                    objType = msg[4];
                    v5 = msg[8];
                    p = &msg[5];
                }

                if (objType == 7 || objType == 9) {
                    u16 i;
                    data_020a89ec.w0 = 0x82;
                    data_020a89ec.w1 = 0;
                    data_020a89ec.w2 = objType;
                    data_020a89ec.d8 = 0;
                    data_020a89ec.dc = 0;
                    data_020a89ec.w10 = 0;
                    data_020a89ec.w12 = v5;
                    data_020a89ec.w1a = 0xffff;
                    MultiCopyHalf(p, &data_020a8a00, 6);
                    for (i = 0; i < 0x10; i++) {
                        data_020a89ec.w6 = i;
                        if ((data_020a89ac + i)[0x2b] != 0) {
                            data_020a89ec.d1c = (data_020a89ac + i)[0x3b];
                            ((Handler)(data_020a89ac + i)[0x2b])(&data_020a89ec);
                        }
                    }
                }
            }
        }

        if (msg[0] == 2 && msg[1] == 0) {
            func_02061674();
            return;
        }
    }

    _ZN4CP1519InvalidateDataCacheEjj(data_020a89ac[4], 0x100);
    func_0206116c();
}
