//cpp
// NONMATCHING: different op / idiom (div=7). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" unsigned char CURRENT_GAMEMODE;
extern "C" unsigned char data_02075998[][7];
struct OvTbl { unsigned int t[7]; };
extern "C" OvTbl ACTOR_BANK_OVL_MAP[];

enum OvId { OV_3C = 0x3c, OV_62 = 0x62, OV_66 = 0x66 };

void LoadOrUnloadObjectOverlays(void (*fn)(int), int idx) {
    unsigned int i;
    int b = (CURRENT_GAMEMODE == 1);
    if (b != 0) return;
    for (i = 0; i < 7; i++) {
        b = (CURRENT_GAMEMODE == 1);
        if (b == 0) break;
        if (i == 6) {
            unsigned char v = data_02075998[idx][i];
            if (v != 0) {
                fn(ACTOR_BANK_OVL_MAP[i].t[v]);
            }
        }
    }
    if (idx == 0x24 || idx == 0x26 || idx == 0x28) {
        fn(OV_3C);
        return;
    }
    b = (CURRENT_GAMEMODE == 1);
    if (b != 0) {
        fn(OV_62);
        fn(OV_66);
    }
}
