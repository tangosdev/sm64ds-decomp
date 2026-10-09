#include "PlayerInput.h"
struct CtrlSlot { unsigned char mode; unsigned char pad[0x17]; };
extern struct CtrlSlot data_0209f4ae[];
extern unsigned char data_0209caa0[];

void SetControllerMode(unsigned char mode)
{
    data_0209f4ae[gActivePlayerSlot].mode = mode;
    data_0209caa0[0x42] = mode;
}
