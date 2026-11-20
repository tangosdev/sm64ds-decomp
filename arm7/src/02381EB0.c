/* storage 0x02381EB0 runtime 0x037F9D48 size 0xA0 wram ARM.
 * Timer sample: latch REG_TM0CNT_L, bump the 48-bit tick when the cascade
 * bit is set and the counter did not wrap. The tick and sample are volatile
 * locals (timer ISR shares them), hence the stack traffic. Four pool words
 * (two MMIO addresses, the tick address, one mask). */

extern int fn_02382AC4(void);
extern int fn_02382AD8(int);
extern unsigned long long qword_3806FD8;

long long fn_02381EB0(void) {
    int v0 = fn_02382AC4();
    volatile unsigned short v2 = *(volatile unsigned short *)0x4000100;
    volatile unsigned long long v3 = qword_3806FD8 & 0xFFFFFFFFFFFFULL;
    if ((*(volatile int *)0x4000214 & 8) != 0 && (v2 & 0x8000) == 0)
        ++v3;
    fn_02382AD8(v0);
    return (v3 << 16) | v2;
}
