extern unsigned char _Z17GetPlayerFlagByteii(int player, int byte);
/* Bit 3 of player's link settings byte 0: the ready bit
   func_02020334 sets. */
unsigned char func_02020364(int player) {
    return _Z17GetPlayerFlagByteii(player, 0) & 8;
}
