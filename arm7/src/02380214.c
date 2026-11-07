/* storage 0x02380214 runtime 0x037F80AC size 0xBC + 4 pool wram ARM.
 * One constant pool (0x0000FFFF), one call (fn_02380194, in-pack). Takes only
 * a1; the 4th arg of fn_02380194 is left as garbage R3 (that callee reloads
 * a4 from *a2 on its first nibble, so incoming R3 is never read), which is
 * why there is no R3 homing. Declared locally with 3 params for that reason.
 * A switch (cases 1, 2 sharing the body) reproduces the CMP/BEQ chains and
 * the shared `return v5` block; an if/|| chain optimizes to SUB+LS instead. */

extern unsigned int fn_02380194(unsigned int result, unsigned short *a2, int a3);

int fn_02380214(int a1) {
    unsigned short v6;
    int v5;
    v5 = 0;
    v6 = v5;
    do {
        unsigned short *v8 = (unsigned short *)(a1 + 116 * v6);
        unsigned int rr = fn_02380194(0xFFFF, v8, 112);
        if (rr == v8[57] && v8[56] < 0x80)
            v5 |= 1 << v6;
        v6++;
    } while (v6 < 2);
    switch (v5) {
    case 1:
    case 2:
        return v5;
    case 3:
        if (((unsigned char)((*(unsigned short *)(a1 + 112) + 1) & 0x7F)) == *(unsigned short *)(a1 + 228))
            return 2;
        return 1;
    }
    return 0;
}
