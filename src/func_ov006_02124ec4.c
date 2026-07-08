// NONMATCHING: missing logic (ROM does more) (div=2). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern unsigned char data_020a0e40;
extern unsigned char TOUCH_INPUT_ARR[];
extern unsigned char data_020a0de9[];
extern int data_ov006_0213fd44[];
extern int data_ov004_020bf9ec;
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);

void func_ov006_02124ec4(char *sl)
{
    unsigned int i = data_020a0e40;
    int ok = 0;
    int sb;
    if (TOUCH_INPUT_ARR[i * 4] != 0) {
        if (data_020a0de9[i * 4] != 0) ok = 1;
    }
    if (ok == 0) return;

    for (sb = 0; sb < 2; sb++) {
        unsigned char *b = &TOUCH_INPUT_ARR[(unsigned int)data_020a0e40 * 4];
        int v = (int)b[2] - (data_ov006_0213fd44[sb] >> 12);
        int w = (int)b[3] - 0x60;
        if (v < -0x10) continue;
        if (v > 0x10) continue;
        if (w < -0x18) continue;
        if (w > 0x18) continue;
        *(int *)(sl + 0x51c0) = sb;
        *(int *)(sl + 0x51c4) = sb ^ 1;
        *(unsigned char *)(sl + 0x51ce) = 0;
        *(int *)(sl + 0x51b8) += 1;
        data_ov004_020bf9ec = 1;
        _ZN5Sound12PlayBank2_2DEj(0x153);
    }
}
