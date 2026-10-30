int fn_02381704(int result, int a2) {
    int tail = *(int *)(result + 100);
    if (tail == 0)
        *(int *)(result + 96) = a2;
    else
        *(int *)(*(int *)(result + 96) + 16) = a2;
    *(int *)(a2 + 12) = tail;
    *(int *)(a2 + 16) = 0;
    *(int *)(result + 100) = a2;
    return result;
}
