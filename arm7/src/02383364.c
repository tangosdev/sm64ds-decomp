/* storage 0x02383364 runtime 0x037FB1FC size 0xC wram ARM. Pool: =0x4000500. */

char fn_02383364(char result) {
    *(unsigned char *)0x4000500 = result;
    return result;
}
