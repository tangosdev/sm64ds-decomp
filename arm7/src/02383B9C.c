/* storage 0x02383B9C runtime 0x037FBA34 linked 0x58 wram.
 * SND_StartIntervalTimer: OS_SetPeriodicAlarm(alarm, tick + 0x10000, 2728, handler, NULL).
 * period is a 64-bit tick, so 2728 occupies r3 and the first stack slot. */
extern long long fn_02381EB0(void);
extern int fn_0238222C(int *alarm, unsigned long long start, unsigned long long period,
                       void (*handler)(void *), void *arg);
extern int dat_038070FC;
extern void fn_02383B08(void);

int fn_02383B9C(void) {
    unsigned long long tick = fn_02381EB0();
    return fn_0238222C(&dat_038070FC, tick + 0x10000, 2728,
                       (void (*)(void *))fn_02383B08, 0);
}
