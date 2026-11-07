// NONMATCHING: best C links 0x78 vs ROM 0x70: two extra frame words (push {r4-r7,lr} plus sub/add sp,#4); b56 keeps every value live across the BL in callee-saved registers.
// @ 0x02386100 size 0x6C (runtime 0x037FDF98, wram). Residue class: register allocation / frame shape.
// Closest C attempt from AH17 (tried, closed); not enrolled, so the build keeps the original bytes for this range.
extern unsigned int unk_3807B04;
extern char *fn_023859CC(int entry, int ch); /* =0x037FD864 */
int fn_02386100(int a1, unsigned int a2, int a3) {
    char *entry = (char *)&unk_3807B04 + 36 * a1;
    unsigned short half = (unsigned short)a3;
    unsigned int mask = a2;
    int ch = 0;
    char *slot;
    int bit;
    goto test;
loop:
    bit = mask & 1;
    if (bit == 0)
        goto next;
    slot = fn_023859CC((int)entry, ch);
    if (slot != 0) {
        *(unsigned short *)(slot + 0x1E) = half;
        *(unsigned char *)slot |= 0x80;
    }
next:
    ++ch;
    mask >>= 1;
test:
    if (ch >= 16)
        goto done;
    if (mask != 0)
        goto loop;
done:
    return bit;
}
