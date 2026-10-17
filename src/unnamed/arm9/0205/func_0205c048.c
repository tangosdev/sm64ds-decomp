extern int func_0205c5e4(void *self, int mode);
extern void func_0205c4e4(void *self, unsigned int value);
extern int func_0205c480(unsigned char *a, unsigned char *b, int n);

typedef struct {
    int a;
    int b;
    int c;
} Triple3;

typedef struct {
    int a;
    int b;
} Pair2;

/* f stays a materialized 0/1: if/return 1 and ?1:0 bodies grow the caller (0x220-0x228) */
static inline int IsNameChar(int c) {
    int f = 0;
    if (c != 0 && c != 0x2f && c != 0x5c)
        f = 1;
    return f;
}

int func_0205c048(unsigned char *self)
{
    int spflag = *(int *)(self + 0x3c);
    unsigned char *sb = *(unsigned char **)(self + 0x38);
    unsigned char c0;
    int r8;
    int r7;

    struct {
        int fieldA;
        int fieldB;
        int fieldC;
        int sepExpect;
        int lenExpect;
        unsigned char name[128];
    } local;

    func_0205c5e4(self, 2);

    for (c0 = *sb; c0 != 0; sb += (*sb != 0) ? 1 : 0, c0 = *sb) {
        r7 = 0;
        while (IsNameChar(r8 = sb[r7]))
            r7 = r7 + 1;

        if (r8 != 0 || spflag != 0)
            r8 = 1;

        if (r7 == 0)
            return 1;

        if (c0 == 0x2e) {
            if (r7 == 1) {
                sb = sb + 1;
                continue;
            }
            if ((r7 == 2) & (sb[1] == 0x2e)) { /* bitwise &: && shrinks it to 0x20c */
                if (*(unsigned short *)(self + 0x20) != 0) {
                    func_0205c4e4(self, *(unsigned int *)(self + 0x28));
                }
                sb = sb + 2;
                continue;
            }
        }

        if (r7 > 0x7f)
            return 1;

        *(int *)(self + 0x2c) = (int)&local;
        *(int *)(self + 0x30) = 0;

        for (;;) {
            if (func_0205c5e4(self, 3) != 0)
                return 1;
            if (r8 != local.sepExpect)
                continue;
            if (r7 != local.lenExpect)
                continue;
            if (func_0205c480(sb, local.name, r7) != 0)
                continue;
            break;
        }

        if (r8 != 0) {
            *(Triple3 *)(self + 0x2c) = *(Triple3 *)&local;
            sb = sb + r7;
            func_0205c5e4(self, 2);
            continue;
        }

        if (spflag != 0)
            return 1;
        {
            *(Pair2 *)(*(int **)(self + 0x40)) = *(Pair2 *)&local;
        }
        return 0;
    }

    if (spflag == 0)
        return 1;
    *(*(Triple3 **)(self + 0x40)) = *(Triple3 *)(self + 0x1c);
    return 0;
}
