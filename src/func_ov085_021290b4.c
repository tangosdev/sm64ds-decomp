// NONMATCHING: extra logic (you do more) (div=23). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern signed char LEVEL_ID;
extern int func_020138dc(void);
extern int func_02013a44(void);
extern unsigned short ObjectMessageIDToActualMessageID(short);

unsigned short func_ov085_021290b4(char* c) {
    int* r5 = *(int**)(c + 0x1f8);
    int r4 = *(unsigned short*)(c + 0x208);
    if (LEVEL_ID == 0x32) {
        int v = func_020138dc();
        if (v != 0) {
            if (v == 0x1c) return 0x134;
            return 0x133;
        }
        return ObjectMessageIDToActualMessageID((short)r4) + r5[2];
    }
    if (*(unsigned char*)(c + 0x20b) == 1) {
        if (func_02013a44() != 0) {
            return ObjectMessageIDToActualMessageID((short)(r5[2] + 0xb0a));
        }
    }
    return ObjectMessageIDToActualMessageID((short)r4) + r5[2];
}
