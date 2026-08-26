extern void func_ov007_020c92d0(void *s);
extern int func_ov007_020aeaec(char *c, int flag, int lo, int hi);
extern void func_ov007_020ae810(char *c);
extern void func_ov007_020aeb34(char *c, int t);

void func_ov007_020ae834(char *c)
{
    char *s = *(char **)(c + 4);
    int cur = *(short *)s;
    int mode;
    int r4;

    if ((cur == 0 && *(short *)(s + 2) == 3)
        || (cur == 1 && *(short *)(s + 2) == 4)) {
        *(short *)(s + 2) = -1;
    } else if (cur == 1 && *(short *)(s + 2) == 7) {
        *(short *)(s + 2) = 3;
    }

    func_ov007_020c92d0(s);
    mode = *(int *)(s + 0xc);
    r4 = *(short *)s;
    if (mode == 0)
        *(int *)(c + 0x18) = 0;

    switch (r4) {
    case 0:
        if (mode == 0)
            *(int *)(c + 0x20) = 1;
        break;
    case 1:
        if (mode == 0) {
            *(int *)(c + 0x10) = 0x1000;
            *(int *)(c + 0x14) = 0;
            *(int *)(c + 0x20) = 0;
            func_ov007_020ae810(c);
        }
        break;
    case 2:
        break;
    case 3:
    case 4:
    case 7: {
        int up = (r4 == 3);
        if (mode == 0)
            *(int *)(c + 0x20) = 1;
        *(int *)(c + 0x14) = func_ov007_020aeaec(c, up, 0, 0xf);
        func_ov007_020aeb34(c, (int)(((long long)*(int *)(c + 0x14)
                                      * *(int *)(c + 0x14)) >> 12));
        if (up) {
            if (*(int *)(c + 0x14) == 0) {
                *(short *)(*(char **)(c + 4) + 2) = 0;
                break;
            }
        }
        if (!up) {
            if (*(int *)(c + 0x14) == 0x1000) {
                if (r4 == 7)
                    *(short *)(*(char **)(c + 4) + 2) = 3;
                else
                    *(short *)(*(char **)(c + 4) + 2) = 1;
            }
        }
        break;
    }
    case 5:
    case 6: {
        int down = (r4 == 5);
        if (mode == 0)
            *(int *)(c + 0x20) = 1;
        if (*(int *)(c + 0x10) == 0)
            *(int *)(c + 0x18) = 0;
        if ((down && *(int *)(c + 0x10) == 0x1000)
            || (!down && *(int *)(c + 0x10) == 0)) {
            *(short *)(*(char **)(c + 4) + 2) = 0;
        } else {
            *(int *)(c + 0x10) = func_ov007_020aeaec(c, !down, 0, 0x14);
        }
        break;
    }
    }

    if (*(int *)(c + 0x18) > 0)
        *(int *)(c + 0x18) -= 1;
}
