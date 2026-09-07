//cpp
// @symbol _ZN4CoinD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body.
 * Vtable slot 16: one vtable store, the five members in reverse, then
 * ~dActor_c. Was a forcing-destructor scaffold (a local duplicate shadow
 * struct) until Coin.h became the real class; now it just includes it. */
#include "Coin.h"

Coin::~Coin()
{
}
