/*
 * Title-screen material descriptors.
 *
 * The strings are the material names ("mat_shadow", "shadow_cube",
 * "shadow_cylinder") and the records beside them reference each other and the
 * texture descriptor at 0x020ab5f8. The layout is not recovered; the words and
 * their pointer relocations are the cartridge's.
 * Leftover: the dThIcon_c RTTI group (_ZTI/_ZTS/_ZTV at 0x020ad478..0x020ad49c)
 * is not claimed here -- a vtable belongs to the class TU that emits it.
 */
#include "types.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
u32 data_ov001_020ad49c[3] = {
    0x5f74616d,
    0x64616873,
    0x0000776f,
};
#pragma section obj end
