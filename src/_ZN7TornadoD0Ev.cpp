//cpp
// @symbol _ZN7TornadoD0Ev
/* A delete expression forces the compiler-spelled deleting destructor. */
#include "Tornado.h"

void Tornado_EmitDeletingDestructor(Tornado *tornado)
{
    delete tornado;
}
