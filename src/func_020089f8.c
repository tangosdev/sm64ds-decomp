// NONMATCHING: register allocation (div=8). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void Vec3_Sub(int *dest, const int *src1, const int *src2);

void func_020089f8(int *obj)
{
    int *ptr = *(int **)((char *)obj + 0x110);
    if (ptr == 0)
        return;

    obj[0x98/4] = *(int *)((char *)ptr + 0x5c);
    obj[0x9c/4] = *(int *)((char *)ptr + 0x60);
    obj[0xa0/4] = *(int *)((char *)ptr + 0x68);

    int tmp[3];
    Vec3_Sub(tmp, (int *)((char *)obj + 0x80), (int *)((char *)obj + 0x98));

    obj[0xa4/4] = tmp[0];
    obj[0xa8/4] = tmp[1];
    obj[0xac/4] = tmp[2];
    obj[0x130/4] = 0;
}
