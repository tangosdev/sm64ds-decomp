//cpp
// @symbol _ZN6PlayerD0Ev
/* recovered: real C++ deleting destructor
 *
 * Player.h's member order generates the nine member destructor calls and
 * three reverse array cleanups. CodeWarrior then destroys dActor_c and uses
 * the inherited actor-heap operator delete for this D0 variant.
 */
#include "Player.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~Player() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" Player *_ZN6PlayerD0Ev(Player *thiz)
{
    thiz->Player::~Player();        /* the D1 body, through the one host symbol */
    Player::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
Player::~Player()
{
}
#endif
