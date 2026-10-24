/* storage 0x0239E8C8 runtime 0x027D0840 size 0x4C main. */

extern short *fn_023857A4(short *a1, char *a2, char *a3);

short *fn_0239E8C8(short a1) {
    char **pp = (char **)0x0380FFF4;
    char *base = *pp;
    short *v2 = *(short **)(base + 1044);

    v2[0] = 0;
    v2[1] = 0;
    v2[2] = 0;
    v2[5] = 20;
    v2[6] = 164;
    v2[7] = (short)(a1 | 0xC000);
    base = *pp;
    return fn_023857A4(v2 + 8, base + 864, base + 744);
}
