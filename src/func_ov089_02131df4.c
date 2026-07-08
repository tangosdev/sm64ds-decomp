// NONMATCHING: base materialization / addressing (div=12). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern int _ZN6Player17SetNoControlStateEhih(void* p, unsigned char a, int b, unsigned char d);
extern void _ZN5Sound22LoadAndSetMusic_Layer3Ej(unsigned int a);
extern void func_ov089_0213115c(char* c, int i);
extern void _ZN5Event6SetBitEj(unsigned int a);
extern int SAVE_DATA[];
extern unsigned char NEW_STAR_COLLECTED;

void func_ov089_02131df4(char* c, char* player)
{
    if (SAVE_DATA[1] & (2 << *(int*)(c + 0x444)))
        NEW_STAR_COLLECTED = 0;
    else
        NEW_STAR_COLLECTED = 1;
    SAVE_DATA[1] |= (2 << *(int*)(c + 0x444));

    if (*(int*)(c + 0x444) <= 1) {
        _ZN6Player17SetNoControlStateEhih(player, 3, -1, 0);
        _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x17);
    } else if (*(int*)(c + 0x444) != 7) {
        _ZN6Player17SetNoControlStateEhih(player, 3, -1, 0);
        _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x17);
    }

    func_ov089_0213115c(c, 3);
    *(char**)(c + 0x110) = player;
    {
        int* s = (int*)(*(char**)(c + 0x110) + 0x5c);
        *(int*)(c + 0x5c) = s[0];
        *(int*)(c + 0x60) = s[1];
        *(int*)(c + 0x64) = s[2];
    }
    *(short*)(c + 0x8e) = *(short*)(*(char**)(c + 0x110) + 0x8e);
    *(int*)(c + 0xb0) &= ~0x40000;
    _ZN5Event6SetBitEj(0x1d);
}
