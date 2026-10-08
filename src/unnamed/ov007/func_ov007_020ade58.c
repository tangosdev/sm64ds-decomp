#include "types.h"
extern char *data_ov007_0210342c[];
extern u8 data_ov007_020ccbc4[];
extern u8 data_ov007_020ccb7d[];
extern u8 data_ov007_020ccb7c[];

extern int func_ov007_020c3df4(int a, int b);
extern char *func_ov007_020aea6c(int a, void *b, void *c);
extern void *func_ov007_020c11ac(int n);
extern void func_ov007_020c10b0(void *a, void *b);
extern void func_ov007_020c0af4(void *c, void *x);
extern void Sprite_SetAnimation(void *c, int r1, int r2, int r3, int sp0);

typedef struct {
    void *state;   /* 0x0: ptr, first u16 is state id */
    void *obj;     /* 0x4 */
} Slot;

#pragma opt_strength_reduction off

void *func_ov007_020ade58(int self)
{
    Slot *s = (Slot *)func_ov007_020c3df4(0, 8);
    char *g = data_ov007_0210342c[0];
    char *r6 = *(char **)(g + 0x74);
    char *r4;
    char *sl;
    int sb;
    int cnt;

    s->state = func_ov007_020aea6c(self, &data_ov007_020ccbc4[self * 0xa], 0);
    r4 = (char *)func_ov007_020c11ac(data_ov007_020ccb7d[self * 8]);
    s->obj = r4;

    switch (*(u16 *)s->state) {
    case 0:
        func_ov007_020c10b0(r4, func_ov007_020c11ac(1));
        func_ov007_020c0af4(*(void **)(r6 + 4), r4);
        **(int **)(r4 + 0x38) = 0xe8000;
        *(u8 *)(r4 + 0x14) = 1;
        break;
    case 1:
        func_ov007_020c0af4(*(void **)(r6 + 4), r4);
        *(u8 *)(r4 + 0x14) = 0;
        break;
    case 2:
        *(u8 *)(r4 + 0x14) = 3;
        break;
    case 3:
    case 4:
    case 5:
        func_ov007_020c0af4(*(void **)(r6 + 4), r4);
        *(u8 *)(r4 + 0x14) = 2;
        break;
    case 7:
        func_ov007_020c0af4(*(void **)r6, r4);
        *(u8 *)(r4 + 0x14) = 1;
        break;
    case 8:
        func_ov007_020c0af4(*(void **)r6, r4);
        *(u8 *)(r4 + 0x14) = 0;
        break;
    }

    cnt = *(u8 *)(r4 + 0x1c);
    sl = *(char **)(r6 + 0x3c);
    sb = 0;
    if (cnt > 0) {
        int zero = sb;
        int one = 1;
        do {
            u16 state = *(u16 *)s->state;
            u8 *entry = &data_ov007_020ccb7c[state * 8];
            int r7 = entry[0];

            if (state == 1 && sb >= 4) {
                r7 = one;
            } else if (state == 0) {
                *(int *)(*(char **)(*(char **)(r4 + 0x38) + 0x18) + sb * 4) =
                    *(int *)(*(int *)(sl + r7 * 4) + 0xc) + ((*(u8 **)(entry + 4))[sb] << 4);
                Sprite_SetAnimation(*(void **)(r4 + 0x38), zero, zero, zero, zero);
            }

            *(int *)(*(char **)(r4 + 0x18) + sb * 4) =
                *(int *)(*(int *)(sl + r7 * 4) + 0xc) + ((*(u8 **)(entry + 4))[sb] << 4);
            sb++;
        } while (sb < *(u8 *)(r4 + 0x1c));
    }

    Sprite_SetAnimation(s->obj, 0, 0, 0, 0);
    return s;
}
