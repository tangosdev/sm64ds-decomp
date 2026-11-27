/* storage 0x02395A44 runtime 0x027C79BC linked 0x38 main ARM.
 * Sound-reg init table walk: 22 pairs, *(off + 0x4808000) = val.
 * Table loads are volatile: the two halfword reads must issue in order. */
extern volatile unsigned int unk_27D65E4[];
int fn_02395A44(void) {
    unsigned int i = 0;
    int addr;
    do {
        unsigned int *t = (unsigned int *)&unk_27D65E4[i];
        unsigned short val = *(volatile unsigned short *)((volatile char *)t + 2);
        int k = (int)(i * 4);
        unsigned short off = *(volatile unsigned short *)((volatile char *)unk_27D65E4 + k);
        addr = off + 0x4800000;
        addr += 0x8000;
        *(unsigned short *)addr = val;
        i++;
    } while (i < 0x16);
    return addr;
}
