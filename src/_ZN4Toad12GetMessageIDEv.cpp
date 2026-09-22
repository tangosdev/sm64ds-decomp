//cpp
// @symbol _ZN4Toad12GetMessageIDEv
#include "Toad.h"

extern "C" {
extern s8 data_0209f2f8;
extern int func_020138dc();
extern int func_02013a44();
extern u16 ObjectMessageIDToActualMessageID(s16);
}

u16 Toad::GetMessageID()
{
    int *player = (int *)mTalkPlayer;
    u16 messageID = mMessageID;

    if (data_0209f2f8 == 0x32) {
        int state = func_020138dc();
        switch (state) {
        case 0x1c:
            return 0x134;
        case 0: {
            int message = ObjectMessageIDToActualMessageID((s16)messageID);
            int sum = message + player[2];
            return (u16)sum;
        }
        default:
            return 0x133;
        }
    }

    if (mVariant == 1 && func_02013a44() != 0) {
        int n = player[2];
        int message = 0xb0a;
        message += n;
        return ObjectMessageIDToActualMessageID((s16)(u16)message);
    }

    int message = ObjectMessageIDToActualMessageID((s16)messageID);
    int sum = message + player[2];
    return (u16)sum;
}
