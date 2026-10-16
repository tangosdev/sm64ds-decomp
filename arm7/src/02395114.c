extern char *unk_380FFF4;
extern void fn_02395160(void);
extern void fn_023A05A4(int a, int b);

int fn_02395114(unsigned short *block) {
    int value;
    unsigned short *counter;

    value = block[0];
    if (value != 0) {
        fn_02395160();
        fn_023A05A4(*(int *)((char *)block + 8), *(int *)((char *)block + 12) - 0x10);
        counter = (unsigned short *)(unk_380FFF4 + 0x3B4);
        value = *counter + 1;
        *counter = (unsigned short)value;
    }
    return value;
}
