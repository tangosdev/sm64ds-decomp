extern int *data_ov004_020beb68;

void _ZN14dScMgMemory2_c11InitPlayersEv(int this)
{
    char *self = (char *)this;
    int n;
    int i;
    int angle;
    char *entry;

    if (data_ov004_020beb68 != 0)
        n = *(int *)((char *)data_ov004_020beb68 + 0xa8);
    else
        n = 0;
    if (n >= 5)
        n = 5;

    entry = self;
    i = 0;
    *(unsigned char *)(self + 0x5409) = (unsigned char)n;
    if (n <= 0)
        return;

    angle = 0;
    do {
        *(unsigned char *)(entry + 0x5398) = 1;
        *(unsigned char *)(entry + 0x539a) = 0;
        *(short *)(entry + 0x5396) = angle;
        i++;
        entry += 0x14;
        angle += 0x10;
    } while (i < n);
}
