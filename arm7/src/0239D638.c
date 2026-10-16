/* storage 0x0239D638 runtime 0x027CF5B0 linked 0x38 main ARM.
 * Copy one slot up, mark it, then tail-call fn_0238AE84. */

extern int fn_0238AE84(int a, unsigned short *b);

int fn_0239D638(int a1, unsigned short *a2) {
    unsigned short index = a2[7];
    unsigned short *slot = (unsigned short *)((char *)a2 + (index << 1) + 0x10);
    a2[6] = *slot;
    slot[1] = 2;
    slot[2] = 0;
    slot[3] = a2[12];
    return fn_0238AE84(a1, a2);
}
