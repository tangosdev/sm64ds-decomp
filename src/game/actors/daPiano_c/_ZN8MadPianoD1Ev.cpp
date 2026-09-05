//cpp
// @symbol _ZN8MadPianoD1Ev
/* Member and base ownership make CodeWarrior emit every teardown step in ROM
 * order; the recovered source destructor itself is empty. */
#include "MadPiano.h"

MadPiano::~MadPiano()
{
}
