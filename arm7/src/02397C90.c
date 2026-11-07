/* storage 0x02397C90 runtime 0x027C9C08 linked 0x3C main ARM.
 * Store a value 0..2 at base+0x2F4, then fn_02386FE8. */

extern void fn_02386FE8(void);

int fn_02397C90(unsigned int value) {
    if (value > 2)
        return 5;
    *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x2F4) = value;
    fn_02386FE8();
    return 0;
}
