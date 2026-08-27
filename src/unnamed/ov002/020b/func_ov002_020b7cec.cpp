//cpp
// func_ov002_020b7cec at 0x020b7d9c
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* thiz);
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void _ZN7fBase_c18MarkForDestructionEv(void* thiz);
extern int data_02092138;

int func_ov002_020b7cec(char* c)
{
    if (data_02092138 > *(int*)(c + 0x60)
        || _ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x144)
        || !DecIfAbove0_Short((unsigned short*)(c + 0x100))
        || (*(int*)(c + 0xb0) & 8))
    {
        _ZN7fBase_c18MarkForDestructionEv(c);
        return 1;
    }
    return 1;
}
}
