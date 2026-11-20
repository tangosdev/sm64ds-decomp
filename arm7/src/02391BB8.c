/* storage 0x02391BB8 runtime 0x027C3B30 size 0xE0 (code 0xDC + 1-word pool) main ARM.
 * Unpack 8-word param block (one word + four load-adjacent ushort narrows +
 * two words), bump the third to 1 when *(base+0x184) is nonzero, call
 * fn_02393C1C with 7 args; when the result != 2, alloc-fill-send a message
 * (0x81, result, hole at +4/+6, 0x14, w4, w3, w2, w1, w6, w7, pool 0xFFFF). */

extern int *fn_0238E688(void); /* =0x027C0600 */
extern int fn_02393C1C(int, int, int, int, int, int, int); /* =0x027C5394 */
extern short *fn_0238DE4C(void); /* =0x03805CE4 alloc */
extern int fn_0238DEF4(short *); /* =0x03805D8C send */

void fn_02391BB8(int *p) {
    int *t0 = fn_0238E688();
    int w1 = p[1];
    unsigned short w2 = p[2];
    unsigned short w3 = p[3];
    unsigned short w4 = p[4];
    unsigned short w5 = p[5];
    int w6 = p[6];
    int w7 = p[7];
    char *qb = (char *)t0 + 0x1000;
    char *sb = *(char **)(qb + 0x610);
    char *fb = *(char **)sb;
    int v;
    short *q;
    if (*(unsigned short *)(fb + 0x184) != 0)
        w3 = 1;
    v = fn_02393C1C(w5, w4, w3, w1, w2, w6, w7);
    if (v != 2) {
        q = fn_0238DE4C();
        q[0] = 0x81;
        q[1] = v;
        q[4] = 0x14;
        q[5] = w4;
        q[6] = w3;
        q[7] = w2;
        *(int *)(q + 8) = w1;
        *(int *)(q + 10) = w6;
        *(int *)(q + 12) = w7;
        *(unsigned short *)(q + 14) = 0xFFFF;
        fn_0238DEF4(q);
    }
}
