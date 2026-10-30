/* storage 0x02381E78 runtime 0x037F9D10 size 0x1C wram ARM.
 * List push: link a2 in front of a1. No calls, no pool. */

int *fn_02381E78(int *a1, int *a2) {
    a2[1] = (int)a1;
    *a2 = 0;
    if (a1 != 0)
        *a1 = (int)a2;
    return a2;
}
