#pragma scheduling off

extern char *unk_380FFF4;
extern void *fn_02381C7C(int a, int b, void *payload);
extern int fn_02394C70(void *list, void *node);

void *fn_02394E7C(void *list, void *node) {
    int *cb;
    void *payload;
    void *got;

    cb = (int *)(unk_380FFF4 + 0x140);
    if (node == 0)
        return 0;
    payload = (char *)node + 12;
    switch (cb[0]) {
    case 0:
        got = fn_02381C7C(cb[1], cb[2], payload);
        break;
    case 1:
        got = ((void *(*)(void *))cb[1])(payload);
        break;
    default:
        break;
    }
    if (got == 0)
        return 0;
    ((unsigned short *)got)[5] = 0xBF1D;
    ((unsigned short *)got)[4] = 0;
    fn_02394C70(list, got);
    return got;
}
