/* Integer range limits used by the crash-screen and debug-font code.
 * data_0208e37c is INT_MIN and data_0208e380 is INT_MAX; both are declared in
 * include/decl_common.h.
 * ROM 0x0208e37c..0x0208e384, 8 bytes, 2 configured symbols.
 *
 * Order: mwcc emits one .data section per initialized object and the linker
 * concatenates them in declaration order, so the definitions below are in the
 * order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x0208e37c: 4 bytes. */
int data_0208e37c = (int)0x80000000u;

/* 0x0208e380: 4 bytes. */
int data_0208e380 = 0x7fffffff;
