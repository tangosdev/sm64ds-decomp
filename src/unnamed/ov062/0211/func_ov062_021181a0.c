extern int _Z14ApproachLinearRiii(int *, int, int);
extern int _ZN9Animation8FinishedEv(void *);
extern int RandomIntInternal(int *seed);
extern void func_ov062_02117994(char *c, int idx);
extern int data_0209e650;

void func_ov062_021181a0(char *c) {
    if (*(int*)(c + 0x390) == 2) {
        _Z14ApproachLinearRiii((int*)(c + 0x98), 0x2000, 0x4cc);
    } else {
        _Z14ApproachLinearRiii((int*)(c + 0x98), 0x3000, 0x4cc);
    }
    if (_ZN9Animation8FinishedEv(c + 0x350) == 0) return;
    {
        unsigned short *p = (unsigned short*)(((int)c + 0x3c4));
        *p = (unsigned short)(*p + 1);
    }
    {
        char *b = c + 0x300;
        *(unsigned short*)(b + 0xc6) =
            (unsigned short)(((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) % 70 + 30);
    }
    func_ov062_02117994(c, 1);
}
