//cpp
// @symbol _ZN6Player29TryExitCharacterDoorWithIntroEv
/* NOT a Player method, despite the name. ov006, 0x020ca78c.
 *
 * The shape is a constructor, not a state handler: it calls a base routine,
 * stores a vptr into [this+0], then runs a member constructor at this+0x78.
 * Player's own constructors are _ZN6PlayerC1Ev / C3Ev in ov002, and Player's
 * ModelAnims live at 0xf0 and 0x174 -- not 0x78, which on a real Player is
 * Actor's mCamSpacePosY.
 *
 * This file was the only evidence for Player.h's `mModelAnim2` at 0x078, so
 * that field can now be dropped and the offset returned to Actor.
 *
 * The vptr it stores is the invented symbol VT0, which carries no address --
 * a separate problem, left alone here.
 *
 * Detached from Player.h; see _ZN6Player23St_InYoshiMouth_CleanupEv.cpp.
 */
#include "decl_ModelAnim.h"
#include "decl_common.h"

extern "C" int *_ZN6Player29TryExitCharacterDoorWithIntroEv(int *self)
{
    func_ov006_020cd6f4(self);
    self[0] = (int)VT0;
    _ZN9ModelAnimC1Ev((char *)self + 0x78);
    return self;
}
