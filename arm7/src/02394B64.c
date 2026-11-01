/* storage 0x02394B64 runtime 0x027C6ADC size 0x1C main ARM, no pool.
 * Init one slot: empty links, zero status, keep the tag. */
int fn_02394B64(int result, int a2) {
    *(int *)result = -1;
    *(int *)(result + 4) = -1;
    *(unsigned short *)(result + 8) = 0;
    *(unsigned short *)(result + 10) = a2;
    return result;
}
