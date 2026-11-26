/* MSL fabs: clear the sign bit in x's high word. */
double fabs(double x) {
    ((int *)&x)[1] &= 0x7fffffff;
    return x;
}
