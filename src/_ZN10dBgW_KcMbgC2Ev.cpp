//cpp
// @symbol _ZN10dBgW_KcMbgC2Ev
/* C2, the base-subobject constructor, and the body is empty: the base call to
 * dBgW_Kc's own C2 and the vptr store that follows it are both generated. Its
 * only ROM caller is the derived base step inside dBgW_KcMbgSclY's constructor
 * pair, which is what makes the variant C2 -- notes/ctor-migration.md 4. */
#include "dBgW_KcMbg.h"

dBgW_KcMbg::dBgW_KcMbg()
{
}
