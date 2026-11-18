/* dBgW RTTI and the mHeap typeinfo records.
 * _ZTS/_ZTI/_ZTV of dBgW_Kc, dBgW_KcMbg, dBgW_KcMbgSclY and dM3dGSph, then
 * the mHeap Heap_t/ExpHeap_t/SolidHeap_t typeinfo records.
 * ROM 0x02099468..0x02099490, 40 bytes, 2 configured symbols.
 * A _ZTI record's first word is its abi RTTI vtable address
 * (__si_class_type_info 0x0209a764); it stays a number because the tree
 * declares no _ZTV array.
 */
#include "types.h"

extern u32 _ZTI14dBgW_KcMbgSclY[3];
extern u32 _ZTS14dBgW_KcMbgSclY[7];
extern u32 _ZTI10dBgW_KcMbg[];

/* 0x02099468: 12 bytes. */
u32 _ZTI14dBgW_KcMbgSclY[3] = {
    0x0209a764,
    (u32)_ZTS14dBgW_KcMbgSclY,
    (u32)_ZTI10dBgW_KcMbg,
};

/* 0x02099474: 28 bytes. */
u32 _ZTS14dBgW_KcMbgSclY[7] = {
    0x42643431,
    0x4b5f5767,
    0x67624d63,
    0x596c6353,
    0x00000000,
    0x00000000,
    (u32)_ZTI14dBgW_KcMbgSclY,
};
