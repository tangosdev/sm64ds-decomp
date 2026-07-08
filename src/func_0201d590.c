//cpp
// NONMATCHING: different op / idiom (div=15). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
typedef unsigned char u8;
extern short CURR_MSG_ID;
extern int MSG_ARR_PTR;
extern int **MSG_FILE_PTR;
extern int data_0209d6fc;
extern int CURR_MSG_PTR;
extern int CURR_MSG_TEXT_CHAR;
extern u8 data_0209d6b0;
extern u8 data_0209d65c;
extern u8 data_0209d674;
extern int _ZN2G213GetBG2CharPtrEv(void);
extern void MultiStore_Int(int val, int dst, int len);
extern void func_0201b6f8(int a);
extern void func_0201b100(int a);

void func_0201d590(void) {
    volatile int li;
    int n;
    int idx = CURR_MSG_ID;
    char *base = (char*)MSG_ARR_PTR;
    CURR_MSG_PTR = (int)(base + idx * 8);
    CURR_MSG_TEXT_CHAR = data_0209d6fc + 0x28 + (*MSG_FILE_PTR)[1] + *(int*)(base + idx * 8);
    func_0201b6f8(1);
    data_0209d65c = 0;
    n = ((data_0209d6b0 + 7) / 8) << 6;
    {
        int p = _ZN2G213GetBG2CharPtrEv() + ((data_0209d674 + 0x280) << 5);
        li = 0;
        MultiStore_Int(li, p, n);
    }
    func_0201b100(1);
    data_0209d674 = data_0209d674 + (u8)(((data_0209d6b0 + 7) / 8) << 1);
}
}
