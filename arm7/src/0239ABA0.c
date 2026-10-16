/* storage 0x0239ABA0 runtime 0x027CCB18 size 0x64 main. */

extern int fn_02380884(unsigned int key);
extern int fn_023808C0(int token);
extern void fn_0239A6FC(unsigned int index);
extern void fn_02394D84(int a, char *b, int c);

int fn_0239ABA0(int arg1, int arg2) {
    int token;

    token = fn_02380884(0x1000000);
    if (*(unsigned short *)(*(char **)0x0380FFF4 + 0x100 + 0x78) == 0)
        fn_0239A6FC(0);
    fn_02394D84(arg1, *(char **)0x0380FFF4 + 0x170, arg2);
    return fn_023808C0(token);
}
