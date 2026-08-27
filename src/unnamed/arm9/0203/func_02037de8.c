struct P2 { int a, b; };

void func_02037de8(int *dst, int *src)
{
    *(struct P2*)dst = *(struct P2*)src;
    dst[2] = src[2];
    dst[3] = src[3];
    dst[4] = src[4];
}
