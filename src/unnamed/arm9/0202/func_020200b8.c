extern unsigned char GetPlayerFlagByte(int player, int byte);

/* Bits 0-1 of player 0's link settings byte 3: the course-select action
   func_02020078 posts. */
unsigned char func_020200b8(void) {
    return GetPlayerFlagByte(0, 3) & 3;
}
