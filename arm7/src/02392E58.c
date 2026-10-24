/* storage 0x02392E58 runtime 0x027C4DD0 size 0xB4 + pool 0xC0 main ARM.
 * Popcount a 16-bit mask, then scale: v5 = 4*(a1+0x22)+0x60 + (a2*v3+0x388)
 * against 10 * (127*(const-halfword)/20). Key shapes: the `+0x388` bound to
 * the PRODUCT (an MLA needs a register addend, so the imm-bound add cannot
 * fuse and the `+0x388` never sinks onto the final compare); loop index
 * initialized from the counter (`i = v3`, MOV R12,LR); unsigned tail
 * compare (`MOVCC`, not MOVLT). Pools are all numeric (MMIO + 2 values).
 * One call (divide helper, stays asm, abs row in syms.d). */
extern int fn_0238D594(int, int);
int fn_02392E58(int a1, int a2, int a3) {
    int v3 = 0;
    int i = v3;
    int v5;
    unsigned v6;
    int result;
    for (; i < 16; i++) {
        if (a3 & (1 << i))
            v3 = (unsigned short)(v3 + 1);
    }
    {
        int base = 4 * (a1 + 0x22) + 0x60;
        int t = a2 * v3 + 0x388;
        v5 = base + t;
    }
    v6 = *(volatile unsigned short *)0x04000006;
    if (v6 > 0x104)
        result = (unsigned short)fn_0238D594(127 * (0x209 - (int)v6), 20);
    else
        result = (unsigned short)fn_0238D594(127 * (0x102 - (int)v6), 20);
    if ((unsigned)(10 * result) < (unsigned)v5)
        return 0;
    return result;
}
