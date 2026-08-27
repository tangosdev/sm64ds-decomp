#include "types.h"
typedef struct {
    u8 status;      /* 0x0 */
    u8 byte1;       /* 0x1 */
    u8 byte2;       /* 0x2 */
    char pad3[0x10 - 0x3];
    int u10;        /* 0x10 */
    int u14;        /* 0x14 */
} Elem;

typedef struct {
    char pad0[0x14];
    int u14;        /* 0x14 */
    int u18;        /* 0x18 */
    int u1c;        /* 0x1c */
    s16 h20;        /* 0x20 */
} Slot;

typedef struct {
    char pad[0x32];
    u16 h32;        /* 0x32 */
} Node65;

extern int func_02065a74(void);
extern Node65 *func_02065a94(int key);
extern int func_0206491c(unsigned int key, int val);
extern int func_02064888(int a, int b);
extern int func_02064d6c(int a, int b, int c, int d, int e, int f, int g);
extern int func_02065bc0(void);
extern void func_02064554(void *base, int a, int b, int c);

int func_02063bc4(void *base, int idx, int r8) {
    int r6 = 0;
    Slot *s = (Slot*)((char*)base + 0x138);
    Elem *e = (Elem*)((char*)base + 0x1d4 + idx * 0x68);
    if (e->status == 2) {
        int r7;
        if (func_02065a74() == 0) r7 = 0xfffe; else r7 = 1;
        if (e->byte1 == 4) {
            Node65 *node = func_02065a94(e->u10);
            if (node != 0) {
                int v = func_0206491c(e->u10, e->u14);
                if (node->h32 == 0) {
                    int r = func_02064888(e->u10, v);
                    if (r == 0) {
                        r6 = func_02064d6c(r8, r7, e->byte2, e->u10, e->u14, 0, 0);
                    } else {
                        r6 = func_02064d6c(r8, r7, e->byte2, e->u10, v, r, func_02065bc0());
                    }
                } else if (node->h32 == 1) {
                    s->u14 = e->u10;
                    s->u18 = v;
                    s->h20 = func_02065bc0();
                    s->u1c = 0;
                    func_02064554(base, 0xd, idx, 0);
                    if (s->u1c != 0) {
                        r6 = func_02064d6c(r8, r7, e->byte2, s->u14, s->u18, s->u1c, s->h20);
                    }
                }
            }
            e->status = 1;
        }
    }
    return r6;
}
