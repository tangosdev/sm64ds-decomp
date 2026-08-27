extern void *func_ov007_020c3df4(int a, void *b);
extern void func_ov007_020c934c(void *a, void *b);

void *func_ov007_020c9388(void *self)
{
    void *r = func_ov007_020c3df4(0, (void *)0x10);
    func_ov007_020c934c(r, self);
    return r;
}
