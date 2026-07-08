//cpp
extern "C" {
extern void func_ov002_020bdd2c(void* c);
extern void func_ov002_020bd9ec(void* c, unsigned r1);
extern void func_ov002_020c43c4(void* c, int r1);
extern unsigned char CURRENT_GAMEMODE[];
void _ZN6Player16InitWingFeathersEb(char* c, int b) {
    int t0 = (CURRENT_GAMEMODE[0] == 1);
    if (!t0) {
        if (*(int*)(c+8) != 0) return;
    }
    func_ov002_020bdd2c(c);
    *(unsigned char*)(c+0x6ff) = 1;
    *(short*)(c+0x600+0xae) = 0x708;
    {
        int t1 = (CURRENT_GAMEMODE[0] == 1);
        if (!t1) func_ov002_020bd9ec(c, 0x33);
        else func_ov002_020bd9ec(c, 0x4e);
    }
    if (b == 0) return;
    func_ov002_020c43c4(c, 1);
}
}
