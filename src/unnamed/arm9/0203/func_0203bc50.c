/* Clear the four per-player key records (data_020a0e58, held and pressed,
   as func_0203bc7c fills them). The soft reset (func_02023498 in
   func_0202345c.cpp) runs it every frame while a reset is pending. */
#include "private/controller_input.h"

extern PadData data_020a0e58[];

void func_0203bc50(void)
{
    PadData *p = data_020a0e58;
    int i;
    for (i = 0; i < 4; i++) {
        p->held = 0;
        p->pressed = 0;
        p++;
    }
}
