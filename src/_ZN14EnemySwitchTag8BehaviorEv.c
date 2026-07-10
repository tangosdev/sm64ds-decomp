// NONMATCHING: different op / idiom (div=20). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned int u32;
typedef unsigned char u8;

extern int _ZNK6Camera12IsUnderwaterEv(void);
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3j(u32 a, u32 b, u32 c, void* v, u32 e);
extern int data_ov002_02110aec;
extern int data_0209b4ac;
extern int data_0209f318;
extern int data_ov002_0210b498[];

int _ZN14EnemySwitchTag8BehaviorEv(char* c)
{
    if (data_ov002_02110aec != 0) return 1;

    if (*(u32*)(c + 8) >= 1 && *(u32*)(c + 8) <= 4) {
        int a = data_0209b4ac;
        data_0209f318;
        if (a != 0x32 && a != 0x33 && a != 0x34) {
            if (_ZNK6Camera12IsUnderwaterEv()) return 1;
        } else {
            return 1;
        }
    }

    *(u32*)(c + 0xd4) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
        *(u32*)(c + 0xd4), 3, data_ov002_0210b498[*(u32*)(c + 8)], c + 0x74, 0);
    return 1;
}
