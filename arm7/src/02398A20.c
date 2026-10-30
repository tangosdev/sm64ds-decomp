/* storage 0x02398A20 runtime 0x027CA998 size 0x94 + pool 0xAC main ARM.
 * Gate on a status bit, range-check a byte id, clamp a halfword against
 * two shifted table limits, then pulse bit 0x80. */
extern int dword_380FFF4[];
void fn_02398A20(void) {
    int *p;
    unsigned short t;
    unsigned v2;
    unsigned v3;
    *(unsigned short *)0x4808010 = 0x80;
    p = *(int **)dword_380FFF4;
    if ((*(unsigned short *)((char *)p + 0x61C) & 0x20) == 0)
        return;
    t = *(unsigned short *)0x4808214 & 0xFF;
    v2 = t;
    v3 = *(unsigned short *)0x4808268;
    if (v2 < 3 || v2 > 5)
        return;
    if (v3 < (0xFFF & ((unsigned)p[271] >> 1)))
        return;
    if (v3 > (0xFFF & ((unsigned)p[261] >> 1)))
        return;
    *(volatile unsigned short *)0x4808244 |= 0x80;
    *(volatile unsigned short *)0x4808244 &= ~0x80;
}
