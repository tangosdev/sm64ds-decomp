// NONMATCHING: base materialization / addressing (div=7). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct Actor;
struct CylinderClsn;
extern struct Actor *_ZN5Actor13ClosestPlayerEv(void);

void _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(struct Actor *self, struct CylinderClsn *clsn)
{
    struct Actor *p;
    *(int*)((char*)clsn+0x18) &= ~2;
    p = _ZN5Actor13ClosestPlayerEv();
    if (p == 0) return;
    if (*(unsigned char*)((char*)p+0x6fb) != 0)
        *(int*)((char*)clsn+0x18) |= 2;
}
