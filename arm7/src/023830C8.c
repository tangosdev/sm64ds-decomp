/* storage 0x023830C8 runtime 0x037FAF60 size 0x74 (code 0x6C + 2-word pool) wram ARM.
 * IRQ enable/disable: stores handler a2 in table unk_3807004[a1], then sets
 * (a2 != 0) or clears (a2 == 0) bit a1 in the word at 0x27FFC00+0x38C, under
 * lock fn_02382AC4/fn_02382AD8. The register is spelled p[227] (227*4=0x38C):
 * an integer (base + 0x38C) folds into one pool word, while the index form
 * keeps the ROM's LDR-base/ADD split. Two pool words (table ABS32 reloc +
 * numeric base). */

extern int fn_02382AC4(void); /* =0x037FA95C lock */
extern int fn_02382AD8(int); /* =0x037FA970 unlock */
extern int unk_3807004[]; /* =0x03807004 */

int fn_023830C8(int a1, int a2) {
    int v = fn_02382AC4();
    int *p = (int *)0x27FFC00;
    unk_3807004[a1] = a2;
    if (a2 != 0)
        p[227] |= 1 << a1;
    else
        p[227] &= ~(1 << a1);
    return fn_02382AD8(v);
}
