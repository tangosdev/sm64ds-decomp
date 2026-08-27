extern int func_0206736c(unsigned int idx, unsigned int addr, unsigned int size);
extern void *data_020a9db8;

int func_02067604(int *c, char *list)
{
    unsigned char idx;
    unsigned short *shortArr;
    int sum;
    int r;

    shortArr = (unsigned short *)((char *)c + 0xc);

    sum = 0;
    if (list == 0) return 0;

    idx = 0;
    do {
        c[idx] = sum;
        sum += *(int *)(list + idx * 16 + 0x14);
        idx = idx + 1;
    } while (idx < 3);

    idx = 0;
    shortArr[0] = 0;
    do {
        char *e = list + 0xc + idx * 16;
        unsigned int divisor = *(unsigned int *)((char *)data_020a9db8 + 0x1318);
        unsigned int esize = *(unsigned int *)(e + 8);
        unsigned int n = (esize + divisor - 1) / divisor;
        unsigned short sb = (unsigned short)(shortArr[idx] + (unsigned short)n);
        unsigned int addr = *(unsigned int *)(e + 4);

        r = func_0206736c(idx, addr, esize);
        if (r == 0) return 0;

        if (idx < 2) {
            shortArr[idx + 1] = sb;
        } else {
            *(unsigned short *)((char *)c + 0x12) = sb;
        }
        idx = idx + 1;
    } while (idx < 3);

    return 1;
}
