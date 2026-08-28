//cpp
// @symbol _ZN16daObjCtMecha04_c17AfterClsnCallbackEP4dBgWP8dActor_cS3_
/* Inferred descriptive name and observed three-register callback ABI. The
 * wrapper deliberately ignores the collider and tail-calls the owner method. */
#include "daObjCtMecha04_c.h"

void daObjCtMecha04_c::AfterClsnCallback(dBgW *collider, dActor_c *owner,
                                         dActor_c *other)
{
    ((daObjCtMecha04_c *)owner)->MoveActorOnBelt(*other);
}
