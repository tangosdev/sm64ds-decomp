/* fBase vtable, display and IRQ state.
 * The _ZTV7fBase_c slot array, sIsDispOn, GXi_DmaId and OS_IRQTable.
 * Consumer: fBase_c, GX, OS interrupt dispatch.
 * ROM 0x02099fa8..0x02099fc4, 28 bytes, 5 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

extern void func_0204c24c();
extern void func_0204c194();
extern void func_0204c0a8();
extern void func_0204c0e8();

/* 0x02099fb4: 8 bytes. */
void (*data_02099fb4[])(unsigned char, unsigned char) = {
    (void (*)(unsigned char, unsigned char))&func_0204c24c,
    (void (*)(unsigned char, unsigned char))&func_0204c194,
};

/* 0x02099fb0: 4 bytes. */
int data_02099fb0 = 0x00000004;

/* 0x02099fac: 4 bytes. */
unsigned int data_02099fac = 0x00000226u;

/* 0x02099fa8: 4 bytes. */
unsigned int data_02099fa8 = 0x0000005au;

/* 0x02099fbc: 8 bytes. */
void (*data_02099fbc[])(int, int, void *) = {
    (void (*)(int, int, void *))&func_0204c0a8,
    (void (*)(int, int, void *))&func_0204c0e8,
};
