//cpp
// @symbol _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh
/* recovered: named members + shared header, real C++ method
 *
 * Thin wrapper: forwards to ShowMessage2 and, if that took, clears mStateWork
 * to 1.
 *
 * The last two parameters are unsigned char. That is what the ROM does -- both
 * are read with ldrb -- and now what the symbol says, since the preceding
 * commit corrected it from jj to hh. While it said jj this function could not
 * be a real method at all: the compiler mangles FROM the declared types.
 */
#include "Player.h"
extern "C" {
extern int _ZN6Player12ShowMessage2ER7fBase_cjPK7Vector3hh(void* c, fBase_c* a, unsigned int b, const Vector3* v, unsigned int d, unsigned int e);
}

int Player::ShowMessage(fBase_c & a_, unsigned int b, const Vector3 * v, unsigned char d_, unsigned char e_)
{
    if (_ZN6Player12ShowMessage2ER7fBase_cjPK7Vector3hh(this, &a_, b, v, d_, e_)) {
        mStateWork = 1;
        return 1;
    }
    return 0;
}
