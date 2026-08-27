extern char *data_ov007_0210342c;

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

typedef int (*pfn)(int);

void func_ov007_020af59c(void)
{
    char *b = data_ov007_0210342c;
    char *s2 = *(char **)(b + 8);
    int state = *(short *)s2;
    int r2var = *(int *)(s2 + 0xc);
    unsigned char *base = *(unsigned char **)(*(char **)(b + 0x28)) + 0xb;

    switch (state) {
    case 9:
        if (r2var == 0) {
            func_ov007_020c1d78(0);
            func_ov007_020b0244();
            *(short *)(*(char **)(data_ov007_0210342c + 0xc) + 2) = 0xd;
        }
        switch (*(int *)(s2 + 4)) {
        case 0: {
            int idx = *(int *)(data_ov007_0210342c + 0x58);
            if (idx == -1)
                return;
            if (base[idx] == 0) {
                func_ov007_020b7d94(3);
                *(int *)(data_ov007_0210342c + 0x58) = -1;
                *(int *)(s2 + 4) = 1;
            } else {
                func_ov007_020b43dc((void *)0xc);
                *(int *)(s2 + 4) = 2;
            }
            return;
        }
        case 1:
            if (func_ov007_020b79e4() == 0)
                *(int *)(s2 + 4) = 0;
            return;
        case 2:
            if (func_ov007_020b4438() == 2) {
                *(short *)(s2 + 2) = 0xa;
                func_ov007_020c1d8c(0);
                return;
            }
            if (func_ov007_020b4438() != 3)
                return;
            *(int *)(data_ov007_0210342c + 0x58) = -1;
            *(int *)(s2 + 4) = 0;
            return;
        }
        return;

    case 10:
        if (r2var == 0) {
            func_ov007_020b7d94(0xa);
            func_ov007_020c1d8c(0);
        }
        switch (*(int *)(s2 + 4)) {
        case 0: {
            int r;
            if (func_ov007_020b79c8() == 0)
                return;
            r = (*(pfn *)(data_ov007_0210342c + 0x2c))(*(int *)(data_ov007_0210342c + 0x58) + 8);
            *(int *)(data_ov007_0210342c + 0x58) = -1;
            switch (r) {
            case 0:
                func_ov007_020aec00();
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
        case 2:
            if (func_ov007_020b79e4() != 0)
                return;
            func_ov007_020b7d94(0xb);
            *(int *)(s2 + 4) = 3;
            return;
        case 3:
            if (func_ov007_020b79e4() == 0) {
                *(short *)(s2 + 2) = 0;
                *(short *)(s2 + 2) = 9;
            }
            return;
        }
        return;
    }
}
