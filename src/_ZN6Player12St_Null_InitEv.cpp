//cpp
// @symbol _ZN6Player12St_Null_InitEv
/* recovered: shared header, real C++ method
 *
 * ov002, 0x020cac30, size 0x8 -- `mov r0, #1; bx lr` and nothing else.
 *
 * The whole body IS the evidence for the name. Player::State holds three
 * `int (Player::*)()` members, ChangeState calls mInit last on the state
 * being entered, and a handler returns 1 for success. A state called Null
 * whose Init does nothing but succeed is exactly eight bytes. ov002 reaches
 * this address by `kind:load ... module:overlay(2)` from 0x0210a17c, which
 * sits in the eight-byte-stride pointer-to-member run that holds those
 * State objects.
 *
 * This file carried the placeholder name func_ov002_020cac30 until the symbol
 * was moved here from ov006, where it had been sitting on an unrelated 0x6c
 * function at the same shared address. See the commit and include/Player.h.
 */
#include "Player.h"

int Player::St_Null_Init()
{
    return 1;
}
