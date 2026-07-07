struct Buf {
    int len;
    char *ptr;
};

void func_02057d30(struct Buf *b, char *src, int n)
{
    int i;
    int ncopy;
    char *dst;

    if (n <= 0) {
        return;
    }
    ncopy = b->len;
    if (n < ncopy) {
        ncopy = n;
    }
    dst = b->ptr;
    for (i = 0; i < ncopy; i++) {
        dst[i] = src[i];
    }
    b->len -= ncopy;
    b->ptr += n;
}