/* storage 0x023952E4 runtime 0x027C725C size 0x14 main. Pool +0x14: 0380FFF4. */

unsigned short fn_023952E4(unsigned short value) {
    char *base;

    base = *(char **)0x0380FFF4;
    base = base + 0x500;
    *(unsigned short *)(base + 0xAC) = value;
    return value;
}
