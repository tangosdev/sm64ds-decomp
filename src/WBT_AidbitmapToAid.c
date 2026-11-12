#include "private/arm9_wbt.h"

int WBT_AidbitmapToAid(u16 aid_bmp) {
    int i;
    for (i = 0; i < 16; i++)
        if ((aid_bmp >> i) & 1)
            return i;
    return -1;
}
