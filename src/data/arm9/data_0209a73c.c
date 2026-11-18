/* std::type_info and the abi RTTI records, plus the overlay digest.
 * _ZSt8thandler, the type_info/__class_type_info/__si_class_type_info/
 * __vmi_class_type_info strings and vtables, and SDK_OVERLAY_DIGEST's
 * 2060-byte block with its zero end marker.
 * ROM 0x0209a73c..0x0209a744, 8 bytes, 1 configured symbols.
 */
#include "types.h"

extern u32 _ZTISt9type_info[];

/* 0x0209a73c: 8 bytes. */
u32 data_0209a73c[2] = {
    0x00000000,
    (u32)_ZTISt9type_info,
};
