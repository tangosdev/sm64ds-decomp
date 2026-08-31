//cpp
// @symbol _ZN19daPropeller_Heyho_cD1Ev
/* recovered: real C++ destructor -- inline in the class, forced out of line here
 *
 * One vtable store and a destructor call per member, every one a consequence of
 * `struct daPropeller_Heyho_c : dEnemyBase_c` and the members that declaration types, destroyed in
 * reverse declaration order, then dEnemyBase_c::~dEnemyBase_c.
 *
 * The shared header keeps the destructor inline so derived lifecycle code and
 * the reconstructed class TU see the retail definition.  This never-called
 * helper materializes the standalone D1 copy while objisolate discards the
 * helper's own text.
 */
#include "daPropeller_Heyho_c.h"

void _force_daPropeller_Heyho_cD1(daPropeller_Heyho_c *actor)
{
    actor->~daPropeller_Heyho_c();
}
