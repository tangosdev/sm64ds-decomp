/* dBgW RTTI and the mHeap typeinfo records.
 * _ZTS/_ZTI/_ZTV of dBgW_Kc, dBgW_KcMbg, dBgW_KcMbgSclY and dM3dGSph, then
 * the mHeap Heap_t/ExpHeap_t/SolidHeap_t typeinfo records.
 * ROM 0x02099ce4..0x02099cec, 8 bytes, 1 configured symbols.
 */
#include "types.h"

extern u8 _ZTVN3abi17__class_type_infoE[];
extern char _ZTSN5mHeap6Heap_tE[];

/* 0x02099ce4: 8 bytes. */
u32 _ZTIN5mHeap6Heap_tE[2] = {
    (u32)_ZTVN3abi17__class_type_infoE,
    (u32)_ZTSN5mHeap6Heap_tE,
};
