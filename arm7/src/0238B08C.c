// NONMATCHING: closest C attempt, not a byte match (storage 0x0238B08C runtime 0x03802F24. Not a byte match.).
// @ 0x0238B08C size 0x90 (runtime 0x03802F24, wram). Residue class: see the notes below.
// Tried by AH56; not enrolled, so the build keeps the original bytes for this range.
#pragma opt_removeemptyloops off

/* storage 0x0238B08C runtime 0x03802F24. Not a byte match.
 * Links 0x88; the ROM function is 0x90 with no pool and no reloc.
 * Not enrolled.
 * SIOCNT 0x04000138 byte writer, low bit first.
 * -O4,s keeps the predicated halfword load, the 0x77/0x74 setup,
 * and the subs/bne delays. It pools the port address, tests the
 * odd pointer with ands, drops a no-op or with immediate 0, folds
 * clear-bit-1 then set-bit-1 into one or, and saves callee-saved
 * registers once instead of pushing the two arguments each byte.
 */

void fn_0238B08C(unsigned char *p, int n) {
    volatile unsigned short *reg = (volatile unsigned short *)0x04000138;
    do {
        unsigned int bits;
        unsigned int cfg;
        unsigned int bit;
        if (((unsigned int)p & 1) == 0)
            bits = *(unsigned short *)p;
        else
            bits = (unsigned int)*(unsigned short *)(p - 1) >> 8;
        cfg = *reg;
        cfg &= ~0x77u;
        cfg |= 0x74u;
        bit = 0;
        do {
            cfg &= ~3u;
            cfg |= (bits >> bit) & 1u;
            *reg = (unsigned short)cfg;
            {
                unsigned d = 9;
                do {
                    d--;
                } while (d);
            }
            cfg &= ~2u;
            cfg |= 2u;
            *reg = (unsigned short)cfg;
            {
                unsigned d = 9;
                do {
                    d--;
                } while (d);
            }
            bit++;
        } while (bit != 8);
        p++;
        n--;
    } while (n != 0);
}
