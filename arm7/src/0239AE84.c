/* storage 0x0239AE84 runtime 0x027CCDFC size 0x90 main. */

extern int fn_02381600(int a, int b, int c);
extern void fn_02394B88(int a, int b);
extern void fn_02394D84(int a, char *b, int c);
extern void fn_0239477C(unsigned int a, unsigned int b);

void fn_0239AE84(int arg1, int arg2) {
    char *base;

    base = *(char **)0x0380FFF4;
    if (*(unsigned short *)(base + 0x100 + 0xC0) != 0) {
        fn_02394D84(arg1, base + 0x1B8, arg2);
        fn_0239477C(2, 0x13);
    } else if (fn_02381600(*(int *)(base + 0x2C8), arg2, 0) != 0) {
        fn_02394B88(arg1, arg2);
    } else {
        fn_02394D84(arg1, *(char **)0x0380FFF4 + 0x1B8, arg2);
    }
}
