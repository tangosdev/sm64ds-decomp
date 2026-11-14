#pragma exceptions off

/* storage 0x02384548 runtime 0x037FC3E0 size 0x1C wram ARM.
 * Calls fn_02383F1C. No pool. */

extern int fn_02383F1C(int);

int fn_02384548(int a1, int a2) {
    int result = fn_02383F1C(a2);
    *(short *)(a1 + 30) = result;
    return result;
}
