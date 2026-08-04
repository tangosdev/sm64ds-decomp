// NONMATCHING: register allocation (div=41). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);

void func_ov006_020fea70(char *o)
{
    int i;
    char *b;
    if (*(unsigned short *)(o + 0x5c1c) == 0) return;
    *(unsigned short *)(o + 0x5c1c) -= 1;
    if (*(short *)(o + 0x5c1c) > 0) return;
    *(unsigned short *)(o + 0x5c1c) = 0;
    for (i = 0; i < 0x30; i++)
    {
        if (*(unsigned char *)(o + i * 0x38 + 0x4f0c) == 0)
        {
            b = o + i * 0x38 + 0x4000;
            *(unsigned char *)(b + 0xf0c) = 1;
            *(unsigned char *)(b + 0xf0e) = 1;
            *(unsigned char *)(b + 0xf0d) = 0;
            *(int *)(b + 0xefc) = 0;
            *(int *)(b + 0xed8) = 0x80000;
            *(int *)(b + 0xedc) = 0x28000;
            *(int *)(b + 0xee8) = 0;
            *(int *)(b + 0xeec) = 0;
            *(unsigned char *)(b + 0xf0f) = 0;
            *(int *)(b + 0xee0) = 0;
            *(int *)(b + 0xee4) = 0;
            *(unsigned short *)(o + i * 0x38 + 0x4f0a) = 0;
            *(int *)(b + 0xf00) = 0;
            *(int *)(b + 0xf04) = 4;
            *(unsigned char *)(o + 0x5c2f) = (unsigned char)(i + 1);
            if (*(unsigned short *)(o + 0x5c20) != 0)
                _ZN5Sound12PlayBank2_2DEj(0x18d);
            else
                *(unsigned short *)(o + 0x5c20) += 1;
            return;
        }
    }
}
