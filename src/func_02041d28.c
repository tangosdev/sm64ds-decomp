struct Node;

extern unsigned int _ZN3IRQ7DisableEv(void);
extern int func_020658c0(void *node, int a1, void *a2, int a3, int arg5, unsigned short arg6);
extern void func_02041b24(char *base, char *node);
extern void _ZN3IRQ7RestoreEj(unsigned int saved);

extern int data_020a1fc0;
extern int data_02082158;

struct R6 {
    char p0[0x58];
    int f58;
    int f5c;
    char p60[0x80 - 0x60];
    int f80;
    int f84;
    int f88;
    char p8c[0xc0 - 0x8c];
    int fc0;
};

struct R7 { int f0; int f4; };

void func_02041d28(char *r7, struct R6 *r6)
{
    unsigned int saved = _ZN3IRQ7DisableEv();
    int mask;

    if ((&data_020a1fc0)[3] != 0) {
        int bit;
        int *q = (int *)(((int)r7 + 0x270c));
        mask = r6->f84;
        r6->f84 = 0;
        *q |= mask;
        if (r6->f80 <= 0) {
            unsigned int diff = r6->f5c - r6->f58;
            int a1 = r6->f88;
            func_020658c0((char *)r6 + 4, a1, &data_02082158,
                          (diff <= 0x400) ? (int)&r6->fc0 : 0, diff, 0);
            func_02041b24(r7, (char *)r6);
        }
        for (bit = 0;; bit++) {
            if ((1 << bit) > mask) {
                break;
            }
            if ((1 << bit) & mask) {
                r6->f80++;
                ((struct R7 *)r7)->f4 = r6->f88;
            }
            r7 += 0xc;
        }
    }
    _ZN3IRQ7RestoreEj(saved);
}
