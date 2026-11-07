/* storage 0x02383438 runtime 0x037FB2D0 size 0x14 wram ARM. Pool: =0x4000501. */

int fn_02383438(void) {
    unsigned char *p = (unsigned char *)0x4000501;
    int result = *p | 0x80;
    *p = result;
    return result;
}
