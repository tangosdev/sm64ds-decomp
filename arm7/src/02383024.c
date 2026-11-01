/* storage 0x02383024 runtime 0x037FAEBC size 0xA4 (code 0x9C + 2-word pool) wram ARM.
 * IRQ acknowledge/enable write: builds a 32-bit 0x4000188 value from the args
 * (bits 0-4 = a1, bit 5 = a3&1, bits 6-31 = a2&0x3FFFFFF, each merged over the
 * previous slot value with an explicit mask), then under lock fn_02382AC4 /
 * fn_02382AD8 writes it unless MMIO 0x4000184 reports busy (0x4000 -> -1) or
 * held (bit 1 -> -2). The first merge seeds from *(int *)&v (a literal load
 * of the slot, no store-back); a plain uninitialized read lets the compiler
 * reuse a1 and promote v to R4 instead. Two numeric pool words (0x4000184
 * deduped, 0x4000188). */

extern int fn_02382AC4(void); /* =0x037FA95C lock */
extern int fn_02382AD8(int); /* =0x037FA970 unlock */

int fn_02383024(int a1, int a2, int a3) {
    int v;
    v = (*(int *)&v & ~0x1F) | (a1 & 0x1F);
    v = (v & ~0x20) | ((a3 & 1) << 5);
    v = (v & 0x3F) | ((a2 & 0x3FFFFFF) << 6);
    if (*(volatile unsigned short *)0x4000184 & 0x4000) {
        *(volatile unsigned short *)0x4000184 |= 0xC000;
        return -1;
    }
    {
        int lk = fn_02382AC4();
        if (*(volatile unsigned short *)0x4000184 & 2) {
            fn_02382AD8(lk);
            return -2;
        }
        *(volatile unsigned int *)0x4000188 = v;
        fn_02382AD8(lk);
        return 0;
    }
}
