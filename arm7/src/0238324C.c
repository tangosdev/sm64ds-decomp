/* storage 0x0238324C runtime 0x037FB0E4 size 0x40 (code 0x38 + 2-word pool) wram ARM.
 * Sound control: run sub_37FB0A8(0x8000), then publish a control halfword to
 * 0x27FFFA8 from MMIO 0x4000136 (bit 7 -> 0x8000, bits 0/1/3 -> <<10). */

extern int fn_02383210(int); /* =0x037FB0A8 */

void *fn_0238324C(void) {
    int v0 = 0;
    unsigned short reg;
    fn_02383210(0x8000);
    reg = *(volatile unsigned short *)0x4000136;
    if ((reg & 0x80) != 0)
        v0 = 0x8000;
    *(volatile unsigned short *)0x27FFFA8 = v0 | ((reg & 0xB) << 10);
    return (void *)0x27FFFA8;
}
