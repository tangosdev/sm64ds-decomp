// NONMATCHING: different op / idiom (div=16). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern int _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void*, int, int, int, unsigned);
extern int RandomIntInternal(int* seed);
extern int data_ov066_0211ae2c[];
extern int data_ov066_0211ae9c[];
extern int data_ov066_0211ae3c[];
extern int data_ov066_0211aebc[];
extern int RNG_STATE;

void func_ov066_02116390(void* thiz)
{
    char* c = (char*)thiz;
    DecIfAbove0_Short((unsigned short*)(c + 0x66c));
    if (*(unsigned short*)(c + 0x66c) != 0)
        return;
    if (*(unsigned char*)(c + 0x66e) == 0) {
        if (*(int*)(c + 0x49c) == 2)
            _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x448, data_ov066_0211ae2c[1], 0x40000000, 0x1000, 0);
        else
            _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x448, data_ov066_0211ae9c[1], 0x40000000, 0x1000, 0);
        *(unsigned short*)(c + 0x66c) = (((unsigned int)RandomIntInternal(&RNG_STATE) >> 8) & 0xf) * 2 + 0x32;
    } else {
        if (*(int*)(c + 0x49c) == 2)
            _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x448, data_ov066_0211ae3c[1], 0x40000000, 0x1000, 0);
        else
            _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x448, data_ov066_0211aebc[1], 0x40000000, 0x1000, 0);
        *(unsigned short*)(c + 0x66c) = 8;
        *(unsigned char*)(((int)c + 0x66e) & 0xFFFFFFFFFFFFFFFF) ^= 1;
    }
}
