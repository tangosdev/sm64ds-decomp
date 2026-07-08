// NONMATCHING: base materialization / addressing (div=13). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern int func_0203d890(void);

typedef unsigned char u8;
extern u8 IS_PLAYER_ACTIVE[];
extern u8 data_0209fc64[];
extern u8 NUM_PLAYERS_2;

void func_02030dac(void)
{
    int n = func_0203d890();
    int i;
    for (i = 0; i < n; i++) {
        IS_PLAYER_ACTIVE[i] = 1;
        data_0209fc64[i] = (u8)i;
    }
    for (i = n; i < 4; i++) {
        IS_PLAYER_ACTIVE[i] = 0;
        data_0209fc64[i] = (u8)-1;
    }
    NUM_PLAYERS_2 = (u8)n;
}
