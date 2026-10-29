extern int func_0201aad4(void *p);

/* The watchdog alarm at data_0209d574 (constructed by __sinit_02074e0c, which
 * registers func_0201aaec to destroy it). It starts with a NitroSDK OSAlarm:
 * func_0201aad4 reaches func_02059ba0, which clears the alarm's handler and
 * tag as OS_CreateAlarm does. Names follow the readers: func_0201a8b4, the
 * alarm handler, re-arms the alarm for what is left of limit since base,
 * and once it is used up enters the fatal-error spin (func_02013f28) unless
 * input bit 0x2000 is held; func_0201a9fc resets base to the
 * current tick; func_0201a96c sets suspended, cancelling the alarm while set
 * and re-arming it for limit when cleared. 0x2c stays raw: no reader found. */
typedef struct WatchdogAlarm {
    char alarm[0x2c]; /* 0x00 -- OSAlarm */
    char pad_2c[4];
    unsigned long long limit; /* 0x30 */
    unsigned long long base;  /* 0x38 */
    unsigned char suspended;  /* 0x40 */
} WatchdogAlarm;

#ifndef SM64DS_PLATFORM_PC
typedef char
    WatchdogAlarm_suspended_must_be_at_0x40[(int)&((WatchdogAlarm *)0)->suspended == 0x40 ? 1 : -1];
#endif

void *func_0201aa18(char *c) {
    WatchdogAlarm *w = (WatchdogAlarm *)c;
    func_0201aad4(c);
    w->limit = 0;
    w->suspended = 1;
    *(int *)(c + 0x2c) = 0;
    w->base = 0;
    return c;
}
