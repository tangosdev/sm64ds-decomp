/* storage 0x0239F6D8 runtime 0x027D1650 size 0xA0 main. */

extern unsigned int fn_023963F0(void);

void fn_0239F6D8(void) {
    char *base;
    char *slot;
    unsigned short *counter;
    char *buf;
    unsigned int bits;

    base = *(char **)0x0380FFF4;
    slot = base + 0x40C;
    counter = (unsigned short *)(base + 0x40E);
    *counter = (unsigned short)(*counter + 1);
    if (*(unsigned short *)slot != 0) {
        *(unsigned short *)(*(char **)(slot + 8) + 4) = 0;
    } else {
        *(unsigned short *)slot = 1;
        buf = *(char **)(slot + 8);
        *(unsigned short *)buf = 0;
        *(unsigned short *)(*(char **)(slot + 8) + 4) = 0;
        *(unsigned short *)(*(char **)(slot + 8) + 8) = (unsigned short)fn_023963F0();
        bits = *(unsigned int *)(slot + 8) & 0x3FFF;
        bits = (unsigned int)(bits << 15);
        bits >>= 16;
        *(unsigned short *)0x048080A8 = (unsigned short)(bits | 0x8000);
    }
}
