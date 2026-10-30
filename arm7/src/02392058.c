/* storage 0x02392058 runtime 0x027C3FD0 size 0x3C main ARM.
 * Alloc-fill-send with header (25, 1) plus two halfword args. No pool. */

extern short *fn_0238DE4C(void); /* =0x03805CE4 alloc */
extern int fn_0238DEF4(short *); /* =0x03805D8C send */

int fn_02392058(int a1, int a2) {
    short *msg = fn_0238DE4C();
    msg[0] = 25;
    msg[1] = 1;
    msg[2] = a1;
    msg[3] = a2;
    return fn_0238DEF4(msg);
}
