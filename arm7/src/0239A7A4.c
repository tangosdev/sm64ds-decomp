/* storage 0x0239A7A4 runtime 0x027CC71C linked 0x8C main.
 * Clears one 0x1C slot, drops its bit, and decrements the live count.
 * Propagation off keeps the shift constant in lr across the zero store.
 */
#pragma opt_propagation off

extern int *dat_0380FFF4;
extern void fn_02396CF0(void);

struct Rec {
    char pad[0x2E0];
    int list;
};

void fn_0239A7A4(int a1) {
    struct Rec **w;
    char *slot;
    int off;
    int bit;
    int one;
    unsigned short *bits;

    w = (struct Rec **)&dat_0380FFF4;
    slot = (char *)((*w)->list + 2);
    off = a1 * 0x1C;
    one = 1;
    bit = *(unsigned short *)(slot + off);
    *(unsigned short *)(slot + off) = 0;
    bits = (unsigned short *)((int)*w + 0x4F2);
    *bits = (unsigned short)(*bits & ~(one << bit));
    --*(unsigned short *)((int)*w + 0x4F0);
    if (((unsigned short *)((int)*w + 0x400))[0x78] == 0)
        fn_02396CF0();
}
