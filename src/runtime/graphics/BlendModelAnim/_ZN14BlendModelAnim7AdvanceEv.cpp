//cpp
// @symbol _ZN14BlendModelAnim7AdvanceEv
#include "BlendModelAnim.h"

void BlendModelAnim::Advance()
{
    Animation::Advance();
    if (blendWeight < 0x1000) {
        /* launder: keep the RMW aliasing the member so the compiler
           re-reads it the way the ROM does */
        *(int *)(&blendWeight) += blendStep;
    }
}
