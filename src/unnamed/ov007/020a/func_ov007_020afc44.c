extern int func_ov007_020c8f98(void* p);
extern void func_ov007_020c1d78(int i);
extern void func_ov007_020b0244(void);
extern void func_ov007_020b7d94(int kind);
extern int func_ov007_020b79e4(void);
extern void func_ov007_020c1d8c(int i);
extern void RequestScreenFade(int a, int b, int c);
extern void func_ov007_020b8188(int a, int b, int c);
extern void func_ov007_020c0638(int a, int b, int c, int d);
extern void func_ov007_020b8070(int n);
extern char* data_ov007_0210342c;

#define G data_ov007_0210342c

void func_ov007_020afc44(void)
{
    char* r8 = *(char**)(G + 8);
    unsigned char* r7 = (unsigned char*)(*(int*)(*(char**)(G + 0x28))) + 0xb;
    int r6 = *(int*)(r8 + 0xc);
    char* r5 = *(char**)(G + 0x6c);
    char* r4 = *(char**)(G + 0x70);
    int v0 = func_ov007_020c8f98(r5);
    int v1 = func_ov007_020c8f98(r4);
    int sb = (v0 == 0);
    int fp = (v1 == 0);
    int st = *(short*)r8;

    switch (st) {
    case 3:
        if (r6 == 0) {
            func_ov007_020c1d78(0);
            func_ov007_020b0244();
            *(short*)(*(char**)(G + 0xc) + 2) = 0xa;
        }
        switch (*(int*)(r8 + 4)) {
        case 0: {
            int idx = *(int*)(G + 0x58);
            if (idx != -1) {
                if (r7[idx] != 0) {
                    *(short*)(r8 + 2) = 4;
                } else {
                    *(int*)(G + 0x58) = -1;
                    func_ov007_020b7d94(3);
                    *(int*)(r8 + 4) = 1;
                }
            }
            break;
        }
        case 1:
            if (func_ov007_020b79e4() == 0) {
                *(int*)(r8 + 4) = 0;
            }
            break;
        }
        break;
    case 4:
        if (r6 == 0) {
            func_ov007_020c1d8c(0);
            RequestScreenFade(8, 0xa, 0);
            *(short*)(*(char**)(G + 0xc) + 2) = 0xb;
            func_ov007_020b8188(*(int*)(G + 0x58), *(int*)(G + 0x28), *(int*)(G + 0x60));
            *(int*)(r5) = 1;
            *(int*)(r5 + 4) = 1;
            *(short*)(r5 + 0xc) = 0;
            *(int*)(r5 + 8) = 0;
            *(int*)(r5 + 0x14) = 0;
            *(int*)(*(int*)(r5 + 0x10)) = *(int*)(r5 + 0x14);
            *(int*)(r5 + 0x18) = 0;
            *(int*)(r4) = 1;
            *(int*)(r4 + 4) = 1;
            *(short*)(r4 + 0xc) = 0;
            *(int*)(r4 + 8) = 0x2000;
            *(int*)(r4 + 0x14) = 0x30000;
            *(int*)(*(int*)(r4 + 0x10)) = *(int*)(r4 + 0x14);
            *(int*)(r4 + 0x18) = 0x1000;
        } else if (v1 != 0) {
            *(short*)(r8 + 2) = 2;
        }
        break;
    case 2:
        if (r6 == 0) {
            func_ov007_020c1d78(0);
        }
        {
            int prev = *(int*)(G + 0x5c);
            if (prev != -1) {
                if (prev != *(int*)(G + 0x58)) {
                    *(short*)(r8 + 2) = 5;
                }
            }
        }
        break;
    case 5: {
        int sl = 0x20000;
        int prev = *(int*)(G + 0x5c);
        int sp4 = -0x20000;
        if (prev < *(int*)(G + 0x58)) {
            sp4 = 0x20000;
            sl = -0x20000;
        }
        if (r6 == 0) {
            func_ov007_020c1d8c(0);
            *(int*)(r5) = 1;
            *(int*)(r5 + 4) = 1;
            *(short*)(r5 + 0xc) = 0;
            *(int*)(r5 + 8) = 0x3000;
            *(int*)(r5 + 0x14) = 0;
            *(int*)(*(int*)(r5 + 0x10)) = *(int*)(r5 + 0x14);
            *(int*)(r5 + 0x18) = sp4;
        } else if (v0 != 0) {
            switch (*(int*)(r8 + 4)) {
            case 1:
                *(int*)(G + 0x58) = prev;
                *(int*)(G + 0x5c) = -1;
                *(short*)(r8 + 2) = 2;
                break;
            case 0:
            case 2:
                if (r7[prev] != 0) {
                    if (func_ov007_020b79e4() == 0) {
                        func_ov007_020c1d8c(0);
                        if (*(int*)(r8 + 4) == 2) {
                            RequestScreenFade(8, 0xa, 0);
                            *(short*)(*(char**)(G + 0xc) + 2) = 0xb;
                        }
                        func_ov007_020b8188(*(int*)(G + 0x5c), *(int*)(G + 0x28), *(int*)(G + 0x60));
                        sb = 1;
                        *(int*)(r5) = 1;
                        *(int*)(r5 + 4) = 1;
                        *(short*)(r5 + 0xc) = 0;
                        *(int*)(r5 + 8) = 0x3000;
                        *(int*)(r5 + 0x14) = sl;
                        *(int*)(*(int*)(r5 + 0x10)) = *(int*)(r5 + 0x14);
                        *(int*)(r5 + 0x18) = 0;
                        *(int*)(r4) = 1;
                        *(int*)(r4 + 4) = 1;
                        *(short*)(r4 + 0xc) = 0;
                        *(int*)(r4 + 8) = 0x1000;
                        *(int*)(r4 + 0x14) = 0x1000;
                        *(int*)(*(int*)(r4 + 0x10)) = *(int*)(r4 + 0x14);
                        *(int*)(r4 + 0x18) = 0x1000;
                        *(int*)(r8 + 4) = 1;
                    }
                } else {
                    int t;
                    RequestScreenFade(8, 8, 0);
                    *(short*)(*(char**)(G + 0xc) + 2) = 0xa;
                    t = *(int*)(G + 0x58);
                    *(int*)(G + 0x58) = *(int*)(G + 0x5c);
                    *(int*)(G + 0x5c) = t;
                    func_ov007_020b7d94(3);
                    func_ov007_020c0638(*(int*)(*(char**)(G + 0xe0) + 4), 0, 0, 0);
                    *(int*)(r8 + 4) = 2;
                }
                break;
            }
        }
        break;
    }
    case 6:
        if (r6 == 0) {
            func_ov007_020c1d8c(0);
            if (*(short*)(r8 + 0xa) == 3) {
                *(short*)(r8 + 2) = 0;
            } else {
                RequestScreenFade(8, 8, 0);
                *(short*)(*(char**)(G + 0xc) + 2) = 0xa;
                *(int*)(r4) = 1;
                *(int*)(r4 + 4) = 1;
                *(short*)(r4 + 0xc) = 0;
                *(int*)(r4 + 8) = 0x2000;
                *(int*)(r4 + 0x14) = 0x1000;
                *(int*)(*(int*)(r4 + 0x10)) = *(int*)(r4 + 0x14);
                *(int*)(r4 + 0x18) = 0x30000;
            }
        } else if (v1 != 0) {
            if (*(short*)(r8 + 0xa) == 2) {
                *(short*)(r8 + 2) = 3;
            } else {
                *(short*)(r8 + 2) = 0;
            }
        }
        break;
    }

    if (*(short*)r8 == 3) return;
    if (func_ov007_020b79e4() != 0) return;
    if (sb != 0) {
        func_ov007_020c0638(*(int*)(*(char**)(G + 0xe0) + 4), (*(int*)(G + 0x64) >> 0xc) * 8, 0, 0);
        func_ov007_020c0638(*(int*)(*(char**)(G + 0xe4) + 4), (*(int*)(G + 0x64) >> 0xc) * 8, 0, 0);
    }
    if (fp == 0) return;
    func_ov007_020b8070(*(int*)(G + 0x68) >> 0xc);
}
