typedef unsigned short u16;

enum { false, true };

extern u16 data_ov002_020ff01c[];

void func_ov002_020ae8b8(char* c, char* arg)
{
    int b;
    b = *(u16*)(arg + 0xc);
    b = b == 0xbf;
    if (b != false) {
        u16 t = data_ov002_020ff01c[(u16)*(int*)(arg + 8)];
        int r4 = (int)t << 3;
        int ip = r4 << 12;
        int d1 = ip / 100;
        int d2 = r4 / 100;
        *(int*)(c + 0x98) = d1;
        *(int*)(c + 0xa8) = (d2 + 0x20) << 12;
    } else {
        *(int*)(c + 0x98) = 0xa000;
        *(int*)(c + 0xa8) = 0x28000;
    }
    *(int *)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFF) &= ~1;
}
