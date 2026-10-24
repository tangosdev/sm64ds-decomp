/* storage 0x023920F8 runtime 0x027C4070 size 0x3C main ARM.
 * Two-short message send: alloc via 0x03805CE4, msg[0]=0x1A, msg[1]=1,
 * msg[2]=a1, msg[3]=a2, send via 0x03805D8C and return its result.
 * No pool. Twin of 0x027C3FD0 (stor 02392058) which sends 0x19. */

extern unsigned short *fn_0238DE4C(void); /* =0x03805CE4 */
extern int fn_0238DEF4(unsigned short *msg); /* =0x03805D8C */

int fn_023920F8(unsigned short a1, unsigned short a2) {
    unsigned short *msg = fn_0238DE4C();
    msg[0] = 0x1A;
    msg[1] = 1;
    msg[2] = a1;
    msg[3] = a2;
    return fn_0238DEF4(msg);
}
