/* storage 0x02383598 runtime 0x037FB430 size 0x18 wram ARM. No pool. */

unsigned short *fn_02383598(int a1, short a2) {
    unsigned short *result;
    a2 = 0x10000 - a2;
    result = (unsigned short *)((a1 << 4) + 0x4000000);
    result[0x204] = a2;
    return result + 0x200;
}
