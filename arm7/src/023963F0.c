/* storage 0x023963F0 runtime 0x027C8368 size 0x4C+pool main ARM.
 * Flag test with two return-10 arms. V4: switch + lazy split pointers. */

int fn_023963F0(void) {
    int v;
    int base = *(int *)0x0380FFF4;
    unsigned short *p1;
    unsigned short *p2;

    p1 = (unsigned short *)(base + 0x200);
    v = p1[0x7A];
    switch (v) {
    case 0:
        p2 = (unsigned short *)(base + 0x300);
        if ((p2[0x2E] & 1) != 0)
            return 10;
        break;
    case 1:
        return 10;
    default:
        break;
    }
    return 20;
}
