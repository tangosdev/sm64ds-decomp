/* dBgW RTTI and the mHeap typeinfo records.
 * _ZTS/_ZTI/_ZTV of dBgW_Kc, dBgW_KcMbg, dBgW_KcMbgSclY and dM3dGSph, then
 * the mHeap Heap_t/ExpHeap_t/SolidHeap_t typeinfo records.
 * ROM 0x02099ce4..0x02099cec, 8 bytes, 1 configured symbols.
 * A _ZTI record's first word is its abi RTTI vtable address
 * (__class_type_info 0x0209a774); it stays a number because the tree
 * declares no _ZTV array.
 */
#include "types.h"

extern char _ZTSN5mHeap6Heap_tE[];

/* 0x02099ce4: 8 bytes. */
u32 _ZTIN5mHeap6Heap_tE[2] = {
    0x0209a774,
    (u32)_ZTSN5mHeap6Heap_tE,
};
