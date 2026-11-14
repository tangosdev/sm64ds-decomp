extern short dat_03805860[];

int fn_02383E00(int *a1, int a2)
{
    if (a2 != 0) {
        switch (*(unsigned char *)((char *)a1 + 2)) {
        case 0:
            a1[4] = -((-a1[4] * *(unsigned char *)((char *)a1 + 0x1C)) >> 8);
            if (a1[4] == 0)
                *(unsigned char *)((char *)a1 + 2) = 1;
            break;
        case 1: {
            int v2 = dat_03805860[*(unsigned char *)((char *)a1 + 0x1D)] << 7;
            a1[4] -= *(unsigned short *)((char *)a1 + 0x1E);
            if (a1[4] <= v2) {
                a1[4] = v2;
                *(unsigned char *)((char *)a1 + 2) = 2;
            }
            break;
        }
        case 2:
            break;
        case 3:
            a1[4] -= *(unsigned short *)((char *)a1 + 0x20);
            break;
        }
    }
    return a1[4] >> 7;
}
