/* Restart the periodic alarm at *(int *)0x0380FFF4 + 0x5C0 with a converted
 * period: cancel, scale (a1 * 0x82EA) >> 6, start = tick + period.
 * The period is passed as one 64-bit value; split words keep the zero in
 * r3 and the add in ip with the stores ordered low to high. Pool: 0380FFF4. */

extern void fn_0238219C(int);
extern unsigned long long fn_02381EB0(void);
extern int fn_0238222C(int *, int, int, unsigned long long, int, int);

int fn_0239565C(int a1, int a2) {
    unsigned v4;
    unsigned long long now;
    unsigned long long start;

    fn_0238219C(*(int *)0x0380FFF4 + 0x5C0);
    v4 = ((unsigned)a1 * 0x82EA) >> 6;
    now = fn_02381EB0();
    start = (unsigned long long)v4 + now;
    return fn_0238222C((int *)(*(int *)0x0380FFF4 + 0x5C0), (unsigned)start,
                       (unsigned)(start >> 32), (unsigned long long)v4, a2, 0);
}
