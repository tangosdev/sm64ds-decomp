// Host definition of daDemo_c's deleting destructor. MSVC folds the Itanium
// destructor variants into the one ~daDemo_c() that src/game/actors/d_a_demo.cpp
// defines, so this file exports the flat ROM D0 name: the D1 body, called
// qualified so it is a direct call, then the class-specific operator delete.
#include "daDemo_c.h"

extern "C" daDemo_c *_ZN8daDemo_cD0Ev(daDemo_c *thiz)
{
    thiz->daDemo_c::~daDemo_c();
    daDemo_c::operator delete(thiz);
    return thiz;
}
