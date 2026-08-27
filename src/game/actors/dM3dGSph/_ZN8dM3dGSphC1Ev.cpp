//cpp
// @symbol _ZN8dM3dGSphC1Ev
/* recovered: real C++ constructor
 *
 * Renamed from func_0203ac60 (2026-08-23). RTTI on the vtable it stores
 * (_ZTV8dM3dGSph) reads typeinfo "8dM3dGSph", and its only ROM caller is
 * dBgCh_Lin's constructor constructing the bound-sphere MEMBER at +0x64 --
 * a complete-object context, hence C1 rather than C2. Body is a single
 * vptr store; centre and radius are left alone (SetObjAndLine fills them).
 *
 * That store is exactly what the compiler synthesises for a root class whose
 * constructor has no base steps and no nontrivial members, so the migrated
 * body is empty and the header carries the declarations. The out-of-line
 * definition also makes mwccarm emit the C2 sibling; the ROM holds no
 * _ZN8dM3dGSphC2Ev anywhere (dead stripping kept only the referenced
 * variant), so objisolate drops it.
 */
#include "dM3dGSph.h"

dM3dGSph::dM3dGSph() {}
