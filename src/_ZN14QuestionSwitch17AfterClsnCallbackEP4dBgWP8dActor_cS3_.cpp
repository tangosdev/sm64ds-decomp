//cpp
// @symbol _ZN19daObjHatenaSwitch_c17AfterClsnCallbackEP4dBgWP8dActor_cS3_
/* Inferred descriptive name and observed three-register callback ABI.
 * func_020393c4 installs this at dBgW+0x1c (the after-collision callback),
 * and the wrapper deliberately ignores the collider argument. */
#include "QuestionSwitch.h"

void daObjHatenaSwitch_c::AfterClsnCallback(dBgW *collider, dActor_c *owner,
                                       dActor_c *other)
{
    ((daObjHatenaSwitch_c *)owner)->HandleClsn(*other);
}
