/* std::type_info and the abi RTTI records, plus the overlay digest.
 * _ZSt8thandler, the type_info/__class_type_info/__si_class_type_info/
 * __vmi_class_type_info strings and vtables, and SDK_OVERLAY_DIGEST's
 * 2060-byte block with its zero end marker.
 * ROM 0x0209a6fc..0x0209a714, 24 bytes, 3 configured symbols.
 * A _ZTI record's first word is its abi RTTI vtable address
 * (__si_class_type_info 0x0209a764, __class_type_info 0x0209a774); it stays
 * a number because the tree declares no _ZTV array.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

extern void _ZSt9dthandlerv();
extern char _ZTSSt9type_info[];
extern char _ZTSN3abi21__vmi_class_type_infoE[];
extern u32 _ZTIN3abi17__class_type_infoE[];

/* 0x0209a700: 8 bytes. */
u32 _ZTISt9type_info[2] = {
    0x0209a774,
    (u32)_ZTSSt9type_info,
};

/* 0x0209a6fc: 4 bytes. */
u32 _ZSt8thandler[1] = {
    (u32)&_ZSt9dthandlerv,
};

/* 0x0209a708: 12 bytes. */
u32 _ZTIN3abi21__vmi_class_type_infoE[3] = {
    0x0209a764,
    (u32)_ZTSN3abi21__vmi_class_type_infoE,
    (u32)_ZTIN3abi17__class_type_infoE,
};
