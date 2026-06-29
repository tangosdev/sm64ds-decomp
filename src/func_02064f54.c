extern void func_0205a61c(const void *src, int off, int size);

int func_02064f54(int off, short b, int c, int d, int e)
{
    unsigned char tag = 2;
    short val = b;
    int c1, c2, c3, c4;
    func_0205a61c(&tag, off, 1);
    c1 = off + 1;
    func_0205a61c(&val, c1, 2);
    c2 = c1 + 2;
    func_0205a61c(&c, c2, 1);
    c3 = c2 + 1;
    func_0205a61c(&d, c3, 2);
    c4 = c3 + 2;
    func_0205a61c(&e, c4, 2);
    return c4 + 2 - off;
}
