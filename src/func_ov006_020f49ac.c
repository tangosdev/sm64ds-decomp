extern void _ZN13dScMgMemory_c11InitPlayersEv(char *c);
void _ZN13dScMgMemory_c11RoundRevealEv(char *c)
{
    int i;
    char *e;
    unsigned short *t;
    if (*(unsigned short *)(c + 0x5300 + 0x22) != 0) {
        t = (unsigned short *)(((int)c + 0x5322));
        *t = *t - 1;
        if (*(short *)(c + 0x5300 + 0x22) > 0)
            return;
        _ZN13dScMgMemory_c11InitPlayersEv(c);
        return;
    }
    e = c;
    for (i = 0; i < 0xc; i++) {
        *(unsigned char *)(e + 0x5000 + 0x1bc) = 2;
        e += 0x18;
    }
    *(int *)(c + 0x5000 + 0x318) = 0;
    *(int *)(c + 0x5000 + 0x314) = 2;
}
