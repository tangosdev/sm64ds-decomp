/* storage 0x02385E20 runtime 0x037FDCB8 size 0x58 wram ARM.
 * Two little-endian bytes at the cursor, returned as an unsigned halfword. */

extern int fn_02385EA0(unsigned int addr);

unsigned int fn_02385E20(int a1) {
    unsigned int *words = (unsigned int *)a1;
    int lo = fn_02385EA0(words[10]);
    ++*(int *)((char *)a1 + 40);
    unsigned int result = (unsigned short)(lo | (fn_02385EA0(words[10]) << 8));
    ++*(int *)((char *)a1 + 40);
    return result;
}
