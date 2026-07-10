// NONMATCHING: different op / idiom (div=9). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef int Fix12i;
extern void _ZN13SharedFilePtr7ReleaseEv(char* p);
extern void _ZN5Actor11UntrackStarERa(char* c, signed char* p);
extern char* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int a, Fix12i x, Fix12i y, Fix12i z);
extern char data_ov002_0210d9a8;
extern char* data_ov002_020ff06c[];
extern char* data_ov002_020ff060[];

int _ZN4Coin16CleanupResourcesEv(char* c)
{
    char* o;
    int b = (int)(*(unsigned short*)(c + 0xc) == 0x121);
    if (b != 0) _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210d9a8);
    if (*(int*)(c + 0x3a0) == 2) {
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_020ff06c[*(int*)(c + 0x3a0)]);
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_020ff060[*(int*)(c + 0x3a0)]);
    }
    _ZN5Actor11UntrackStarERa(c, (signed char*)(c + 0x3ac));
    o = _ZN5Actor10FindWithIDEj(*(unsigned int*)(c + 0xd4));
    if (o != 0) {
        int b2 = (int)(*(unsigned short*)(o + 0xc) == 0x4f);
        if (b2 != 0) {
            if (*(unsigned char*)(o + 0xd6) != 0) (*(unsigned char*)(o + 0xd6))--;
        }
    }
    if (*(unsigned short*)(c + 0x3a8) != 0) return 1;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd2, *(Fix12i*)(c + 0x5c), *(Fix12i*)(c + 0x60) + 0x28000, *(Fix12i*)(c + 0x64));
    return 1;
}
