/* storage 0x0238313C runtime 0x037FAFD4 size 0xD4 (code 0xB8 + 7-word pool) wram ARM.
 * One-time init under lock fn_02382AC4/fn_02382AD8: guards on word_3807000,
 * zeroes *(0x27FFC00+0x38C) (via int* base + index 227 so the pool keeps the
 * 0x27FFC00 base with a #0x38C store offset) and table unk_3807004[0..31],
 * registers callback fn_02382F04 with fn_02380978(0x40000), runs
 * fn_023808C0(0x40000), writes 0xC408 to MMIO 0x4000184, then a `for v3=8..0`
 * loop poking MMIO 0x4000180 with fn_02382B28(0x3E8) and a retry
 * (`if ((*v4&0xF)!=v3) v3=8`). */

extern int fn_02382AC4(void); /* =0x037FA95C lock */
extern int fn_02382AD8(int); /* =0x037FA970 unlock */
extern unsigned int fn_02380978(unsigned int, int); /* =0x037F8810 */
extern int fn_023808C0(int); /* =0x037F8758 */
extern int fn_02382B28(int); /* =0x037FA9C0 */
extern int fn_02382F04(void); /* =0x037FAD9C (in-pack 0x40000-callback arg) */
extern unsigned short word_3807000; /* =0x03807000 */
extern int unk_3807004[]; /* =0x03807004 */

int fn_0238313C(void) {
    int v0 = fn_02382AC4();
    if (word_3807000 == 0) {
        int v1;
        int *p;
        int v3;
        volatile unsigned short *v4;
        word_3807000 = 1;
        v1 = 0;
        p = (int *)0x27FFC00;
        p[227] = 0;
        do
            unk_3807004[v1++] = 0;
        while (v1 < 32);
        fn_02380978(0x40000, (int)&fn_02382F04);
        fn_023808C0(0x40000);
        *(volatile unsigned short *)0x4000184 = 0xC408;
        v3 = 8;
        v4 = (volatile unsigned short *)0x4000180;
        while (v3 >= 0) {
            *v4 = v3 << 8;
            fn_02382B28(0x3E8);
            if ((*v4 & 0xF) != v3)
                v3 = 8;
            --v3;
        }
    }
    return fn_02382AD8(v0);
}
