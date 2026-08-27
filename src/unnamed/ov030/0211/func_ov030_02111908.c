extern int data_ov030_02115cf0[];
extern int data_ov030_02115cd0[];
extern int data_ov030_02115cf8[];
extern void func_0201267c(int a, char* b);

enum Bool { FALSE, TRUE };

void func_ov030_02111908(char* c)
{
    int r3 = (short)(((unsigned)(*(int*)(c + 0x12c) << 4)) >> 16);
    int v = *(int*)(c + 0x134);
    enum Bool b;

    b = (enum Bool)(v == data_ov030_02115cf0[1]);
    if (b) {
        if (r3 != 7) {
            if (r3 != 0x28) return;
        }
        func_0201267c(0xeb, c + 0x74);
        return;
    }
    b = (enum Bool)(v == data_ov030_02115cd0[1]);
    if (b) {
        if (r3 != 1) return;
        func_0201267c(0xf1, c + 0x74);
        func_0201267c(0xe8, c + 0x74);
        return;
    }
    b = (enum Bool)(v == data_ov030_02115cf8[1]);
    if (b) {
        if (r3 != 8) return;
        func_0201267c(0xe9, c + 0x74);
    }
}
