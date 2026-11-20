unsigned short *fn_0238E57C(unsigned short *result, unsigned short *a2) {
    int i;
    for (i = 0; i < 3; i++) {
        unsigned short v;
        v = *a2;
        *result = (unsigned short)(((int)v >> 8) | ((v & 0xFF) << 8));
        result++;
        a2++;
    }
    return result;
}
