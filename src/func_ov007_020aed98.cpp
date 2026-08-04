//cpp
extern "C" {
extern void func_ov007_020b46b0(int a, int b);
extern void func_ov007_020b7a78(void);
extern void func_ov007_020c92d0(int o);
extern void func_ov007_020c934c(int o, int v);
extern void func_ov007_020b066c(void);
extern void func_ov007_020af0e4(void);
extern void func_ov007_020af1f4(void);
extern void func_ov007_020aef84(void);
extern void func_ov007_020af434(void);
extern void func_ov007_020b0340(void);
extern void func_ov007_020c2090(int c);
extern void func_ov007_020b0548(void);
extern void func_ov007_020b0358(void);
extern void func_ov007_020aefb4(void);
extern void func_ov007_020af12c(void);
extern void func_ov007_020aef78(void);
extern void func_ov007_020af390(void);
extern void func_ov007_020b0324(void);
extern void func_ov007_020b44ec(int a);
extern void func_ov007_020add3c(int c);
extern void func_ov007_020ad660(int a);
extern char *data_ov007_0210342c;

#pragma opt_strength_reduction off
void func_ov007_020aed98(void)
{
    char *g = data_ov007_0210342c;
    int r6 = *(int*)(g + 4);
    int r5 = *(int*)(g + 8);
    int i;

    for (i = 0; i < 0x18; i++) {
        char *gg = data_ov007_0210342c;
        func_ov007_020b46b0(*(int*)(gg + i * 4 + 0x114), *(int*)(gg + 0x50));
    }

    func_ov007_020b7a78();
    func_ov007_020c92d0(r6);

    if (*(int*)(r6 + 0xc) == 0) {
        func_ov007_020c934c(r5, 0);
        switch (*(short*)r6) {
        case 0: func_ov007_020b066c(); break;
        case 2: func_ov007_020af0e4(); break;
        case 3: func_ov007_020af1f4(); break;
        case 4: func_ov007_020aef84(); break;
        case 5: func_ov007_020af434(); break;
        case 6: func_ov007_020b0340(); break;
        }
    }

    func_ov007_020c92d0(r5);

    if (*(int*)(r6 + 0xc) == 0 || *(int*)(r5 + 0xc) == 0) {
        char *gg = data_ov007_0210342c;
        func_ov007_020c2090(*(int*)(gg + 0x54));
    }

    switch (*(short*)r6) {
    case 0: func_ov007_020b0548(); break;
    case 1: func_ov007_020b0358(); break;
    case 2: func_ov007_020aefb4(); break;
    case 3: func_ov007_020af12c(); break;
    case 4: func_ov007_020aef78(); break;
    case 5: func_ov007_020af390(); break;
    case 6: func_ov007_020b0324(); break;
    }

    {
        char *gg = data_ov007_0210342c;
        func_ov007_020c92d0(*(int*)(gg + 0xc));
    }

    {
        char *gg = data_ov007_0210342c;
        *(int*)(gg + 0x180) = -1;
    }
    for (i = 0; i < 0x18; i++) {
        char *gg = data_ov007_0210342c;
        func_ov007_020b44ec(*(int*)(gg + i * 4 + 0x114));
    }
    for (i = 0; i < 9; i++) {
        char *gg = data_ov007_0210342c;
        func_ov007_020add3c(*(int*)(gg + i * 4 + 0xa4));
    }
    for (i = 0; i < 8; i++) {
        char *gg = data_ov007_0210342c;
        func_ov007_020ad660(*(int*)(gg + i * 4 + 0xc8));
    }
}
}
