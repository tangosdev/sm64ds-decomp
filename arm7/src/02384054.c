/* storage 0x02384054 runtime 0x037FBEEC size 0x50 wram ARM.
 * Gain helper: zero when muted or below threshold, else the product of
 * two byte gains with the table lookup of the high byte. One BL, no pool.
 * From P1 calibration (matched at -O4,s, never swapped until now);
 * callee name canonicalized to the enrolled fn_0238385C. */

extern int fn_0238385C(int index); /* =0x037FB6F4 */

int fn_02384054(int a1) {
    unsigned t;
    unsigned char b2;
    unsigned char b3;
    if (*(unsigned char *)(a1 + 2) == 0)
        return 0;
    if (*(unsigned short *)(a1 + 6) < *(unsigned short *)(a1 + 4))
        return 0;
    t = *(unsigned short *)(a1 + 8);
    t = fn_0238385C(t >> 8);
    b2 = *(unsigned char *)(a1 + 2);
    b3 = *(unsigned char *)(a1 + 3);
    return b2 * t * b3;
}
