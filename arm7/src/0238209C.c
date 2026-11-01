/* storage 0x0238209C runtime 0x037F9F34 size 0xF4 + pool 0x100 wram ARM.
 * Timer-stamped queue pump: stop the channel, set the busy bit, take the head
 * node whose deadline passed, unlink it, run its callback (direct or queued),
 * then run the next head. Pools: two MMIO/constant words and dword_3806FE4.
 * Five calls (one indirect through the node). */

extern int dword_3806FE4[2];
extern unsigned long long fn_02381EB0(void);
extern int fn_02380884(int);
extern int fn_023824BC(int);
extern void fn_0238231C(int *, int, int);

int fn_0238209C(void) {
    unsigned long long v0;
    *(volatile unsigned short *)0x4000106 = 0;
    fn_02380884(16);
    *(int *)0x0380FFF8 |= 0x10;
    v0 = fn_02381EB0();
    {
        int *v2 = (int *)dword_3806FE4[0];
        int *v1 = dword_3806FE4;
        if (v2 == 0)
            return (int)v0;
    if (v0 < *(unsigned long long *)(v2 + 3))
        return fn_023824BC((int)v2);
    {
        int v3 = v2[6];
        v1[0] = v3;
        if (v3 == 0)
            v1[1] = 0;
        else
            *(int *)(v3 + 20) = 0;
        {
            int v4 = *v2;
            if (*(unsigned long long *)(v2 + 7) == 0)
                *v2 = 0;
            if (v4 != 0)
                ((void (*)(int))v4)(v2[1]);
            if (*(unsigned long long *)(v2 + 7) != 0) {
                *v2 = v4;
                fn_0238231C(v2, 0, 0);
            }
        }
        {
            int h = dword_3806FE4[0];
            if (h != 0)
                return fn_023824BC(h);
            return h;
        }
    }
    }
}
