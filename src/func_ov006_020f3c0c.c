extern int *data_ov004_020beb68;
void _ZN13dScMgMemory_c11InitPlayersEv(char *self)
{
    int n;
    int i;
    int angle;
    char *entry;

    if (data_ov004_020beb68 != 0)
        n = *(int *)((char *)data_ov004_020beb68 + 0xa8);
    else
        n = 0;
    if (n >= 3)
        n = 3;

    entry = self;
    i = 0;
    *(unsigned char *)(self + 0x533b) = (unsigned char)n;
    if (n <= 0)
        return;

    angle = 0;
    do {
        *(unsigned char *)(entry + 0x52d8) = 1;
        *(unsigned char *)(entry + 0x52da) = 0;
        *(short *)(entry + 0x52d6) = angle;
        i++;
        entry += 0x14;
        angle += 0x10;
    } while (i < n);
}
