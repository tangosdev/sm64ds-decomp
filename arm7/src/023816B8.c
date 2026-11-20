/* storage 0x023816B8 runtime 0x037F9550 size 0x28 wram. No pool, no calls. */

int fn_023816B8(int *a1) {
    int *node;
    int next;

    node = (int *)a1[24];
    next = node[4];
    if (next == 0)
        a1[25] = 0;
    else
        *(int *)(next + 12) = 0;
    a1[24] = next;
    return (int)node;
}
