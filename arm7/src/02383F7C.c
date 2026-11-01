extern void fn_02384114(int *);

int fn_02383F7C(int *a1, int a2)
{
    unsigned char *p;
    int result;
    a1[4] = 0xFFFE9680;
    *(unsigned char *)((char *)a1 + 2) = 0;
    a1[13] = a2;
    fn_02384114(a1 + 10);
    p = (unsigned char *)((char *)a1 + 3);
    *p |= 2;
    result = (*p & ~1) | 1;
    *p = result;
    return result;
}
