/* std::type_info and the abi RTTI records, plus the overlay digest.
 * _ZSt8thandler, the type_info/__class_type_info/__si_class_type_info/
 * __vmi_class_type_info strings and vtables, and SDK_OVERLAY_DIGEST's
 * 2060-byte block with its zero end marker.
 * ROM 0x0209a77c..0x0209a7d8, 92 bytes, 3 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x0209a798: 32 bytes. */
char _ZTSN3abi20__si_class_type_infoE[32] = "N3abi20__si_class_type_infoE";

/* 0x0209a77c: 28 bytes. */
char _ZTSN3abi17__class_type_infoE[28] = "N3abi17__class_type_infoE";

/* 0x0209a7b8: 32 bytes. */
char _ZTSN3abi21__vmi_class_type_infoE[32] = "N3abi21__vmi_class_type_infoE";
