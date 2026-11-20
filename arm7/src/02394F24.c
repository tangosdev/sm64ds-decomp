#pragma scheduling off
#pragma opt_propagation off

extern char *unk_380FFF4;
extern void *fn_02394E7C(void *list, int n);
extern void fn_02394FC4(int bit);
extern int fn_02380884(int key);
extern void fn_023808C0(int token);
extern void fn_0239AE84(void *list, void *node);

void fn_02394F24(void) {
    char *base = unk_380FFF4;
    void *node;
    int token;
    unsigned short flag;

    if (*(unsigned short *)(base + 0x300 + 0xAC) == 0)
        return;
    node = fn_02394E7C(base + 0x14C, 0x12);
    if (node == 0) {
        fn_02394FC4(1);
        return;
    }
    ((unsigned short *)node)[6] = 0x186;
    ((unsigned short *)node)[7] = 1;
    token = fn_02380884(0x1000000);
    {
        char **pp;
        int zero;

        pp = &unk_380FFF4;
        flag = *(unsigned short *)(*pp + 0x300 + 0xAC);
        ((unsigned short *)node)[8] = flag;
        zero = 0;
        *(unsigned short *)(*pp + 0x300 + 0xAC) = (unsigned short)zero;
    }
    fn_023808C0(token);
    fn_0239AE84(unk_380FFF4 + 0x14C, node);
}
