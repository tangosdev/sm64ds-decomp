//cpp
// NONMATCHING: base materialization / addressing (div=15). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
extern unsigned char CURR_PLAYER_ID;
extern signed char NEXT_LEVEL_ID;
extern int data_020873dc;
extern int data_0208742c;
extern int data_0209b0e8;
extern void func_0200cb58(void *obj, int index);
extern void func_0200c66c(void *self, void *base, int *a, int *b, int *c);
extern void _ZN6Camera11ChangeStateEPNS_5StateE(void *self, void *st);

void _ZN6Camera14GoBehindPlayerEj(char *self, unsigned int j) {
    int slot4, slot8, slotc;
    int *p;
    if (j != CURR_PLAYER_ID) return;
    if (NEXT_LEVEL_ID >= 0) return;
    p = (int*)(self + 0x154);
    *p = *p & 0xfffffaf7;
    func_0200cb58(self, 0xa);
    p = (int*)(self + 0x154);
    *p = *p | 4;
    slot4 = *(int*)(self + 0x13c);
    slotc = 0;
    func_0200c66c(self, (char*)(*(int*)(self + 0x110)) + 0x5c, &slot8, &slot4, &slotc);
    if (slot4 == (int)&data_020873dc) return;
    if (slot4 == (int)&data_0208742c) return;
    _ZN6Camera11ChangeStateEPNS_5StateE(self, &data_0209b0e8);
}
}
