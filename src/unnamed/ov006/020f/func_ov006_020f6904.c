// NONMATCHING: different op / idiom (div=29). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);

void func_ov006_020f6904(char *c)
{
    int cntA = 0;
    int cntB = 0;
    for (int i = 0; i < 8; i++) {
        unsigned char id = *(unsigned char*)(c + 0x53fd + i);
        if (id == 0xff) continue;
        if (*(unsigned char*)(c + id * 24 + 0x51bb) == 0) {
            *(unsigned char*)(c + 0x53fd + i) = 0xff;
        } else if (*(unsigned char*)(c + id * 24 + 0x51bc) == 1) {
            *(unsigned char*)(c + id * 24 + 0x51bc) = 3;
            *(unsigned char*)(c + 0x53fd + i) = 0xff;
            cntB++;
        }
        cntA++;
    }
    if (cntB == 1) _ZN5Sound12PlayBank2_2DEj(0x148);
    else if (cntB == 2) _ZN5Sound12PlayBank2_2DEj(0x145);
    else if (cntB >= 3) _ZN5Sound12PlayBank2_2DEj(0x146);
    if (cntA != 0) return;
    *(int*)(c + 0x53d8) = 6;
    *(short*)(c + 0x53ec) = *(unsigned char*)(c + 0x540a) * 20 + 0x50;
}
