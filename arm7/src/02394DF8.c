extern char *unk_380FFF4;
extern int fn_02394B88(void *list, unsigned short *node);
extern void fn_02381C08(int a, int b, unsigned short *node);

int fn_02394DF8(void *list, unsigned short *node) {
    int *cb;
    int rc;

    cb = (int *)(unk_380FFF4 + 0x140);
    if (node[5] != 0xBF1D)
        return 1;
    rc = fn_02394B88(list, node);
    if (rc == 0) {
        switch (cb[0]) {
        case 0:
            fn_02381C08(cb[1], cb[2], node);
            break;
        case 1:
            ((void (*)(unsigned short *))cb[2])(node);
            break;
        default:
            break;
        }
    }
    return rc;
}
