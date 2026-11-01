/* storage 0x02380194 runtime 0x037F802C size 0x7C + 1 pool word, wram ARM.
 * Nibble CRC. The table is dword_380581C (0x0380581C). Incoming a4 is ignored
 * until the first nibble, which reloads it from *a2 (the caller leaves r3
 * garbage). The shift is written as a narrowed >> 4: that spelling is the
 * same 16-bit extract as (x << 12) >> 16, and it is what puts zero in r4,
 * the table in r5, and the shift temp in lr. */

extern unsigned short dword_380581C[];

unsigned int fn_02380194(unsigned int result, unsigned short *a2, int a3, int a4) {
    unsigned short *end = (unsigned short *)((char *)a2 + (a3 & ~1));
    unsigned int v5 = 0;
    int zero = 0;
    while (a2 < end) {
        if (v5 == 0)
            a4 = *a2;
        result = (unsigned short)(dword_380581C[result & 0xF] ^ (unsigned short)((unsigned int)result >> 4) ^ dword_380581C[((a4 >> v5) & 0xF)]);
        v5 += 4;
        if (v5 >= 16) {
            v5 = zero;
            a2++;
        }
    }
    return result;
}
