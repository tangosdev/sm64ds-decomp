extern unsigned char GetLinkByte(int slot, int byte);
/* Bits 0-2 of player 0's link settings byte 1: the course index
   func_0202029c posts. */
unsigned char GetParentCourseIndex(void) {
    return GetLinkByte(0, 1) & 7;
}
