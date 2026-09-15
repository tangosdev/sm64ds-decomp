//cpp
// @symbol _ZN7daTrs_c16OnAimedAtWithEggEv
/* daTrs_c::OnAimedAtWithEgg (slot 29): half the collision height -- the
   egg-aim vertical. mdCcAcPos_c.height, not a daTrs_c field of its own. */
#include "daTrs_c.h"

int daTrs_c::OnAimedAtWithEgg() {
    return mdCcAcPos_c.height / 2;
}
