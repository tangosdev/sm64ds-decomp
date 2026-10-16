/* storage 0x0239C9D4 runtime 0x027CE94C linked 0xC4 main ARM.
 * Three flags in a record. Flag 0 selects the 0x8000 path. */

extern void fn_02386F44(int arg);
extern void fn_02386EE8(int arg);
extern void fn_02386EFC(int arg);

int fn_0239C9D4(unsigned short *rec, unsigned short *out) {
    unsigned short flag;
    out[1] = 9;
    flag = rec[8];
    if (flag > 1)
        return 5;
    if (rec[9] > 1)
        return 5;
    if (rec[10] > 1)
        return 5;
    fn_02386F44(flag);
    if (rec[8] == 1) {
        unsigned short *reg;
        if (rec[9] == 1)
            fn_02386EE8(0x8001);
        else
            fn_02386EE8(0);
        reg = (unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x316);
        *reg = (*reg & ~8) | ((rec[10] & 1) << 3);
    } else {
        fn_02386EE8(0x8000);
        fn_02386EFC(2);
    }
    return 0;
}
