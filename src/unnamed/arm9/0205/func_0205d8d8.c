void func_0205d8d8(int node, int list)
{
    int a;
    int b;
    int p;
    int n;
    a = *(int*)node;
    b = *(int*)(node + 4);
    if (a != 0) *(int*)(a + 4) = b;
    if (b != 0) *(int*)b = a;
    p = list;
    n = *(int*)(list + 4);
    while (n != 0) {
        p = n;
        n = *(int*)(n + 4);
    }
    *(int*)(p + 4) = node;
    *(int*)node = p;
    *(int*)(node + 4) = 0;
}
