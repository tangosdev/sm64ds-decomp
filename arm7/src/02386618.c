#pragma opt_propagation off

int fn_02386618(int a1, int a2, int a3, unsigned char *a4) {
    unsigned int v5;
    int *t;
    unsigned char *v10;
    unsigned short *s;
    unsigned short *d;
    unsigned int n;
    int i;
    int v11;
    int v12;
    if (a2 < 0)
        return 0;
    if ((unsigned int)a2 >= *(unsigned int *)(a1 + 0x38))
        return 0;
    t = (int *)a1 + a2;
    v5 = t[0xF];
    *a4 = v5;
    switch (*a4) {
    case 1:
    case 2:
    case 3:
    case 4:
        s = (unsigned short *)((unsigned char *)a1 + (v5 >> 8));
        d = (unsigned short *)(a4 + 2);
        n = 5;
        do {
            *d++ = *s++;
        } while (--n);
        break;
    case 0x10:
        v10 = (unsigned char *)a1 + (v5 >> 8);
        v12 = v10[1];
        v11 = ((unsigned char *)a1)[v5 >> 8];
        if (a3 < v11)
            return 0;
        if (a3 > v12)
            return 0;
        s = (unsigned short *)(v10 + (a3 - v11) * 12 + 2);
        d = (unsigned short *)a4;
        n = 6;
        do {
            *d++ = *s++;
        } while (--n);
        break;
    case 0x11:
        i = 0;
        v10 = (unsigned char *)a1 + (v5 >> 8);
        while (a3 > v10[i]) {
            ++i;
            if (i >= 8)
                return 0;
        }
        s = (unsigned short *)(v10 + i * 12 + 8);
        d = (unsigned short *)a4;
        n = 6;
        do {
            *d++ = *s++;
        } while (--n);
        break;
    default:
        return 0;
    }
    return 1;
}
