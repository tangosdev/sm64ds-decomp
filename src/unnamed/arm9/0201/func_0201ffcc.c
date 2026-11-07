#include "private/watchdog_alarm.h"

extern unsigned char data_0209e64c;
extern struct WatchdogAlarm data_0209d574;
extern unsigned char data_0208f274;
extern unsigned char func_0201a9ec(char *c);
extern void func_0203df40(void);
extern int func_0203daac(void);
extern void func_0201a9fc(char *c);
extern unsigned char func_0201a96c(char *self, unsigned char newVal);

/* The link step, once the link is started (data_0209e64c): run
   func_0203df40 with the watchdog (data_0209d574) held, take the player
   count (func_0203daac) into data_0208f274 and restart the watchdog's
   count. The main loop (func_020197b8) runs it every frame. */
void func_0201ffcc(void)
{
    unsigned char saved;
    if (data_0209e64c == 0) return;
    saved = func_0201a9ec((char *)&data_0209d574);
    func_0203df40();
    data_0208f274 = func_0203daac();
    func_0201a9fc((char *)&data_0209d574);
    func_0201a96c((char *)&data_0209d574, saved);
}
