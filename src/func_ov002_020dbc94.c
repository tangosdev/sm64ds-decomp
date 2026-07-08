extern void func_ov002_020dc174(char* c, void* v, int r2, int r3, unsigned int a5, unsigned int a6);
extern unsigned char CURRENT_GAMEMODE;

struct V3 { int x, y, z; };

void func_ov002_020dbc94(char* c) {
    struct V3 v;
    int b = (int)(CURRENT_GAMEMODE == 1);
    if (b) {
        int r2 = 0x82000;
        v.x = 0;
        v.y = -0x32000;
        v.z = 0;
        if (*(int*)(c + 8) == 2) r2 = 0xc8000;
        func_ov002_020dc174(c, &v, r2, 0x5a000, 0x20, 0);
    } else if (*(unsigned char*)(c + 0x703) == 0) {
        v.x = 0;
        v.y = -0x32000;
        v.z = 0;
        func_ov002_020dc174(c, &v, 0x32000, 0x5a000, 0x20, 0);
    } else {
        v.x = 0;
        v.y = -0x40000;
        v.z = 0;
        func_ov002_020dc174(c, &v, 0xc0000, 0x80000, 0x20, 0);
    }
}
