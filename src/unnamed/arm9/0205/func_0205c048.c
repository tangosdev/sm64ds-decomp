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

int func_0205c048(unsigned char *self)
{
    int spflag = *(int *)(self + 0x3c);
    unsigned char *sb = *(unsigned char **)(self + 0x38);
    unsigned char c0;
    unsigned char *fp;
    int r8;
    int r7;
    int flag0;

    struct {
        int fieldA;
        int fieldB;
        int fieldC;
        int sepExpect;
        int lenExpect;
        unsigned char name[128];
    } local;

    func_0205c5e4(self, 2);

    c0 = *sb;
    if (c0 == 0)
        goto Lfinal;

    fp = local.name;

    for (;;) {
        r7 = 0;
        goto Lscan;

    Lscan_inc:
        r7 = r7 + 1;

    Lscan:
        r8 = sb[r7];
        flag0 = 0;
        if (r8 == 0)
            goto Lmerge;
        if (r8 == 0x2f)
            goto Lmerge;
        if (r8 != 0x5c)
            flag0 = 1;
    Lmerge:
        if (flag0 != 0)
            goto Lscan_inc;

        if (r8 != 0)
            goto Lsep1;
        if (spflag == 0)
            goto Lskip1;
    Lsep1:
        r8 = 1;
    Lskip1:

        if (r7 == 0)
            return 1;

        if (c0 == 0x2e) {
            if (r7 == 1) {
                sb = sb + 1;
                goto Ladvance;
            }
            {
                unsigned char c1 = sb[1];
                int isLen2 = (r7 == 2);
                int isDot2 = (c1 == 0x2e);
                if ((isLen2 & isDot2) == 0)
                    goto Lfc;
                if (*(unsigned short *)(self + 0x20) != 0) {
                    func_0205c4e4(self, *(unsigned int *)(self + 0x28));
                }
                sb = sb + 2;
                goto Ladvance;
            }
        }

    Lfc:
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
            if (func_0205c480(sb, fp, r7) != 0)
                continue;
            break;
        }

        if (r8 != 0) {
            *(Triple3 *)(self + 0x2c) = *(Triple3 *)&local;
            sb = sb + r7;
            func_0205c5e4(self, 2);
            goto Ladvance;
        }

        if (spflag != 0)
            return 1;
        {
            *(Pair2 *)(*(int **)(self + 0x40)) = *(Pair2 *)&local;
        }
        return 0;

    Ladvance:
        {
            unsigned char cur = *sb;
            int adv = (cur != 0) ? 1 : 0;
            sb = sb + adv;
            c0 = *sb;
        }
        if (c0 != 0)
            continue;
        break;
    }

Lfinal:
    if (spflag == 0)
        return 1;
    *(*(Triple3 **)(self + 0x40)) = *(Triple3 *)(self + 0x1c);
    return 0;
}
