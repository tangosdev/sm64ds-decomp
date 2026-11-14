/* storage 0x02385DB4 runtime 0x037FDC4C size 0x6C wram ARM.
 * Three little-endian bytes at the cursor in words[10]. */

extern int fn_02385EA0(unsigned int addr);

int fn_02385DB4(int a1) {
    unsigned int *words = (unsigned int *)a1;
    int b0 = fn_02385EA0(words[10]);
    ++*(int *)((char *)a1 + 40);
    int pair = b0 | (fn_02385EA0(words[10]) << 8);
    ++*(int *)((char *)a1 + 40);
    int result = pair | (fn_02385EA0(words[10]) << 16);
    ++*(int *)((char *)a1 + 40);
    return result;
}
