//cpp
// @symbol _ZN14QuestionSwitch10HandleClsnER8dActor_c
/* Inferred descriptive name. The sole caller is AfterClsnCallback, which
 * supplies the collider owner as this and the colliding actor by reference. */
#include "QuestionSwitch.h"

void QuestionSwitch::HandleClsn(dActor_c &other)
{
    u16 actorID = other.actorID;
    int isPlayer = (int)(actorID == 0xbf);
    if (isPlayer == 0) {
        int isYoshi = (int)(actorID == 0xc2);
        if (isYoshi == 0)
            return;
    }

    if (other.mPosY > mPosY + 0x64000)
        mPressedThisFrame = 1;
}
