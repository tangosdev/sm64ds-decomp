//cpp
// @symbol _ZN19daPropeller_Heyho_cD0Ev
/* recovered: real C++ deleting destructor -- inline in the class, forced here
 *
 * A delete-expression is the source-level operation that needs the deleting
 * variant.  This never-called helper makes mwcc materialize D0 while
 * objisolate retains only the configured destructor symbol.
 */
#include "daPropeller_Heyho_c.h"

void _force_daPropeller_Heyho_cD0(daPropeller_Heyho_c *actor)
{
    delete actor;
}
