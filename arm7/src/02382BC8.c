/* storage 0x02382BC8 runtime 0x037FAA60 size 0x38 + 8 pool wram ARM.
 * Two pool refs (word_3806FF8 data, fn_02382B7C address), one call. */

extern unsigned short word_3806FF8;
extern int fn_023830C8(int a1, int a2);
extern int fn_02382B7C(int a1, int a2);

int fn_02382BC8(void) {
    int result = (int)&word_3806FF8;
    if (word_3806FF8 == 0) {
        word_3806FF8 = 1;
        return fn_023830C8(12, (int)&fn_02382B7C);
    }
    return result;
}
