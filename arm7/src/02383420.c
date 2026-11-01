/* storage 0x02383420 runtime 0x037FB2B8 size 0x14 wram ARM. Pool: =0x4000501. */

int fn_02383420(void) {
    unsigned char *p = (unsigned char *)0x4000501;
    int result = *p & ~0x80;
    *p = result;
    return result;
}
