extern void func_ov006_020d5e3c(void *a);

#pragma opt_strength_reduction off
#pragma opt_common_subs off

void func_ov006_020d7604(void *a)
{
    unsigned char *base = (unsigned char *)a;
    int ca = 0;
    int cb = 0;
    int cc = 0;
    int i;

    for (i = 0; i < 0x70; i++) {
        unsigned char *p = base + i * 64;
        if (p[0x4698] != 0) {
            if (base[0x62f8] == 1) {
                if (p[0x4696] != 0) {
                    int v;
                    int q;
                    p[0x469b] = 3;
                    v = ca;
                    q = 0;
                    *(unsigned short *)(p + 0x4690) = (unsigned short)(cc * 8);
                    while (v >= 5) { v -= 5; q++; }
                    ((unsigned char *)(int)(((long long)(int)(base + i * 64))))[0x469c] = (unsigned char)(q * 10 + v);
                    ca++;
                    cc++;
                } else {
                    int v;
                    int q;
                    unsigned char *d = (unsigned char *)(int)(((long long)(int)(p + 0x469c)));
                    *d = (unsigned char)cb;
                    p[0x469b] = 3;
                    v = cb;
                    *(unsigned short *)(p + 0x4690) = (unsigned short)(cc * 8);
                    q = 0;
                    while (v >= 5) { v -= 5; q++; }
                    *d = (unsigned char)(q * 10 + v + 5);
                    cb++;
                    cc++;
                }
            } else if (base[0x62f8] != 0) {
                p[0x469c] = (unsigned char)cc;
                p[0x469b] = 3;
                *(unsigned short *)(p + 0x4690) = (unsigned short)(cc * 8);
                cc++;
            } else {
                if (base[0x62f5] == p[0x4696] && p[0x4697] == 6) {
                    p[0x469c] = (unsigned char)cc;
                    p[0x469b] = 3;
                    *(unsigned short *)(p + 0x4690) = (unsigned short)(cc * 8);
                    cc++;
                }
            }
        }
    }
    func_ov006_020d5e3c(a);
}
