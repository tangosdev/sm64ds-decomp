//cpp
// @symbol _ZN4TreeD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole
 * body: run the complete-object destruction (see _ZN4TreeD1Ev.cpp), then
 * return the object to its heap through dActor_c's inline operator delete
 * (Memory::Deallocate into data_020a0eac, the actor heap). Nobody writes
 * that tail; declaring ~Tree() is enough, because mwccarm emits D2/D1/D0
 * together.
 */
#include "Tree.h"

Tree::~Tree()
{
}
