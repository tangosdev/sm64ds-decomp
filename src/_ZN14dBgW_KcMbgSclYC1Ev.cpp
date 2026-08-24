//cpp
// @symbol _ZN14dBgW_KcMbgSclYC1Ev
/* recovered: real C++ constructor -- the whole body is synthesized
 *
 * An empty out-of-line definition makes mwcc emit exactly the ROM pair:
 * `bl _ZN10dBgW_KcMbgC2Ev', then the dBgW_KcMbgSclY vptr store. The
 * emitted C2 sibling has no ROM counterpart (nothing derives from SclY)
 * and is stripped by objisolate (section 1).
 */
#include "dBgW_KcMbgSclY.h"

dBgW_KcMbgSclY::dBgW_KcMbgSclY() {}
