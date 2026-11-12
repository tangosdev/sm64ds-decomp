#include "nitro/spi/pm.h"

extern unsigned char data_0209d4d4;
extern PMBackLightSwitch data_0209d508;
extern PMBackLightSwitch data_0209d510;

/* Turn both backlights off for a closed lid, once: keep their states
   (PM_GetBackLight into data_0209d508 and data_0209d510), then
   PM_SetBackLight(PM_LCD_ALL, PM_BACKLIGHT_OFF), as
   TurnBacklightOff does. data_0209d4d4 records it. The lid check
   func_02019ac4 calls it when the lid closes. */
void func_02019e60(void) {
    if (data_0209d4d4)
        return;
    PM_GetBackLight(&data_0209d508, &data_0209d510);
    PM_SetBackLight(PM_LCD_ALL, PM_BACKLIGHT_OFF);
    data_0209d4d4 = 1;
}

/* Put the backlights back as func_02019e60 kept them: the top from
   data_0209d508, the bottom from data_0209d510. The lid check
   func_02019ac4 calls it when the lid opens again. */
void func_02019dfc(void) {
    if (data_0209d4d4 == 0)
        return;
    PM_SetBackLight(PM_LCD_TOP, data_0209d508);
    PM_SetBackLight(PM_LCD_BOTTOM, data_0209d510);
    data_0209d4d4 = 0;
}
