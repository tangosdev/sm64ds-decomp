#pragma exceptions off

/* storage 0x02384628 runtime 0x037FC4C0 size 0x3C wram ARM.
 * 3-word struct copy -> ldm/stm. Calls fn_02383F7C. No pool. */

extern void fn_02383F7C(int, int);

struct Three { int x, y, z; };

int fn_02384628(int a1, int *a2, int a3, int a4) {
    *(char *)(a1 + 1) = 0;
    *(struct Three *)(a1 + 0x38) = *(struct Three *)a2;
    *(int *)(a1 + 0x44) = a3;
    fn_02383F7C(a1, a4);
    return 1;
}
