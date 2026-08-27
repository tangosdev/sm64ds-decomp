//cpp
// @symbol _ZN14QuestionSwitch15OnGroundPoundedER8dActor_c
/* recovered: real C++ override -- slot 21, attributed by the vtable.
   _ZTV14QuestionSwitch + 4*21 = 0x02108e5c + 0x54 = 0x02108eb0;
   config/arm9/overlays/ov002/relocs.txt: 0x02108eb0 -> 0x020b4fc4. */
#include "QuestionSwitch.h"

void QuestionSwitch::OnGroundPounded(dActor_c &other)
{
    mPressTimer = 0;
}
