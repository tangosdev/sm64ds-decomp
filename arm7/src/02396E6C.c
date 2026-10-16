/* storage 0x02396E6C runtime 0x027C8DE4 size 0x7C main. */

extern void fn_02395B9C(unsigned int arg);
extern unsigned int fn_02395C74(unsigned int arg);
extern void fn_02395BE0(unsigned int a, unsigned int b);

void fn_02396E6C(void) {
    unsigned int mode;
    unsigned int value;

    mode = *(unsigned short *)(*(char **)0x0380FFF4 + 0x500 + 0xB0);
    switch (mode) {
    case 2:
        fn_02395B9C(0xC008);
        break;
    case 3:
        break;
    }
    value = fn_02395C74(0x1E) | 0x3F;
    fn_02395BE0(0x1E, value);
    *(unsigned short *)0x04808168 = 0x800D;
    *(unsigned short *)0x04808036 = 1;
}
