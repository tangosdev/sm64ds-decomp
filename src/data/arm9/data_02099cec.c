/* dBgW RTTI and the mHeap typeinfo records.
 * _ZTS/_ZTI/_ZTV of dBgW_Kc, dBgW_KcMbg, dBgW_KcMbgSclY and dM3dGSph, then
 * the mHeap Heap_t/ExpHeap_t/SolidHeap_t typeinfo records.
 * ROM 0x02099cec..0x02099d28, 60 bytes, 4 configured symbols.
 * A _ZTI record's first word is its abi RTTI vtable address
 * (__si_class_type_info 0x0209a764); it stays a number because the tree
 * declares no _ZTV array.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

extern char _ZTSN5mHeap9ExpHeap_tE[20];
extern u32 _ZTIN5mHeap6Heap_tE[];
extern u8 _ZTSN5mHeap11SolidHeap_tE[];

/* 0x02099d04: 16 bytes. */
char _ZTSN5mHeap6Heap_tE[16] = "N5mHeap6Heap_tE";

/* 0x02099cf8: 12 bytes. */
u32 _ZTIN5mHeap11SolidHeap_tE[3] = {
    0x0209a764,
    (u32)_ZTSN5mHeap11SolidHeap_tE,
    (u32)_ZTIN5mHeap6Heap_tE,
};

/* 0x02099cec: 12 bytes. */
u32 _ZTIN5mHeap9ExpHeap_tE[3] = {
    0x0209a764,
    (u32)_ZTSN5mHeap9ExpHeap_tE,
    (u32)_ZTIN5mHeap6Heap_tE,
};

/* 0x02099d14: 20 bytes. */
char _ZTSN5mHeap9ExpHeap_tE[20] = "N5mHeap9ExpHeap_tE";
