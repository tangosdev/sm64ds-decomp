/* storage 0x02381D94 runtime 0x037F9C2C size 0xBC wram ARM.
 * Sorted address-ordered insert with neighbour coalescing. No calls,
 * no pool. */

int *fn_02381D94(int *result, int *a2) {
    int *v3;
    int *v2;
    v2 = result;
    v3 = 0;
    while (v2 != 0) {
        if (a2 <= v2)
            break;
        v3 = v2;
        v2 = (int *)v2[1];
    }
    a2[1] = (int)v2;
    *a2 = (int)v3;
    if (v2 != 0) {
        *v2 = (int)a2;
        if ((int *)((char *)a2 + a2[2]) == v2) {
            a2[2] += v2[2];
            v2 = (int *)v2[1];
            a2[1] = (int)v2;
            if (v2 != 0)
                *v2 = (int)a2;
        }
    }
    if (v3 != 0) {
        v3[1] = (int)a2;
        if ((int *)((char *)v3 + v3[2]) == a2) {
            v3[2] += a2[2];
            v3[1] = (int)v2;
            if (v2 != 0)
                *v2 = (int)v3;
        }
    } else {
        return a2;
    }
    return result;
}
