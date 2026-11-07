/* storage 0x0239CF0C runtime 0x027CEE84 size 0xAC main. */

extern unsigned char *off_380FFF4; /* =0x0380FFF4 */

void fn_0239CF0C(void) {
    unsigned int t = *(unsigned short *)(off_380FFF4 + 0x300 + 0x16);
    if ((t << 26) >> 31) {
        unsigned short *p = (unsigned short *)0x4804000;
        p[0] = 0;
        p[2] = 0;
        *(unsigned short *)0x48080A0 |= 0x8000;
    } else {
        *(unsigned short *)0x48080AC = 1;
        *(unsigned short *)0x4808004 = 0;
        *(unsigned short *)0x4808012 = 2;
        *(unsigned short *)0x4808010 = 0xFFFF;
        *(unsigned short *)0x4808194 = 0;
        *(unsigned short *)0x4808040 = 0;
    }
}
