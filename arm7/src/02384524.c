/* storage 0x02384524 runtime 0x037FC3BC size 0x1C wram ARM.
 * r = fn_02383F1C(a2); store low half to a1[16]; return full r. No pool. */

extern int fn_02383F1C(int a1);

int fn_02384524(int *a1, int a2) {
    int r = fn_02383F1C(a2);
    ((short *)a1)[16] = (short)r;
    return r;
}
