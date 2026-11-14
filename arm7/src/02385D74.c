/* storage 0x02385D74 runtime 0x037FDC0C size 0x40 wram ARM.
 * 7-bit-group integer at the cursor in words[10]. The high bit continues. */

extern int fn_02385EA0(unsigned int addr);

int fn_02385D74(int a1) {
    unsigned int *words = (unsigned int *)a1;
    int acc = 0;
    int byte;
    do {
        byte = fn_02385EA0(words[10]);
        ++*(int *)((char *)a1 + 40);
        acc = (byte & 0x7F) | (acc << 7);
    } while (byte & 0x80);
    return acc;
}
