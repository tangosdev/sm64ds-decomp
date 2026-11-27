/* storage 0x02381E50 runtime 0x037F9CE8 size 0x28 wram ARM.
 * List unlink: splice a2 out of its doubly-linked list. No calls, no pool.
 * Returns the untouched first argument. */

int fn_02381E50(int a1, int *a2) {
    int *v2 = (int *)a2[1];
    if (v2 != 0)
        *v2 = *a2;
    if (*a2 == 0)
        return a2[1];
    *(int *)(*a2 + 4) = a2[1];
    return a1;
}
