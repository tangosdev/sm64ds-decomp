/* storage 0x0239F690 runtime 0x027D1608 size 0x48 main. */

void fn_0239F690(void) {
    char *slot;
    unsigned int word;

    slot = *(char **)0x0380FFF4 + 0x45C;
    *(unsigned short *)slot = 1;
    word = *(unsigned int *)(slot + 8) & 0x3FFF;
    *(unsigned short *)0x04808080 = (unsigned short)((word >> 1) | 0x8000);
}
