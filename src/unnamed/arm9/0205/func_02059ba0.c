/* NitroSDK OSAlarm, the fields cleared here (SDK names; func_02059a60, the
 * alarm-list insert, reads fire / prev / next / period / start further on). */
typedef struct OSAlarm {
    void (*handler)(void *arg); /* 0x00 */
    void *arg;                  /* 0x04 */
    unsigned int tag;           /* 0x08 */
} OSAlarm;

/* NitroSDK OS_CreateAlarm(alarm): no handler, tag 0. */
void func_02059ba0(char *p)
{
    OSAlarm *alarm = (OSAlarm *)p;
    alarm->handler = 0;
    alarm->tag = 0;
}
