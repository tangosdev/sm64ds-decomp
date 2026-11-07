/* storage 0x02396B68 runtime 0x027C8AE0 size 0x44 main.
 * Publishes a1 into the WRAM state block plus both MMIO halfwords.
 * Three pool refs (off_380FFF4, two MMIO), no calls. The WRAM pointer
 * slot is volatile: each *pp deref is its own load. */

extern unsigned char *off_380FFF4; /* =0x0380FFF4 */

unsigned short fn_02396B68(unsigned short a1) {
    unsigned char * volatile *pp = &off_380FFF4;
    unsigned int bit;
    *(unsigned short *)(*pp + 0x300 + 0x66) = a1;
    bit = 1 << a1;
    *(unsigned short *)(*pp + 0x400 + 0x86) = bit;
    *(volatile unsigned short *)0x480802A = a1;
    *(volatile unsigned short *)0x4808028 = a1;
    return a1;
}
