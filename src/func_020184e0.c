struct Obj { char pad[0x20]; unsigned int cur; unsigned int end; };

extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(unsigned int a, unsigned int b);
extern int func_02018d48(void *a, unsigned int b, unsigned int c);
extern void func_0205d4cc(void *a);
extern void func_02018770(void);
extern int func_02018568(unsigned int a);
extern void _ZN4CP1514FlushDataCacheEjj(unsigned int a, unsigned int b);

int func_020184e0(struct Obj *self, unsigned int buf, unsigned int n)
{
    int r;
    unsigned int len = self->end - self->cur;
    if (len > n) len = n;
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(buf, len);
    r = func_02018d48(self, buf, len);
    func_0205d4cc(self);
    func_02018770();
    if (len != r) return 0;
    func_02018568(buf);
    _ZN4CP1514FlushDataCacheEjj(buf, len);
    return len;
}
