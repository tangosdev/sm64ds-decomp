//cpp
// @symbol _ZN7ClipperD0Ev
/* The deleting variant comes from the same real destructor. Clipper's inline
 * operator delete selects the ROM's Memory::operator_delete2 callee. */
#include "Clipper.h"

Clipper::~Clipper()
{
}
