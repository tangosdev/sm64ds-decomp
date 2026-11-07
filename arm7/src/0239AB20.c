/* storage 0x0239AB20 runtime 0x027CCA98 size 0x80 main. */

extern int fn_02380884(unsigned int key);
extern int fn_023808C0(int token);
extern void fn_0239A6FC(unsigned int index);

int fn_0239AB20(unsigned short *arg) {
    char *entry;
    int token;
    unsigned int index;
    unsigned short *count;
    unsigned short result;

    index = arg[1];
    entry = (char *)(*(int *)(*(char **)0x0380FFF4 + 0x2E0) + index * 0x1C);
    token = fn_02380884(0x1000000);
    if (*(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0x14) == 1
        && *(unsigned short *)(entry + 0x16) == 0)
        fn_0239A6FC(index);
    count = (unsigned short *)(entry + 0x16);
    *count = (unsigned short)(*count + 1);
    fn_023808C0(token);
    result = *(unsigned short *)(entry + 0x1A);
    *(unsigned short *)(entry + 0x18) = result;
    return result;
}
