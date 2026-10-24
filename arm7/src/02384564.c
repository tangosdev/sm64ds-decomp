/* storage 0x02384564 runtime 0x037FC3FC size 0x20 wram ARM.
 * Pool: =arr_3805974 (abs32). Predicated diamond: STRBLT/STRBGE. */

extern unsigned char arr_3805974[];

int fn_02384564(int result, int a2) {
    if (a2 < 109)
        *(char *)(result + 28) = 0xFF - a2;
    else
        *(char *)(result + 28) = arr_3805974[127 - a2];
    return result;
}
