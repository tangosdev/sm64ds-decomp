/* storage 0x023964FC runtime 0x027C8474 size 0xAC main.
 * Compares a1 bytes from a2 against two WRAM streams via fn_02395334.
 * One pool data ref (off_380FFF4), three calls to fn_02395334. */

extern unsigned char *off_380FFF4; /* =0x0380FFF4 */
extern unsigned int fn_02395334(unsigned char *p); /* =0x027C72AC */

unsigned int fn_023964FC(unsigned int a1, unsigned char *a2) {
    unsigned char * volatile *pp;
    unsigned char *base;
    unsigned short n;
    unsigned char *p796;
    unsigned char *p828;
    unsigned int i;
    if (a1 > 0x20)
        return 0;
    pp = &off_380FFF4;
    base = *pp;
    n = *(unsigned short *)(base + 0x300 + 0x1A);
    if (n == 0)
        return 1;
    if (a1 != n)
        return 0;
    p796 = base + 796;
    p828 = base + 828;
    for (i = 0; i < a1; i++) {
        unsigned int e = fn_02395334(p828);
        unsigned int c;
        unsigned int d;
        p828++;
        c = fn_02395334(a2);
        a2++;
        d = fn_02395334(p796);
        p796++;
        if ((c | e) != (d | e))
            return 0;
    }
    return 1;
}
