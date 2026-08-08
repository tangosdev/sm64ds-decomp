//cpp
/* Actor::OnYoshiTryEat() at 0x02010160, 8 bytes -- vtable slot 18.
 *
 * `mov r0,#0; bx lr`. Yoshi asks the actor whether it can be eaten, and the
 * base answer is no; leaf classes override to say otherwise.
 *
 * Unlike the empty combat hooks either side of it, this one really does set
 * r0, so the return is observable at the definition -- it returns 0.
 * include/types.h carries an `OnYoshiEatReturnVal` typedef for exactly this
 * slot, but it is a typedef of `int` and Actor.h declares the slot `int`, so
 * spelling it `int` here keeps the definition and the declaration identical.
 */
#include "Actor.h"

int Actor::OnYoshiTryEat()
{
    return 0;
}
