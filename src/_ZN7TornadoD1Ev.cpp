//cpp
// @symbol _ZN7TornadoD1Ev
/* Force mwccarm to emit the class-body destructor as a genuine C++ D1. */
#include "Tornado.h"

void Tornado_EmitDestructor(Tornado *tornado)
{
    tornado->~Tornado();
}
