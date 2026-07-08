// NONMATCHING: base materialization / addressing (div=13). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern int func_0202044c(char *thiz, int idx);
extern void func_0202043c(char *thiz);

extern int GAME_SPEED_RELATED;

void func_02020768(char *thiz)
{
    int *acc;
    unsigned char *cnt;

    if (*(unsigned char *)(thiz + 0x25) == 0)
        return;

    acc = (int *)(thiz + 0x1c);
    *acc += GAME_SPEED_RELATED;

    if (*(int *)(thiz + 0x1c) < *(int *)(thiz + 0x18))
        return;

    *(int *)(thiz + 0x1c) = 0;

    if (func_0202044c(thiz, *(int *)(thiz + 0x14) + 1) != 0)
        return;

    if (*(unsigned char *)(thiz + 0x24) == 0) {
        func_0202043c(thiz);
        return;
    }

    if (*(unsigned char *)(thiz + 0x24) <= 1) {
        *(unsigned char *)(thiz + 0x25) = 0;
        return;
    }

    cnt = (unsigned char *)(thiz + 0x24);
    *cnt -= 1;
    func_0202043c(thiz);
}
