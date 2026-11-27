/* storage 0x023965A8 runtime 0x027C8520 size 0x320 main. Straight-line mixer:
 * folds 17 sound MMIO halfwords into a 45-word state table at
 * *(int *)0x0380FFF4 + 0x4F4. No calls, pool is immediates/MMIO addrs. */

int fn_023965A8(void) {
    int *v0 = (int *)(*(int *)0x0380FFF4 + 0x4F4);
    unsigned int ph;
    
    ph = *(unsigned short *)0x48081B0;
    v0[20] += (unsigned short)(ph & 0xFF);
    ph = *(unsigned short *)0x48081B2;
    v0[19] += (ph << 8) >> 16;
    v0[23] += (unsigned short)(ph & 0xFF);
    ph = *(unsigned short *)0x48081B4;
    v0[22] += (ph << 8) >> 16;
    v0[21] += (unsigned short)(ph & 0xFF);
    ph = *(unsigned short *)0x48081B6;
    v0[24] += (ph << 8) >> 16;
    v0[14] += (unsigned short)(ph & 0xFF);
    ph = *(unsigned short *)0x48081B8;
    ph &= 0xFF;
    v0[18] += (unsigned short)ph;
    ph = *(unsigned short *)0x48081BA;
    ph &= 0xFF;
    v0[8] += (unsigned short)ph;
    ph = *(unsigned short *)0x48081BC;
    v0[12] += (ph << 8) >> 16;
    v0[17] += (unsigned short)(ph & 0xFF);
    ph = *(unsigned short *)0x48081BE;
    v0[15] += (unsigned short)(ph & 0xFF);
    v0[16] += (ph << 8) >> 16;
    ph = *(unsigned short *)0x48081C0;
    ph &= 0xFF;
    v0[3] += (unsigned short)ph;
    ph = *(unsigned short *)0x48081D0;
    v0[30] += (ph << 8) >> 16;
    ph = *(unsigned short *)0x48081D2;
    v0[31] += (unsigned short)(ph & 0xFF);
    v0[32] += (ph << 8) >> 16;
    ph = *(unsigned short *)0x48081D4;
    v0[33] += (unsigned short)(ph & 0xFF);
    v0[34] += (ph << 8) >> 16;
    ph = *(unsigned short *)0x48081D6;
    v0[35] += (unsigned short)(ph & 0xFF);
    v0[36] += (ph << 8) >> 16;
    ph = *(unsigned short *)0x48081D8;
    v0[37] += (unsigned short)(ph & 0xFF);
    v0[38] += (ph << 8) >> 16;
    ph = *(unsigned short *)0x48081DA;
    v0[39] += (unsigned short)(ph & 0xFF);
    v0[40] += (ph << 8) >> 16;
    ph = *(unsigned short *)0x48081DC;
    v0[41] += (unsigned short)(ph & 0xFF);
    v0[42] += (ph << 8) >> 16;
    ph = *(unsigned short *)0x48081DE;
    v0[43] += (unsigned short)(ph & 0xFF);
    {
        int *slot = &v0[44];
        int r = *slot + ((ph << 8) >> 16);
        *slot = r;
        return r;
    }
}
