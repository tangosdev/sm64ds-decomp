int fn_023816E0(int result, int a2) {
    int next = *(int *)(a2 + 16);
    int prev = *(int *)(a2 + 12);
    if (next == 0)
        *(int *)(result + 100) = prev;
    else
        *(int *)(next + 12) = prev;
    if (prev == 0)
        *(int *)(result + 96) = next;
    else
        *(int *)(prev + 16) = next;
    return result;
}
