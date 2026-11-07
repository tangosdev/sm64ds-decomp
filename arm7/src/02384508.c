/* storage 0x02384508 runtime 0x037FC3A0 size 0x10 wram ARM. No pool, no calls.
 * Return bit 0 of the flag byte at +3. The 1-bit bitfield is what emits the
 * LDRB + LSL#31/LSR#31 extraction; a plain `& 1` would fold to AND. */

typedef struct {
    unsigned char unk_00[3];
    unsigned char flag0 : 1;
} P14_02384508;

int fn_02384508(P14_02384508 *a1) {
    return a1->flag0;
}
