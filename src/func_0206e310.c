void *func_0206e310(void *dst, const void *src, unsigned int n)
{
    unsigned char *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;
    while (n != 0) {
        *d++ = *s++;
        n--;
    }
    return dst;
}
