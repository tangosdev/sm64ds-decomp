extern unsigned char _Z17GetPlayerFlagByteii(int player, int byte);

/* Bits 0-1 of player 0's link settings byte 3: the course-select action
   func_02020078 posts. */
unsigned char func_020200b8(void) {
    return _Z17GetPlayerFlagByteii(0, 3) & 3;
}
