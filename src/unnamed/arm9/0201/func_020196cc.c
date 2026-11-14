#include "nitro/spi/pm.h"

struct LevelRng;

extern void func_0203b9b4(struct LevelRng *p, int v);
extern void SetSoundMode(int mode);
extern unsigned char data_0208ee3c;
extern struct LevelRng data_0209d4b8;
extern unsigned char data_0209d4b4;

/* Empty. */
void func_0201973c(void) {}

/* Boot setup (func_0201a054). If both screens' backlights read off
   (PM_GetBackLight), the backlight setting data_0208ee3c, which
   TurnBacklightOff clears and the pause menu's option screen shows,
   starts off. Then seed the random state data_0209d4b8 with 1234 and
   apply the sound mode again (SetSoundMode(GetSoundMode's byte)). */
void func_020196cc(void) {
    PMBackLightSwitch top, bottom;
    if (PM_GetBackLight(&top, &bottom) == PM_SUCCESS && top == PM_BACKLIGHT_OFF) {
        if (bottom == PM_BACKLIGHT_OFF)
            data_0208ee3c = 0;
    }
    func_0203b9b4(&data_0209d4b8, 0x4d2);
    SetSoundMode(data_0209d4b4);
}
