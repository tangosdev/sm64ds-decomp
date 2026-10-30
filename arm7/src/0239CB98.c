/* storage 0x0239CB98 runtime 0x027CEB10 size 0x70 (code 0x6C + pool) main ARM.
 * Store 1 at a2+2; if *(unk_380FFF4+754) != 1 return 11; else compare a1[7]
 * against fn_0238D594(a1[8]+1, 2)+1, returning 4 or fn_02396BAC(a1[8], a1+9).
 * One pool word (unk_380FFF4), two BL relocs. */
extern unsigned int unk_380FFF4 /* =0x0380FFF4 */;
extern int fn_0238D594 /* =0x0380542C */(int a1, int a2);
extern int fn_02396BAC /* =0x027C8B24 */(unsigned int a1, unsigned short *a2);

int fn_0239CB98(unsigned short *a1, int a2) {
    unsigned int v4;
    *(unsigned short *)(a2 + 2) = 1;
    if (*(unsigned short *)(unk_380FFF4 + 754) != 1)
        return 11;
    v4 = a1[8];
    if (a1[7] < fn_0238D594(v4 + 1, 2) + 1)
        return 4;
    return fn_02396BAC(v4, a1 + 9);
}
