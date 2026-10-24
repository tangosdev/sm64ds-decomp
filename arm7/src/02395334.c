/* storage 0x02395334 runtime 0x027C72AC size 0x20 main. No pool. */

unsigned int fn_02395334(unsigned int addr) {
    unsigned int value;

    if ((addr & 1) != 0)
        value = ((int)*(unsigned short *)(addr - 1) >> 8) & 0xFF;
    else
        value = *(unsigned short *)addr & 0xFF;
    return value & 0xFF;
}
