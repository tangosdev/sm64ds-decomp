/* storage 0x02383EBC runtime 0x037FBD54 size 0x60 wram ARM.
 * Compare the scaled low bytes of the halfwords at +0x24. */

extern unsigned char dat_03805960[];

#pragma opt_propagation off

int fn_02383EBC(unsigned short *a1, unsigned short *a2) {
    int h1 = *(unsigned short *)((char *)a1 + 0x24);
    int s1 = h1 & 0xFF;
    int h2 = *(unsigned short *)((char *)a2 + 0x24);
    int s2 = h2 & 0xFF;
    int v1;
    int v2;
    int result;

    s1 <<= 4;
    s2 <<= 4;
    v1 = s1 >> dat_03805960[h1 >> 8];
    v2 = s2 >> dat_03805960[h2 >> 8];
    if (v1 != v2) {
        if (v1 < v2)
            result = 1;
        else
            result = -1;
    } else
        result = 0;
    return result;
}
