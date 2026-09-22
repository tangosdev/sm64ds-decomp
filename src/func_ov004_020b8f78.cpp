//cpp
#include "dMgPsOpt_c.h"

extern "C" {
int TouchArea_Update(char *icon, int touchID);
void func_ov001_020ab3f0(void *icon);
u32 func_02012790(u32 soundID);
int GetSoundMode(void);
void SetSoundMode(int mode);
void func_ov001_020ab41c(void *icon);
void TurnBacklightOn(void);
void TurnBacklightOff(void);
u8 DecIfAbove0_Byte(u8 *p);
void func_ov004_020b91fc(char *options);
}

extern "C" u8 func_ov004_020b8f78(char *self)
{
    dMgPsOpt_c *options = (dMgPsOpt_c *)self;
    u8 state = options->mActive;
    switch (state) {
    case 1:
        if (TouchArea_Update((char *)&options->mIcons[5], -1)) {
            func_ov001_020ab3f0(&options->mIcons[5]);
            options->mCloseDelay = 0x14;
            options->mActive = 2;
            func_02012790(0x65);
        } else {
            if (options->mIcons[2].unk_011 != 0) {
                int soundChanged = 0;
                int mode = GetSoundMode();
                if (TouchArea_Update((char *)&options->mIcons[2], -1) ||
                    TouchArea_Update((char *)&options->mIcons[1], -1) ||
                    TouchArea_Update((char *)&options->mIcons[6], -1)) {
                    if (mode == 2)
                        mode = 0;
                    else
                        mode = (mode + 1) & 0xff;
                    options->mIcons[1].unk_010 = 0;
                    func_ov001_020ab3f0(&options->mIcons[1]);
                    soundChanged = 1;
                } else if (TouchArea_Update((char *)&options->mIcons[0], -1)) {
                    if (mode == 0)
                        mode = 2;
                    else
                        mode = (mode - 1) & 0xff;
                    options->mIcons[0].unk_010 = 0;
                    func_ov001_020ab3f0(&options->mIcons[0]);
                    soundChanged = 1;
                }
                if (soundChanged != 0) {
                    SetSoundMode(mode);
                    options->mIcons[2].unk_020 = mode;
                    func_ov001_020ab41c(&options->mIcons[2]);
                    func_02012790(0x64);
                }
            }
            if (options->mIcons[3].unk_011 != 0 && options->mIcons[4].unk_011 != 0) {
                int backlightDirection = 0;
                if (TouchArea_Update((char *)&options->mIcons[3], -1)) {
                    backlightDirection = -1;
                } else if (TouchArea_Update((char *)&options->mIcons[4], -1)) {
                    backlightDirection = 1;
                } else if (TouchArea_Update((char *)&options->mIcons[7], -1)) {
                    backlightDirection = (options->mIcons[3].unk_010 != 0) ? 1 : -1;
                }
                if (backlightDirection < 0) {
                    if (options->mIcons[3].unk_010 != 0) {
                        func_02012790(0x67);
                    } else {
                        TurnBacklightOn();
                        func_02012790(0x66);
                    }
                    func_ov001_020ab41c(&options->mIcons[3]);
                    options->mIcons[4].unk_010 = 0;
                } else if (backlightDirection > 0) {
                    if (options->mIcons[4].unk_010 != 0) {
                        func_02012790(0x67);
                    } else {
                        TurnBacklightOff();
                        func_02012790(0x66);
                    }
                    options->mIcons[3].unk_010 = 0;
                    func_ov001_020ab41c(&options->mIcons[4]);
                }
            }
        }
        break;
    case 2:
        if (DecIfAbove0_Byte(&options->mCloseDelay) == 0)
            func_ov004_020b91fc(self);
        break;
    }

    for (int iconIndex = 0; iconIndex < options->mIconCount; iconIndex++) {
        options->mIcons[iconIndex].Behavior();
    }
    return options->mActive;
}
