/* storage 0x023952FC runtime 0x027C7274 size 0x30 main. Pool +0x30: 0000FFF8, 0380FFF4. */

void fn_023952FC(unsigned int first, unsigned int second) {
    char *base;

    base = *(char **)0x0380FFF4 + 0x500;
    *(unsigned short *)(base + 0xA8) = (unsigned short)((first & 0xFFF8) + 5);
    base = *(char **)0x0380FFF4 + 0x500;
    *(unsigned short *)(base + 0xAA) = (unsigned short)(second | 1);
}
