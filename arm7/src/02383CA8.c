/* Capture channel. Bit 2 of the control byte is set when the fifth
 * argument is zero; the pointer arms are what emit movne then moveq. */

void fn_02383CA8(int channel, int control, int dest, int length, int repeat, int bit1, int bit0) {
    volatile unsigned char *mmio = (volatile unsigned char *)0x04000000;
    int scaled = channel << 3;
    int bit2 = (int)(repeat ? (char *)0 : (char *)1);
    int packed = bit0 | ((control << 3) | (bit2 << 2) | (bit1 << 1));
    *(mmio + 0x508 + channel) = (unsigned char)packed;
    *(volatile unsigned int *)(mmio + scaled + 0x510) = dest;
    *(volatile unsigned short *)(mmio + scaled + 0x514) = (unsigned short)length;
}
