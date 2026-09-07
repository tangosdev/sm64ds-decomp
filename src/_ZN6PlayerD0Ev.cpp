//cpp
// @symbol _ZN6PlayerD0Ev
/* recovered: real C++ deleting destructor
 *
 * Player.h's member order generates the nine member destructor calls and
 * three reverse array cleanups. CodeWarrior then destroys dActor_c and uses
 * the inherited actor-heap operator delete for this D0 variant.
 */
#include "Player.h"

Player::~Player()
{
}
