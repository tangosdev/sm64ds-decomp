/* storage 0x02383634 runtime 0x037FB4CC size 0x20 wram ARM. Pool: =0x4000400. */

int fn_02383634(int a1, int a2) {
    int *v2 = (int *)(0x4000400 + 16 * a1);
    int v3 = *v2 & 0x7FFFFFFF;
    int result = a2 & 1;
    if (result)
        v3 |= 0x8000;
    *v2 = v3;
    return result;
}
