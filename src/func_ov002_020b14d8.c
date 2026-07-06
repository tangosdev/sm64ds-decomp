typedef short s16;

enum { false = 0, true = 1 };

struct M12 { int w[12]; };

struct BF3ae {
    unsigned char b0 : 1;
    unsigned char : 4;
    unsigned char sel : 3;
};

extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *c, void *sm, void *mtx, int a, int b, unsigned int u);
extern int data_02082128[];

void func_ov002_020b14d8(char *c)
{
    int drop;
    int sel;

    Matrix4x3_FromRotationY(c + 0xe4, *(s16*)(c + 0x8e));
    *(int*)(c + 0x108) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x10c) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x110) = *(int*)(c + 0x64) >> 3;
    *(struct M12*)(c + 0x120) = *(struct M12*)(c + 0xe4);
    *(struct M12*)(c + 0x368) = *(struct M12*)data_02082128;
    *(int*)(c + 0x38c) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x390) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x394) = *(int*)(c + 0x64) >> 3;

    drop = *(int*)(c + 0xb0) & 0x40000;
    drop = drop != 0;
    if (drop) return;

    if (((struct BF3ae*)(c + 0x3ae))->b0 == 0) return;

    if (*(int*)(c + 0x3a0) == 1) {
        if (*(unsigned char*)(c + 0x3b0) != 0) return;
    }

    sel = ((struct BF3ae*)(c + 0x3ae))->sel;
    if (sel == 0) {
        _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, c + 0x150, c + 0x368, 0x50000, 0x1f4000, 0xf);
        *(int*)(c + 0xb8) = 0x3e800;
        return;
    }
    if (sel != 1) return;

    {
        int t = *(int*)(c + 0x60) - *(int*)(c + 0x398);
        *(int*)(c + 0xb8) = (t + 0x50000) >> 3;
        _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, c + 0x150, c + 0x368, 0x50000, t + 0x28000, 0xf);
    }
}
