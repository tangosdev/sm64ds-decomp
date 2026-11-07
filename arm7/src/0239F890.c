/* storage 0x0239F890 runtime 0x027D1808 size 0xB8 main. */

extern void fn_0239F7F8(unsigned int which, unsigned int flag);

void fn_0239F890(void) {
    unsigned int mode;

    mode = *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0x14);
    switch (mode) {
    case 1:
        fn_0239F7F8(0, 1);
        fn_0239F7F8(1, 0);
        fn_0239F7F8(2, 1);
        break;
    case 2:
    case 3:
        fn_0239F7F8(0, 1);
        fn_0239F7F8(1, 0);
        fn_0239F7F8(2, 0);
        break;
    default:
        fn_0239F7F8(0, 0);
        fn_0239F7F8(1, 0);
        fn_0239F7F8(2, 0);
        break;
    }
}
