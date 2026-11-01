extern int *dat_27D7A84[];

int fn_0238E500(unsigned short *a1) {
    unsigned short *v1;
    unsigned short s0;
    unsigned short s1;
    unsigned short s2;
    v1 = *(unsigned short **)dat_27D7A84[388];
    s0 = (unsigned short)(((int)v1[110] >> 8) | ((v1[110] & 255) << 8));
    if (*a1 == s0) {
        s1 = (unsigned short)(((int)v1[111] >> 8) | ((v1[111] & 255) << 8));
        if (a1[1] == s1) {
            s2 = (unsigned short)(((int)v1[112] >> 8) | ((v1[112] & 255) << 8));
            if (a1[2] == s2)
                return 1;
        }
    }
    return 0;
}
