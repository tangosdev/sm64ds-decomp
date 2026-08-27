extern char *data_ov007_0210342c;
extern int data_ov007_020ccc20[];

extern void func_ov007_020c1d78(int i);
extern void func_ov007_020b0244(void);
extern void func_ov007_020b7d94(int kind);
extern void func_ov007_020b43dc(void *self);
extern int func_ov007_020b79e4(void);
extern int func_ov007_020b4438(void);
extern void func_ov007_020c1d8c(int i);
extern int func_ov007_020b79c8(void);
extern void func_ov007_020aec00(void);
extern void Crash(void);
extern void func_ov007_020bdeb0(void *self);
extern void func_ov007_020b7a44(void);

typedef int (*pfn)(int);

void func_ov007_020af84c(void)
{
    char *b = data_ov007_0210342c;
    char *s2 = *(char **)(b + 8);
    int state = *(short *)s2;
    int r2var = *(int *)(s2 + 0xc);
    unsigned char *base = *(unsigned char **)(*(char **)(b + 0x28)) + 0xb;

    switch (state) {
    case 7:
        if (r2var == 0) {
            func_ov007_020c1d78(0);
            func_ov007_020b0244();
            *(short *)(*(char **)(data_ov007_0210342c + 0xc) + 2) = 0xc;
        }
        switch (*(int *)(s2 + 4)) {
        case 0: {
            int idx = *(int *)(data_ov007_0210342c + 0x58);
            if (idx == -1)
                return;
            if (base[idx] != 0) {
                *(int *)(s2 + 4) = 1;
                return;
            }
            func_ov007_020b7d94(3);
            *(int *)(data_ov007_0210342c + 0x58) = -1;
            *(int *)(s2 + 4) = 2;
            return;
        }
        case 1: {
            int idx = *(int *)(data_ov007_0210342c + 0x58);
            if (idx == -1) {
                *(int *)(s2 + 4) = 0;
                func_ov007_020c1d78(0);
                return;
            }
            {
                int idx2 = *(int *)(data_ov007_0210342c + 0x5c);
                if (idx2 == -1)
                    return;
                if (base[idx2] == 0) {
                    *(short *)(s2 + 2) = 8;
                    return;
                }
                func_ov007_020b43dc((void *)9);
                *(int *)(s2 + 4) = 3;
                return;
            }
        }
        case 2:
            if (func_ov007_020b79e4() == 0)
                *(int *)(s2 + 4) = 0;
            return;
        case 3:
            if (func_ov007_020b4438() == 2) {
                *(short *)(s2 + 2) = 8;
                func_ov007_020c1d8c(0);
                return;
            }
            if (func_ov007_020b4438() != 3)
                return;
            *(int *)(data_ov007_0210342c + 0x5c) = -1;
            *(int *)(s2 + 4) = 0;
            return;
        }
        break;

    case 8:
        if (r2var == 0) {
            func_ov007_020c1d8c(0);
            func_ov007_020b7d94(4);
        }
        switch (*(int *)(s2 + 4)) {
        case 0: {
            int r;
            if (func_ov007_020b79c8() == 0)
                return;
            r = (*(pfn *)(data_ov007_0210342c + 0x2c))(
                ((int (*)[3])data_ov007_020ccc20)
                    [*(int *)(data_ov007_0210342c + 0x58)]
                    [*(int *)(data_ov007_0210342c + 0x5c)]);
            func_ov007_020aec00();
            switch (r) {
            case 0:
                *(int *)(s2 + 4) = 1;
                return;
            case 1:
                *(int *)(s2 + 4) = 2;
                return;
            case 2:
                Crash();
                return;
            }
            return;
        }
        case 1:
        case 2: {
            int bb;
            if (func_ov007_020b79e4() != 0)
                return;
            bb = (*(int *)(s2 + 4) == 1);
            func_ov007_020b7d94(bb ? 5 : *(int *)(data_ov007_0210342c + 0x5c) + 6);
            *(int *)(data_ov007_0210342c + 0x5c) = -1;
            *(int *)(data_ov007_0210342c + 0x58) = *(int *)(data_ov007_0210342c + 0x5c);
            *(int *)(s2 + 4) = bb ? 4 : 3;
            return;
        }
        case 3:
            if (func_ov007_020b79c8() == 0)
                return;
            {
                char *p54 = *(char **)(data_ov007_0210342c + 0x54);
                unsigned short aa = *(unsigned short *)(p54 + 2);
                unsigned short bx = *(unsigned short *)p54;
                if ((bx & ~aa) == 0) {
                    char *p50 = *(char **)(data_ov007_0210342c + 0x50);
                    if (*(unsigned short *)(p50 + 0xc) == 0)
                        return;
                    if (*(unsigned short *)(p50 + 0x14) != 0)
                        return;
                    if (*(unsigned int *)(p50 + 0x24) < 1)
                        return;
                }
                func_ov007_020bdeb0((void *)0xc);
                func_ov007_020b7a44();
                *(int *)(s2 + 4) = 4;
                return;
            }
        case 4:
            if (func_ov007_020b79e4() == 0)
                *(short *)(s2 + 2) = 7;
            return;
        }
        break;
    }
}
