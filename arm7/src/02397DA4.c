/* storage 0x02397DA4 runtime 0x027C9D1C linked 0x34 main ARM.
 * Store a byte value at base+0x2EE and at 0x0480802C. */

int fn_02397DA4(unsigned short value) {
    if (value > 0xFF)
        return 5;
    *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x2EE) = value;
    *(unsigned short *)0x0480802C = value;
    return 0;
}
