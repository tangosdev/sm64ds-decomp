struct Buf {
    int len;
    char *ptr;
};

void func_02057d94(struct Buf *b, char val, int n)
{
    int copy;
    int i;
    char *dst;

    if (n <= 0) {
        return;
    }
    copy = b->len;
    if (n < copy) {
        copy = n;
    }
    if (copy > 0) {
        dst = b->ptr;
        for (i = 0; i < copy; i++) {
            dst[i] = val;
        }
        b->len -= copy;
    }
    b->ptr += n;
}