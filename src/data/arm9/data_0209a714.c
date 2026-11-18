/* std::type_info and the abi RTTI records, plus the overlay digest.
 * _ZSt8thandler, the type_info/__class_type_info/__si_class_type_info/
 * __vmi_class_type_info strings and vtables, and SDK_OVERLAY_DIGEST's
 * 2060-byte block with its zero end marker.
 * ROM 0x0209a714..0x0209a72c, 24 bytes, 2 configured symbols.
 * A _ZTI record's first word is its abi RTTI vtable address
 * (__si_class_type_info 0x0209a764); it stays a number because the tree
 * declares no _ZTV array.
 */
#include "types.h"

extern u32 _ZTIN3abi17__class_type_infoE[3];
extern char _ZTSN3abi20__si_class_type_infoE[];
extern char _ZTSN3abi17__class_type_infoE[];
extern u32 _ZTISt9type_info[];

/* 0x0209a714: 12 bytes. */
u32 _ZTIN3abi20__si_class_type_infoE[3] = {
    0x0209a764,
    (u32)_ZTSN3abi20__si_class_type_infoE,
    (u32)_ZTIN3abi17__class_type_infoE,
};

/* 0x0209a720: 12 bytes. */
u32 _ZTIN3abi17__class_type_infoE[3] = {
    0x0209a764,
    (u32)_ZTSN3abi17__class_type_infoE,
    (u32)_ZTISt9type_info,
};
