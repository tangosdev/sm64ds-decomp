extern void func_ov060_02117ae0(void *);
extern void func_ov060_02117a64(void *);

int _ZN18BowserFireSeaArena8BehaviorEv(char *c) {
    short *a = (short *)(((int)c + 0x8c) & 0xFFFFFFFFFFFFFFFF);
    short *b = (short *)(((int)c + 0x8e) & 0xFFFFFFFFFFFFFFFF);

    *a = *a + *(short *)(c + 0x31e);
    *b = *b + *(short *)(c + 0x320);

    {
        short *d = (short *)(((int)c + 0x90) & 0xFFFFFFFFFFFFFFFF);
        *d = *d + *(short *)(c + 0x322);
    }

    func_ov060_02117ae0(c);
    func_ov060_02117a64(c);
    return 1;
}
