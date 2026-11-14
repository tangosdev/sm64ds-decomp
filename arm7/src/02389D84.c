/* storage 0x02389D84 runtime 0x03801C1C linked 0x128 wram ARM.
 * The <4 and tail calls pass v0 / res as the 4th arg. That value already
 * lives in r3, so b56 emits no mov and the call is the ROM's 3-register
 * setup. Dropping the arg parks the entry load in r1. */

extern unsigned int unk_3809518;
extern int dword_3805E84;
extern int unk_3805E88[];
extern unsigned int unk_3809514;
extern int fn_023882B0(int a1, ...);
extern void fn_02389D60(int a1);
extern int fn_0238D7A0(unsigned int a1, int a2);

void fn_02389D84(void) {
    int v0;
    char *base;
    char *rec;
    int n;
    unsigned long long flags;
    int res;

    v0 = unk_3809518;
    if (v0 == 0) {
        if (fn_023882B0(3, 0x66, 1, 1) != 0)
            fn_02389D60(1);
        return;
    }
    if (v0 < 4) {
        if (v0 == dword_3805E84)
            return;
        fn_023882B0(3, 0x66, 1, v0);
        return;
    }
    base = (char *)&unk_3805E88;
    rec = base + (v0 - 4) * 12;
    n = fn_0238D7A0(unk_3809514, *(unsigned short *)(rec + 0xA));
    flags = *(unsigned long long *)(base + (v0 - 4) * 12);
    if (flags & (0x8000000000000000ULL >> n))
        res = 1;
    else
        res = 2;
    if (++unk_3809514 >= (unsigned)*(unsigned short *)(rec + 8) * *(unsigned short *)(rec + 0xA))
        unk_3809514 = 0;
    if (res != dword_3805E84)
        fn_023882B0(3, 0x66, 1, res);
}
