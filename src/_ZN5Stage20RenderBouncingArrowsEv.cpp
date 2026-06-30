//cpp
// _ZN5Stage20RenderBouncingArrowsEv at 0x02023be0
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
extern "C" {
extern int data_0208ee44;
extern int data_020a0db0;
extern unsigned char data_0209f2c4;
extern unsigned char data_0209f284;
extern unsigned char data_0209f2d8;
extern unsigned char data_0209f248;
extern void func_020abd88(void);
int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int, void*, int, int, int, int, int, int, int, int);

void _ZN5Stage20RenderBouncingArrowsEv(void) {
    int r4;
    unsigned char A;
    if (data_0208ee44 == 1) {
        r4 = (data_020a0db0 & 0x10) ? 0xae : 0xb0;
    } else {
        r4 = (data_020a0db0 & 8) ? 0xae : 0xb0;
    }
    A = data_0209f2c4;
    if (A == 0 && data_0209f284 != 0) {
        int t = (data_0209f2d8 == 1);
        if (t == 0) goto draw1;
    }
    if (A == 0) goto draw2;
    {
        unsigned char d = data_0209f248;
        if ((unsigned char)(d + 0xf7) > 2u) goto draw2;
    }
draw1:
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)func_020abd88, 0x40, r4, -1, -1, 0x1000, 0x1000, 0, -1);
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)func_020abd88, 0x80, r4, -1, -1, 0x1000, 0x1000, 0, -1);
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)func_020abd88, 0xc0, r4, -1, -1, 0x1000, 0x1000, 0, -1);
    return;
draw2:
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)func_020abd88, 0xc, r4, -1, -1, 0x1000, 0x1000, 0, -1);
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)func_020abd88, 0xf4, r4, -1, -1, 0x1000, 0x1000, 0, -1);
    return;
}
}