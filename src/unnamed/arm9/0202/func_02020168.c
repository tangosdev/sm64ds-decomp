extern unsigned char GetPlayerFlagByte(int player, int byte);
/* Bit 5 of player 0's link settings byte 2: the go flag func_02020124
   raises. On seeing it the dScEntry_c sync steps clear this console's
   ready bit (func_02020304) and player 0 drops the flag (func_020200e0). */
unsigned char func_02020168(void) {
    return GetPlayerFlagByte(0, 2) & 0x20;
}
