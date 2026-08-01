#include "types.h"
extern int func_02065a74(void);
extern int func_0206491c(int a, int b);
extern int func_02064888(int a, int b);
extern int func_02064c2c(int a, int b, int c, int d, int e, int f, int g);
extern int func_02065bc0(void);

typedef struct {
    u8 status;      /* 0x0 */
    u8 byte1;       /* 0x1 */
    u8 byte2;       /* 0x2 */
    char pad3[0x10 - 0x3];
    int u10;        /* 0x10 */
    int u14;        /* 0x14 */
} Elem;

int func_02063ae4(void *base, int idx, int r6) {
    Elem *e = (Elem*)((char*)base + 0x1d4 + idx * 0x68);
    int r7 = 0;
    if (e->status == 2) {
        int r5;
        if (func_02065a74() == 0) r5 = 0xffff; else r5 = 1;
        if (e->byte1 == 6) {
            int r8 = func_0206491c(e->u10, e->u14);
            r7 = func_02064888(e->u10, r8);
            if (r7 == 0) {
                r7 = func_02064c2c(r6, r5, e->byte2, e->u10, e->u14, 0, 0);
            } else {
                int t = func_02065bc0();
                r7 = func_02064c2c(r6, r5, e->byte2, e->u10, r8, r7, t);
            }
            e->status = 1;
        }
    }
    return r7;
}
