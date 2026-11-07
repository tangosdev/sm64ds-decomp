/* storage 0x02396DD8 runtime 0x027C8D50 size 0x94 main. */

extern void fn_02395744(unsigned int arg);
extern unsigned int fn_02395C74(unsigned int arg);
extern void fn_02395BE0(unsigned int a, unsigned int b);
extern void fn_02395830(void);

void fn_02396DD8(void) {
    unsigned int mode;
    unsigned int value;

    *(unsigned short *)0x04808036 = 0;
    fn_02395744(8);
    *(unsigned short *)0x04808168 = 0;
    mode = *(unsigned short *)(*(char **)0x0380FFF4 + 0x500 + 0xB0);
    switch (mode) {
    case 2:
        value = fn_02395C74(1);
        fn_02395BE0(1, value & 0x7F);
        fn_02395BE0(1, value);
        fn_02395744(0x28);
        fn_02395830();
        break;
    case 3:
        fn_02395830();
        break;
    }
}
