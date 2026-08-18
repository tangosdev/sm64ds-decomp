//cpp
/* Actor::OnPushed(Actor&) at 0x02010138, 4 bytes -- vtable slot 25.
 *
 * A bare `bx lr`. The base actor does not react to being pushed; leaf classes
 * override.
 *
 * The declared `int` return comes from include/Actor.h and cannot be confirmed
 * here: the ROM sets no r0, so the caller gets whatever was already in it, and
 * `int` and `void` compile to the identical single instruction. The definition
 * is silent on the question -- only a caller could settle it, and the header
 * already holds every caller in the tree to `int`.
 */
#include "Actor.h"

int Actor::OnPushed(Actor &pusher)
{
}
