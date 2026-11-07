/* storage 0x0239AAA8 runtime 0x027CCA20 size 0x78 main. */

extern int fn_02380884(unsigned int key);
extern int fn_023808C0(int token);
extern void fn_0239A650(unsigned int index);

int fn_0239AAA8(unsigned short *arg) {
    char *entry;
    int token;
    unsigned int index;
    unsigned short *count;

    index = arg[1];
    entry = (char *)(*(int *)(*(char **)0x0380FFF4 + 0x2E0) + index * 0x1C);
    token = fn_02380884(0x1000000);
    if (*(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0x14) == 1
        && *(unsigned short *)(entry + 0x16) == 1)
        fn_0239A650(index);
    count = (unsigned short *)(entry + 0x16);
    *count = (unsigned short)(*count - 1);
    return fn_023808C0(token);
}
