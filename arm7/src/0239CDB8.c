/* storage 0x0239CDB8 runtime 0x027CED30 size 0x60 (code 0x5C + pool) main ARM.
 * Store 1 at a2+2; v2 = *(unk_380FFF4+784): return 1 if v2 > 0x20, or if
 * v2 == 0x20 with bit 2 of *(unk_380FFF4+790) set; else fn_02397CCC(a1[8]).
 * Bit 2 is ((hv << 29) >> 31): a single-bit extract stays LSL/LSR under
 * -O4,s. One pool word (unk_380FFF4), one BL reloc. */
extern unsigned int unk_380FFF4 /* =0x0380FFF4 */;
extern int fn_02397CCC /* =0x027C9C44 */(int a1);

int fn_0239CDB8(unsigned short *a1, int a2) {
    unsigned int v2;
    unsigned int hv;
    *(unsigned short *)(a2 + 2) = 1;
    v2 = *(unsigned short *)(unk_380FFF4 + 784);
    if (v2 > 0x20)
        return 1;
    if (v2 == 0x20) {
        hv = *(unsigned short *)(unk_380FFF4 + 790);
        if (((hv << 29) >> 31) != 0)
            return 1;
    }
    return fn_02397CCC(a1[8]);
}
