extern char *unk_380FFF4;
extern int fn_02380884(int key);
extern void fn_023808C0(int token);
extern void fn_0239477C(int a, int b);

void fn_02394FC4(int bit) {
    int token;
    unsigned short *flag;

    token = fn_02380884(0x1000000);
    flag = (unsigned short *)(unk_380FFF4 + 0x3AC);
    *flag = (unsigned short)(*flag | bit);
    fn_023808C0(token);
    fn_0239477C(2, 0x15);
}
