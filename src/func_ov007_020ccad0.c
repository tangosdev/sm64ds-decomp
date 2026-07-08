// NONMATCHING: base materialization / addressing (div=6). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef struct {
    char pad[0x13];
    unsigned char f0 : 1;
    unsigned char f1 : 1;
    unsigned char f2 : 1;
} Obj;
extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN9ActorBaseC1Ev(void *);
extern void func_02017278(int *);
extern int _ZTV12ActorDerived[];
extern int _ZTV5Scene[];
extern int data_ov007_021032e8[];
extern int data_0208ee14[];
extern int data_ov007_021032b0[];
int *func_ov007_020ccad0(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(0x64);
    if (p) {
        _ZN9ActorBaseC1Ev(p);
        p[0] = (int)_ZTV12ActorDerived;
        p[0] = (int)_ZTV5Scene;
        ((Obj *)p)->f0 = 1;
        ((Obj *)p)->f2 = 1;
        p[0] = (int)data_ov007_021032e8;
        p[0x14] = (int)data_0208ee14;
        p[0x14] = (int)data_ov007_021032b0;
        func_02017278(p + 0x15);
    }
    return p;
}
