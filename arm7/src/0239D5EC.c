/* storage 0x0239D5EC runtime 0x027CF564 linked 0x4C main ARM.
 * Command 1. Modes 0 and 0x10 call fn_023869F4(0). */

extern void fn_023869F4(int arg);

int fn_0239D5EC(int unused, unsigned short *out) {
    unsigned short mode;
    int result = 1;
    out[1] = 1;
    mode = *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x310);
    if (mode == 0 || mode == 0x10) {
        fn_023869F4(0);
        return 0;
    }
    return result;
}
