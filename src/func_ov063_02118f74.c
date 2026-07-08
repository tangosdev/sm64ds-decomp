struct Vector3;

extern void func_ov063_02119e38(char *thiz, int a1, int a2, int a3);
extern int func_ov063_0211a0dc(char *c);
extern unsigned char func_ov063_021163d0(char *c);
extern int func_ov063_0211adb4(char *c);
extern void func_0201267c(unsigned int id, const struct Vector3 *v);

void func_ov063_02118f74(char *c) {
    int a2;
    int a3;
    int r5v;
    unsigned char mode;

    *(unsigned short *)(((long long)(int)(c + 0x5d4)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x40;
    mode = *(unsigned char *)(c + 0x5ca);
    if (mode == 3) {
        a2 = 0x180;
        a3 = 0x999;
    } else if (mode == 2) {
        a2 = 0x240;
        a3 = 0x1000;
    } else {
        a2 = 0x380;
        a3 = 0x1800;
    }
    if (*(unsigned short *)(c + 0x5c0) != 0) {
        a2 = 0;
        a3 = 0;
    }
    func_ov063_02119e38(c, -0x64, a2, a3);
    r5v = func_ov063_0211a0dc(c);
    if (*(unsigned char *)(c + 0x5cf) == 0xd) {
        if (func_ov063_021163d0(c) == 0) {
            *(unsigned char *)(c + 0x5cc) = 0;
        }
    } else {
        if (func_ov063_0211adb4(c) != 0) {
            *(unsigned char *)(c + 0x5cc) = 0;
        }
    }
    if (r5v == -1) {
        *(unsigned char *)(c + 0x5cc) = 2;
        return;
    }
    if (r5v != 1) {
        return;
    }
    *(unsigned char *)(c + 0x5cc) = 3;
    func_0201267c(0xc7, (const struct Vector3 *)(c + 0x74));
}
