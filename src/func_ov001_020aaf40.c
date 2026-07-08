// NONMATCHING: different op / idiom (div=41). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void func_ov001_020aa6b0(void *node, int v);
extern void func_ov001_020aa420(void);
extern int _ZN6Player8HasNoCapEv(void *p);
extern int _ZN6Player15IsCollectingCapEv(void *p);
extern void func_ov001_020aadac(void);
extern void func_ov001_020aaa54(void);

extern unsigned char data_ov001_020ad620;
extern char *data_ov001_020ad634[3];
extern unsigned char BOUNCING_MSG_ARROWS_VISIBLE;
extern unsigned char CURRENT_GAMEMODE;
extern char *PLAYER_ARR;
extern unsigned char data_ov001_020ad624;
extern unsigned char data_ov001_020ad62c[3];

void func_ov001_020aaf40(void)
{
    int i;
    int j;

    if (data_ov001_020ad620 != 0) {
        data_ov001_020ad620 -= 1;
        for (i = 0; i < 3; i++) {
            char *node = data_ov001_020ad634[i];
            while (node) {
                if (*(int*)(node + 0x14) != -1 && *(unsigned char*)(node + 0x1a) != 0) {
                    int v;
                    *(unsigned char*)(node + 0x1a) -= 1;
                    v = *(unsigned char*)(node + 0x1a);
                    if (v == 0) {
                        func_ov001_020aa6b0(node, 1);
                    } else if (v % 10 == 0) {
                        func_ov001_020aa6b0(node, ((v / 10) & 1) == 0 ? 1 : 0);
                    }
                }
                node = *(char**)(node + 0x10);
            }
        }
        if (data_ov001_020ad620 == 0)
            BOUNCING_MSG_ARROWS_VISIBLE = 0;
    }

    if ((int)(CURRENT_GAMEMODE == 1) == 0) {
        char *pl = PLAYER_ARR;
        if (_ZN6Player8HasNoCapEv(pl) && !_ZN6Player15IsCollectingCapEv(pl)) {
            func_ov001_020aadac();
            data_ov001_020ad624 = 1;
            return;
        }
        for (j = 0; j < 3; j++) {
            unsigned char x = data_ov001_020ad62c[j];
            if (x & 2)
                data_ov001_020ad62c[j] = x & ~3;
        }
        func_ov001_020aaa54();
        data_ov001_020ad624 = 0;
        return;
    }
    func_ov001_020aa420();
}
