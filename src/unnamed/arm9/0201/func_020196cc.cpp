#include "nitro/spi/pm.h"

/* leftover: LevelRng::~LevelRng() is these four bytes and also emits D2,
   which has no address before OS_Terminate. */
void func_0201973c(void) {}

extern "C" {

struct LevelRng;

extern void func_0203b9b4(int *p, int v);
extern void SetSoundMode(int mode);
extern unsigned char gBacklightEnabled;
extern struct LevelRng data_0209d4b8;
extern unsigned char data_0209d4b4;

/* Boot setup (func_0201a054). If both screens' backlights read off
   (PM_GetBackLight), the backlight setting gBacklightEnabled, which
   TurnBacklightOff clears and the pause menu's option screen shows,
   starts off. Then seed the random state data_0209d4b8 with 1234 and
   apply the sound mode again (SetSoundMode(GetSoundMode's byte)). */
void func_020196cc(void) {
    PMBackLightSwitch top, bottom;
    if (PM_GetBackLight(&top, &bottom) == PM_SUCCESS && top == PM_BACKLIGHT_OFF) {
        if (bottom == PM_BACKLIGHT_OFF)
            gBacklightEnabled = 0;
    }
    func_0203b9b4((int *)&data_0209d4b8, 1234);
    SetSoundMode(data_0209d4b4);
}
}
