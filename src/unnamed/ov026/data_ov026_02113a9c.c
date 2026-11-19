/* Cylinder offset for the Wet-Dry World pole lift: (0, 0x28000, 0).
 * It sits between _ZTI17daObjWlPolelift_c and _ZTS17daObjWlPolelift_c.
 * The class TU's other sections are earlier in the overlay, so this word
 * stays in its own .data object. A C Vector3 stays a POD; the C++ type
 * would register ~Vector3. */
#include "types.h"

Vector3 data_ov026_02113a9c = {0, 0x28000, 0};
