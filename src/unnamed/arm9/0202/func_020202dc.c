extern unsigned char _Z17GetPlayerFlagByteii(int player, int byte);
/* Bits 0-2 of player 0's link settings byte 1: the course index
   func_0202029c posts. */
unsigned char func_020202dc(void) {
    return _Z17GetPlayerFlagByteii(0, 1) & 0x7;
}
