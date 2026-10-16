/* storage 0x02395354 runtime 0x027C72CC size 0x34 main. No relocs, no pool. */

void fn_02395354(unsigned int addr, unsigned int val) {
    if (addr & 1) {
        unsigned short *p = (unsigned short *)(addr - 1);
        *p = (*p & 0xFF) | (val << 8);
        return;
    }
    *(unsigned short *)addr = (*(unsigned short *)addr & 0xFF00) | (val & 0xFF);
}
