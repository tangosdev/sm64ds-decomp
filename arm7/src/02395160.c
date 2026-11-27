/* storage 0x02395160 runtime 0x027C70D8 size 0x3C + pool 0x44 main.
 * Clear 0x04808004, then poll 0x04808214 up to 16 times. */

void fn_02395160(void) {
    volatile unsigned short *flag = (volatile unsigned short *)0x04808004;
    volatile unsigned short *poll = (volatile unsigned short *)0x04808214;
    int left;
    unsigned short s;

    *flag = 0;
    left = 16;
    goto test;
body:
    s = *poll;
    if (s == 0)
        return;
    if (s == 9)
        return;
    left = left - 1;
test:
    if (left != 0)
        goto body;
}
