//cpp
// @symbol _ZN6Player29TryExitCharacterDoorWithIntroEv
/* recovered: named members + shared header, vtable identified, real C++ method, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified, real C++ method */
#include "Player.h"

int * Player::TryExitCharacterDoorWithIntro()
{
    func_ov006_020cd6f4(((int *)this));
    ((int *)this)[0] = (int)VT0;
    _ZN9ModelAnimC1Ev((char *)&mModelAnim2);
    return ((int *)this);
}
