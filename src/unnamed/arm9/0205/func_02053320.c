void func_02053320(int s, int *a, int *b, int *out)
{
    out[0] = b[0] + (int)(((long long)s * a[0]) >> 12);
    out[1] = b[1] + (int)(((long long)s * a[1]) >> 12);
    out[2] = b[2] + (int)(((long long)s * a[2]) >> 12);
}
