//cpp
// @symbol _ZN5ModelD0Ev
/* D0, the DELETING destructor. Unlike the D1/D2 pair these are NOT the
 * same code -- D0 runs the destructor and then hands the object to
 * operator delete, so it is longer. What is shared is the SOURCE: one
 * `Model::~Model()` makes mwcc emit D0, D1 and D2 together, and
 * objisolate keeps the one this file is bound to by config/arm9/delinks.txt.
 * That is why this file carries the same definition as
 * src/_ZN5ModelD1Ev.cpp -- it is not duplication, it is how
 * one-symbol-per-file enrolment meets a compiler that emits three. */
/* recovered: real C++ destructor
 *
 * Model owns the transformed-vertex buffer allocated by DoSetFile. Releasing
 * that buffer through the game's delete veneer is the only part of destruction
 * written by this class: C++ emits the Model vtable restore before the body and
 * the ModelBase destructor call after it. The class-specific operator delete
 * inherited from ModelBase also gives the compiler-generated deleting
 * destructor its ROM callee.
 */
#include "Model.h"

extern "C" void func_0203cbc0(void *ptr);

Model::~Model()
{
    if (transformsBuf)
        func_0203cbc0(transformsBuf);
}
