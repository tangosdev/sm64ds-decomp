/* storage 0x023969A4 runtime 0x027C891C size 0x34 main.
 * Copies two halfword triplets from a2/a3 into result. No calls. */

unsigned short *fn_023969A4(unsigned short *result, unsigned short *a2, unsigned short *a3) {
    result[0] = a2[0];
    result[1] = a2[1];
    result[2] = a2[2];
    result[3] = a3[0];
    result[4] = a3[1];
    result[5] = a3[2];
    return result;
}
