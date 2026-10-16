/* storage 0x023969D8 runtime 0x027C8950 size 0x1C main.
 * Copies one halfword triplet from a2 into result. No calls. */

unsigned short *fn_023969D8(unsigned short *result, unsigned short *a2) {
    result[0] = a2[0];
    result[1] = a2[1];
    result[2] = a2[2];
    return result;
}
