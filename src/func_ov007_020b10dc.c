/* func_ov007_020b10dc at 0x020b10dc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov007).
 */
extern void func_ov007_020bdeb0(int x);
extern void func_ov007_020bfacc(int a, int b, void* p);
extern void func_ov007_020b1fa4(int self);
extern void func_ov007_020b2764(int x);
extern char data_ov007_0210342c[];
extern char func_ov007_020b16f0[];

void func_ov007_020b10dc(void) {
    char* g = *(char**)data_ov007_0210342c;
    int t = *(int*)(*(char**)(g + 8) + 0xc);
    if (t == 0) {
        (*(void(**)(int))(g + 0x2c))(0x11);
    }
    switch (*(int*)(*(char**)data_ov007_0210342c + 0xec)) {
    case 0:
        if (t == 0x1e) func_ov007_020bdeb0(0x2b);
        else if (t == 0x80) func_ov007_020bdeb0(0x2d);
        break;
    case 1:
        if (t == 0x1e) func_ov007_020bdeb0(0x2c);
        else if (t == 0x7a) func_ov007_020bdeb0(0x2e);
        break;
    }
    if (t == 0x3c) {
        func_ov007_020bfacc(5, 3, func_ov007_020b16f0);
        func_ov007_020b1fa4(1);
        return;
    }
    if (t == 0x3d) {
        func_ov007_020b2764(0x1000);
    }
}
