//cpp
// @symbol _ZN7PathPtrC1Ev
/* recovered: real C++ constructor
 *
 * The whole ROM body (0x0203ad74, 0x10 bytes) is two zero stores -- the init
 * list is the entire constructor. PathPtr is not polymorphic and has no bases,
 * so there is no vptr store and no base call: what `PathPtr();` declared in
 * PathPtr.h plus this definition means is exactly what the ROM does.
 *
 * Fifth real constructor in the tree; recipe in notes/ctor-migration.md
 * section 6. common.h is not needed here -- nothing copies a Matrix4x3.
 */
#include "PathPtr.h"

PathPtr::PathPtr() : def(0), unk_004(0) {}
