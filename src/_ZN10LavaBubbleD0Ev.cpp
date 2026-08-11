//cpp
// @symbol _ZN10LavaBubbleD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases, then
 * return the object to its heap. Nobody writes that; declaring `~LavaBubble()`
 * is enough, because mwcc emits D2, D0 and D1 together and objisolate keeps the
 * one this file is bound to.
 *
 * The deallocation is an inline operator delete -- Enemy's, reached because
 * Enemy is this class's IMMEDIATE base -- which is why nothing below mentions a
 * heap.
 */
#include "LavaBubble.h"

LavaBubble::~LavaBubble()
{
}
