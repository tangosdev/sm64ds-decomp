/* storage 0x02383FCC runtime 0x037FBE64 size 0x88 wram ARM.
 * Init a 40-byte header plus words, then tail-call fn_02384124(a1 + 40).
 * No pool, one call. Store order matters (0x50 before 0x48/0x4C; 9 before A). */

extern int fn_02384124(unsigned char *a1);

int fn_02383FCC(unsigned char *a1, int a2, int a3, int a4) {
    ((int *)a1)[20] = 0;
    ((int *)a1)[18] = a2;
    ((int *)a1)[19] = a3;
    ((int *)a1)[13] = 0;
    a1[34] = (char)a4;
    *(short *)(a1 + 36) = 127;
    a1[3] &= ~2;
    a1[8] = 0x3C;
    a1[5] = 0x3C;
    a1[9] = 127;
    a1[10] = 0;
    *(short *)(a1 + 12) = 0;
    *(short *)(a1 + 14) = 0;
    a1[11] = 0;
    *(short *)(a1 + 50) = 0;
    ((int *)a1)[6] = 0;
    ((int *)a1)[5] = 0;
    a1[28] = 127;
    *(short *)(a1 + 30) = 127;
    a1[29] = 127;
    *(short *)(a1 + 32) = 127;
    return fn_02384124(a1 + 40);
}
