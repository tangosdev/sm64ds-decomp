#include "types.h"
extern char *_ZN8dActor_c10FindWithIDEj(u32 id);
extern int func_ov002_020cc01c(char *c);

#define LA(p) ((int)(p))

int func_ov002_020cbea8(char *c)
{
    char *o;
    int amt;
    int b;
    int ca8;
    int *p;
    int t;

    if (*(u32 *)(c + 0x2f8) == 0) return 0;
    if ((*(int *)(c + 0x2f4) & 0x400000) == 0) return 0;
    o = _ZN8dActor_c10FindWithIDEj(*(u32 *)(c + 0x2f8));
    if (o == 0 || o == c) return 0;
    b = (int)(*(unsigned short *)(o + 0xc) == 0xbf);
    if (b == 0) return 0;
    if (*(int *)(c + 0x37c) == 0 || *(int *)(o + 0x37c) == 0) return 0;
    if (func_ov002_020cc01c(c) != 0 || func_ov002_020cc01c(o) != 0) return 1;
    ca8 = *(int *)(c + 0xa8);
    amt = *(int *)(c + 0x2e4);
    if (ca8 == 0 || *(int *)(o + 0xa8) == 0) amt = amt * 2;
    if (ca8 != 0) {
        p = (int *)LA(c + 0x688);
        *p += amt * 2;
        *(short *)(c + 0x69c) = 0;
        *(int *)(c + 0xa8) = 0;
    }
    if (*(int *)(o + 0xa8) != 0) {
        p = (int *)LA(o + 0x688);
        *p -= amt * 2;
        *(short *)(o + 0x69c) = 0;
        t = *(int *)(o + 0xac);
        *(int *)(o + 0xa8) = 0;
        *(int *)(o + 0xac) = t;
    }
    return 0;
}
