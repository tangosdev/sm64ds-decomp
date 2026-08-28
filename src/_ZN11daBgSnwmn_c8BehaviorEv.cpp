//cpp
// @symbol _ZN11daBgSnwmn_c8BehaviorEv
/* daBgSnwmn_c::Behavior() -- vtable slot 6. See include/daBgSnwmn_c.h. All
 * four calls reach into the typed mTexSeq/mCylClsn members. */
#include "daBgSnwmn_c.h"

extern const Vector3 data_ov072_02122c70;

s32 daBgSnwmn_c::Behavior()
{
    mTexSeq.Advance();
    mCylClsn.SetPosRelativeToActor(data_ov072_02122c70);
    mCylClsn.Clear();
    mCylClsn.Update();
    return 1;
}
