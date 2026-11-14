// NONMATCHING: closest C attempt, not a byte match (storage 0x02385F2C runtime 0x037FDDC4 NOT matched (AH14 C-only redo: 0/1).).
// @ 0x02385F2C size 0x90 (runtime 0x037FDDC4, wram). Residue class: see the notes below.
// Tried by AH14; not enrolled, so the build keeps the original bytes for this range.
/* storage 0x02385F2C runtime 0x037FDDC4 NOT matched (AH14 C-only redo: 0/1).
 * Best C shape: 0x94 linked (36 code words + pool) vs ROM 0x94 - SAME SIZE,
 * closest ever (prior best 0x9C). SDK port of SNDi_SetTrackParam (SND_seq.c)
 * with if/else-if dispatch. The `*(volatile int *)&size` re-read forces a
 * per-iteration stack load of the 5th arg after the BL (blocks LICM hoisting
 * that would otherwise home size to a callee-saved reg); this drops the push
 * from 9 regs + frame to 8 regs, no frame. Remaining diff is purely homes +
 * dispatch layout: mask r8 vs r4, offset r7 vs r2, data r6 vs r3, ch r5 vs r7,
 * entry r4 vs r8, half sb vs r5, byte sl vs r6, size reload r1 vs homed r12,
 * predicated-inline stores vs switch BEQ-forward arms. b56 keeps every
 * BL-crossing value callee-saved (verified: static-leaf same-TU, pure
 * attribute, all inert); offset/data in r2/r3 and size in r12 are outside
 * its repertoire. */
extern unsigned int unk_3807B04;
extern char *fn_023859CC(int entry, int ch); /* local extern: callee owned by another job, no header */
void fn_02385F2C(int player, unsigned int trackMask, unsigned int offset, unsigned int data, int size) {
    char *entry = (char *)&unk_3807B04 + 36 * player;
    unsigned int mask = trackMask;
    char *slot;
    for (int ch = 0; ch < 16 && mask != 0; ch++, mask >>= 1) {
        int w;
        if ((mask & 1) == 0)
            continue;
        slot = fn_023859CC((int)entry, ch);
        if (slot == 0)
            continue;
        w = *(volatile int *)&size;
        if (w == 1)
            *(unsigned char *)(slot + offset) = (unsigned char)data;
        else if (w == 2)
            *(unsigned short *)(slot + offset) = (unsigned short)data;
        else if (w == 4)
            *(unsigned int *)(slot + offset) = data;
    }
}
