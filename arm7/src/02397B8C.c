/* storage 0x02397B8C runtime 0x027C9B04 linked 0x24 main ARM.
 * Store a value 0..3 at base+0x2FA. */

int fn_02397B8C(unsigned int value) {
    if (value > 3)
        return 5;
    *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x2FA) = value;
    return 0;
}
