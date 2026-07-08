// NONMATCHING: different op / idiom (div=15). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void func_0203da2c(int v);
extern void func_02030500(void);
extern void func_02019a58(void);
extern void func_02012790(int v);
extern void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int v);
extern void func_020132d8(void);
extern void func_0201ffcc(void);
extern void func_020199a4(void);
extern unsigned char DAT_02092778;
extern int DP_STATE;
extern int data_0209277c;
extern unsigned char data_0209e64c;
extern unsigned char data_0209b480;

void func_02030790(void)
{
    DP_STATE = 6;
    if (DAT_02092778 == 0)
        return;
    func_0203da2c(0);
    func_02030500();
    data_0209277c = 0;
    func_02019a58();
    if (data_0209e64c != 0 && data_0209b480 != 0) {
        func_02012790(0x123);
        _ZN5Sound22StopLoadedMusic_Layer1Ej(4);
        func_020132d8();
        func_0201ffcc();
        func_020199a4();
    }
    return;
}
