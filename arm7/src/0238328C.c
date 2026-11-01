/* storage 0x0238328C runtime 0x037FB124 size 0x90 + 4 pool words wram ARM. */

extern int fn_02381FE8(void);
extern unsigned fn_0238245C(void);
extern int *fn_0238244C(int *);
extern long long fn_02381EB0(void);
extern void fn_0238222C(int *, long long, long long, void *(*)(void), int);
extern void *fn_0238324C(void);
extern int dword_3807084;
extern int unk_3807088[];

int fn_0238328C(int a1) {
    long long tick;
    if (fn_02381FE8() != 0 && fn_0238245C() != 0)
        goto cont;
    return 0;
cont:
    if (dword_3807084 != 0)
        return 0;
    fn_0238244C(unk_3807088);
    tick = fn_02381EB0();
    fn_0238222C(unk_3807088, tick + 0x82E, 0x82E, fn_0238324C, 0);
    dword_3807084 = 1;
    return 1;
}
