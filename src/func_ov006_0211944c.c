#pragma opt_strength_reduction off
extern int data_ov006_0213ec98[];
extern int data_ov006_0213ed10[];
extern int data_ov006_0213ed88[];
extern int data_ov006_0213ed4c[];
extern int data_ov006_0213ecfc[];
extern int data_ov006_0213ece8[];
extern int data_ov006_0213ecac[];
extern int data_ov006_0213ed9c[];
extern int data_ov006_0213ed38[];
extern int data_ov006_0213ed74[];
extern int data_ov006_0213ed60[];
extern int data_ov006_0213ed24[];

extern void _ZN6Memory16operator_delete2EPv(void *p);
extern void func_0207328c(void *p, int a, int b, void *cb);
extern void func_0203d47c(void);
extern void func_ov004_020b0840(char *c, int arg);

void func_ov006_0211944c(char *c, int mode)
{
    if (mode != 2)
        return;

    for (int i = 0; i < 0xd; i++) {
        int **slot = (int **)(((long long)(int)(c + i * 4 + 0x4688)) & 0xFFFFFFFFFFFFFFFFLL);
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)data_ov006_0213ec98;
                *(int volatile *)p = (int)data_ov006_0213ed10;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *slot = 0;
        }
    }
    *(int *)(c + 0x4000 + 0x668) = 0;

    for (int i = 0; i < 0x19; i++) {
        int **slot = (int **)(((long long)(int)(c + i * 4 + 0x46bc)) & 0xFFFFFFFFFFFFFFFFLL);
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)data_ov006_0213ed88;
                *(int volatile *)p = (int)data_ov006_0213ed10;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *slot = 0;
        }
    }
    *(int *)(c + 0x4000 + 0x670) = 0;

    for (int i = 0; i < 8; i++) {
        int **slot = (int **)(((long long)(int)(c + i * 4 + 0x4720)) & 0xFFFFFFFFFFFFFFFFLL);
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)data_ov006_0213ed4c;
                *(int volatile *)p = (int)data_ov006_0213ed10;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *slot = 0;
        }
    }
    *(int *)(c + 0x4000 + 0x66c) = 0;

    {
        int *p = *(int **)(c + 0x4000 + 0x684);
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)data_ov006_0213ecfc;
                *(int volatile *)p = (int)data_ov006_0213ed10;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *(int *)(c + 0x4000 + 0x684) = 0;
        }
    }

    for (int i = 0; i < 3; i++) {
        int **slot = (int **)(((long long)(int)(c + i * 4 + 0x4740)) & 0xFFFFFFFFFFFFFFFFLL);
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)data_ov006_0213ece8;
                *(int volatile *)p = (int)data_ov006_0213ed10;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *slot = 0;
        }
    }

    for (int i = 0; i < 6; i++) {
        int **slot = (int **)(((long long)(int)(c + i * 4 + 0x474c)) & 0xFFFFFFFFFFFFFFFFLL);
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)data_ov006_0213ecac;
                *(int volatile *)p = (int)data_ov006_0213ed10;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *slot = 0;
        }
    }
    *(int *)(c + 0x4000 + 0x678) = 0;

    for (int i = 0; i < 3; i++) {
        int **slot = (int **)(((long long)(int)(c + i * 4 + 0x4764)) & 0xFFFFFFFFFFFFFFFFLL);
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)data_ov006_0213ed9c;
                *(int volatile *)p = (int)data_ov006_0213ed10;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *slot = 0;
        }
    }
    *(int *)(c + 0x4000 + 0x67c) = 0;

    for (int i = 0; i < 2; i++) {
        int **slot = (int **)(((long long)(int)(c + i * 4 + 0x4770)) & 0xFFFFFFFFFFFFFFFFLL);
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)data_ov006_0213ed38;
                *(int volatile *)p = (int)data_ov006_0213ed10;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *slot = 0;
        }
    }
    *(int *)(c + 0x4000 + 0x680) = 0;

    {
        int *p = *(int **)(c + 0x4000 + 0x778);
        if (p != 0) {
            if (p != 0) {
                *p = (int)data_ov006_0213ed74;
                func_0207328c((char *)p + 0x4c, 3, 8, func_0203d47c);
                func_0207328c((char *)p + 0x34, 3, 8, func_0203d47c);
                *(int volatile *)p = (int)data_ov006_0213ed10;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *(int **)(c + 0x4000 + 0x778) = 0;
        }
    }
    {
        int *p = *(int **)(c + 0x4000 + 0x77c);
        if (p != 0) {
            if (p != 0) {
                *p = (int)data_ov006_0213ed60;
                *(int volatile *)p = (int)data_ov006_0213ed10;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *(int **)(c + 0x4000 + 0x77c) = 0;
        }
    }
    {
        int *p = *(int **)(c + 0x4000 + 0x780);
        if (p != 0) {
            if (p != 0) {
                *p = (int)data_ov006_0213ed24;
                *(int volatile *)p = (int)data_ov006_0213ed10;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *(int **)(c + 0x4000 + 0x780) = 0;
        }
    }

    func_ov004_020b0840(c, mode);
}
