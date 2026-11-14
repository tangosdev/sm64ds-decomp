extern unsigned char _Z17GetPlayerFlagByteii(int player, int byte);
/* Bit 4 of player's link settings byte 2: the rematch ready bit
   func_020201c0 sets. */
unsigned char func_020201f0(int player) {
    return _Z17GetPlayerFlagByteii(player, 2) & 0x10;
}
