extern void MultiCopy_Int(int *dst, int *src, int len);
struct S;
extern struct S *func_ov007_020c2c68(int a, int len, int b);
extern void func_ov007_020c27b4(struct S *s);
extern void func_ov007_020c26fc(struct S *s);
struct S { int f0; int f4; char pad8[0x18]; int f20; int f24; int f28; int f2c; };
struct S *func_ov007_020c2a04(struct S *src, int start, int end)
{
    struct S *d;
    int n;
    n = end - start + 1;
    d = func_ov007_020c2c68(src->f0, n, src->f4);
    *(short*)((char*)d + 8) = n;
    switch (src->f0) {
    case 0:
        MultiCopy_Int((int*)(src->f24) + start, (int*)(d->f24), n << 2);
        MultiCopy_Int((int*)(src->f28) + start, (int*)(d->f28), n << 2);
        break;
    case 1:
        MultiCopy_Int((int*)((char*)src->f2c + start * 0xc), (int*)(d->f2c), n * 0xc);
        break;
    }
    if (src->f4 & 1) {
        MultiCopy_Int((int*)(src->f20) + start, (int*)(d->f20), n << 2);
    }
    func_ov007_020c27b4(d);
    func_ov007_020c26fc(d);
    return d;
}
