/* MSL fabs: clear the sign bit in x's high word. */
double fabs(double x) {
    int a = (int)(&x);
    int *p = (int *)(a + 4);
    *p &= 0x7fffffff;
    return x;
}
