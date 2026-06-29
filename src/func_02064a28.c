extern void func_0205a61c(const void *src, int off, int size);

int func_02064a28(int off, short v, char b)
{
    unsigned char tag = 0xb;
    short val = v;
    int c1, c2;
    func_0205a61c(&tag, off, 1);
    c1 = off + 1;
    func_0205a61c(&val, c1, 2);
    c2 = c1 + 2;
    func_0205a61c(&b, c2, 1);
    return c2 + 1 - off;
}
