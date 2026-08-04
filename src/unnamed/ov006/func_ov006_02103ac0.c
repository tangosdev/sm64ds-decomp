// NONMATCHING: different op / idiom (div=34). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);

void func_ov006_02103ac0(char *c)
{
    int i;
    unsigned short *q;
    if (*(unsigned short *)(c + 0x566c) == 0) return;
    q = (unsigned short *)(c + 0x566c);
    *q = *q - 1;
    if (*(short *)(c + 0x566c) > 0) return;
    *(unsigned short *)(c + 0x566c) = 0;
    for (i = 0; i < 0x30; i++) {
        char *b = c + i * 64;
        if (*(unsigned char *)(b + 0x4698) == 0) {
            *(unsigned char *)(b + 0x4698) = 1;
            *(unsigned char *)(b + 0x469a) = 1;
            *(unsigned char *)(b + 0x4699) = 0;
            *(int *)(b + 0x4684) = 0;
            *(int *)(b + 0x4660) = 0x80000;
            *(int *)(b + 0x4664) = 0x28000;
            *(int *)(b + 0x4670) = 0;
            *(int *)(b + 0x4674) = 0;
            *(unsigned char *)(b + 0x469b) = 0;
            *(int *)(b + 0x4668) = 0;
            *(int *)(b + 0x466c) = 0;
            *(short *)(b + 0x4696) = 0;
            *(short *)(b + 0x4690) = 0;
            *(short *)(b + 0x4692) = 0;
            *(unsigned char *)(b + 0x469c) = 0;
            *(unsigned char *)(b + 0x469d) = 0;
            *(unsigned char *)(b + 0x469e) = 0;
            *(int *)(b + 0x4688) = 0;
            *(int *)(b + 0x468c) = 4;
            *(unsigned char *)(c + 0x5676) = (unsigned char)(i + 1);
            if (*(unsigned short *)(c + 0x5670) != 0) {
                _ZN5Sound12PlayBank2_2DEj(0x19d);
            } else {
                *(unsigned short *)(c + 0x5670) += 1;
            }
            return;
        }
    }
}
