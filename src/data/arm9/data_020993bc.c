/* dBgW RTTI and the mHeap typeinfo records.
 * _ZTS/_ZTI/_ZTV of dBgW_Kc, dBgW_KcMbg, dBgW_KcMbgSclY and dM3dGSph, then
 * the mHeap Heap_t/ExpHeap_t/SolidHeap_t typeinfo records.
 * ROM 0x020993bc..0x020993dc, 32 bytes, 2 configured symbols.
 * A _ZTI record's first word is its abi RTTI vtable address
 * (__si_class_type_info 0x0209a764); it stays a number because the tree
 * declares no _ZTV array.
 */
#include "types.h"

extern char _ZTS7dBgW_Kc[12];
extern u32 _ZTI7dBgW_Kc[5];
extern u8 _ZTI4dBgW[];

/* 0x020993bc: 12 bytes. */
char _ZTS7dBgW_Kc[12] = "7dBgW_Kc";

/* 0x020993c8: 20 bytes. */
u32 _ZTI7dBgW_Kc[5] = {
    0x0209a764, (u32)_ZTS7dBgW_Kc, (u32)_ZTI4dBgW, 0x00000000, (u32)_ZTI7dBgW_Kc,
};
