extern int IsStarCollectedInLevel(signed char levelID, int starID);
extern int _Z14ApproachLinearRiii(int *a, int b, int c);
extern void func_ov080_0212555c(void *c);
extern void func_ov080_021256f8(void *c);
extern int func_ov080_02125bb0(void *c, int x);
extern void func_ov080_02125940(void *c);
extern void func_ov080_02125af0(void *c);
extern int SAVE_DATA[];

struct E { char _0[8]; int f8; int fc; char _10[8]; };

void func_ov080_021264ec(char *c)
{
    if ((unsigned char)((*(unsigned int*)(c + 8) >> 8) & 0x1f) == 7 &&
        !(SAVE_DATA[2] & 0x40000) &&
        IsStarCollectedInLevel(0x12, 1)) {
        if (_Z14ApproachLinearRiii((int*)(c + 0x5c), *(int*)(c + 0x1b0) + 0x802000, 0x13e72))
            SAVE_DATA[2] |= 0x40000;
        func_ov080_0212555c(c);
    }
    func_ov080_021256f8(c);
    int i;
    for (i = 0; i < *(unsigned short*)(c + 0x1b8); i++) {
        struct E *e = (struct E *)(*(char**)(c + 0x1a0) + i * 0x18);
        e->f8 = func_ov080_02125bb0(c, e->fc);
    }
    func_ov080_02125940(c);
    func_ov080_02125af0(c);
    *(unsigned short*)(c + 0x1b4) = (unsigned short)(*(short*)(c + 0x1b4) + (*(int**)(c + 0x1ac))[1]);
}
