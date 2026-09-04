extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);

void _ZN14dScMgMemory2_c14RoundShowCardsEv(char *c)
{
    unsigned int off_bb;
    int cntA = 0;
    int cntB = 0;
    int i;
    int ff = 0xff;
    int three = 3;
    int off_ids = 0x53fd;
    int off_bc = 0x51bc;
    int stride = 0x18;

    for (i = 0; i < 8; i++) {
        unsigned char *slot = (unsigned char *)((c + i) + off_ids);
        unsigned char id = *slot;
        if (id == 0xff)
            continue;
        {
            int base = (int)c + id * stride;
            off_bb = 0x51bb;
            if (*(unsigned char *)(base + off_bb) != 0) {
                unsigned char *fp = (unsigned char *)(base + off_bc);
                if (*fp == 1) {
                    *fp = (unsigned char)three;
                    *slot = (unsigned char)ff;
                    cntB++;
                }
            } else {
                *slot = (unsigned char)ff;
            }
            cntA++;
        }
    }

    if (cntB == 1)
        _ZN5Sound12PlayBank2_2DEj(0x148);
    else if (cntB == 2)
        _ZN5Sound12PlayBank2_2DEj(0x145);
    else if (cntB >= 3)
        _ZN5Sound12PlayBank2_2DEj(0x146);
    if (cntA != 0)
        return;
    *(int *)(c + 0x53d8) = 6;
    *(short *)(c + 0x53ec) = *(unsigned char *)(c + 0x540a) * 20 + 0x50;
}
