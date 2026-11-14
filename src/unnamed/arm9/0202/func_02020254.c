extern unsigned char _Z17GetPlayerFlagByteii(int player, int byte);
/* Bits 0-3 of player 0's link settings byte 2: the slot mask
   func_02020214 posts. */
unsigned char func_02020254(void) {
    return _Z17GetPlayerFlagByteii(0, 2) & 0xf;
}
