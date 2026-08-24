//cpp
// @symbol _ZN9ModelBaseC2Ev
/* recovered: real C++ constructor -- the base of the model family
 *
 * An empty body; the two stores in these 0x18 bytes are what the declaration
 * already means. The vptr store at +0x0 is implicit in the first virtual
 * declaration, and `modelFile(0)` is the zero at +0x4 -- without it the
 * compiler emits nothing for the field and the ROM's second store never
 * appears.
 *
 * This is the function every derived constructor in the family calls
 * out-of-line as its base subobject step, so it carries the C2 (base-object)
 * variant label: all four ROM callers are derived constructors, none is a
 * complete-object construction, and mwccarm names the base-subobject call
 * C2 under the Itanium ABI. Renamed from C1, which is why the sibling C1
 * exists nowhere in this image -- nothing constructs a standalone ModelBase,
 * and the linker's dead stripping kept only the referenced variant.
 */
#include "ModelBase.h"

ModelBase::ModelBase() : modelFile(0) {}
