/* storage 0x02396958 runtime 0x027C88D0 size 0x4C main.
 * Copies three halfword triplets from a2/a3/a4 into result. No calls. */

unsigned short *fn_02396958(unsigned short *result, unsigned short *a2, unsigned short *a3, unsigned short *a4) {
    result[0] = a2[0];
    result[1] = a2[1];
    result[2] = a2[2];
    result[3] = a3[0];
    result[4] = a3[1];
    result[5] = a3[2];
    result[6] = a4[0];
    result[7] = a4[1];
    result[8] = a4[2];
    return result;
}
