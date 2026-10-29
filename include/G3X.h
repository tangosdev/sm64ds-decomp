/* Hand-edited, against evidence. G3X has no instance state: the three
 * functions write the 3D engine's fog and clear-color registers through
 * literal hardware addresses (0x4000350/0x4000354 the clear-color value and
 * depth, 0x400035c/0x4000360 the fog registers and 32-entry table, 0x4000060
 * the display control bits they read-modify-write). None takes a `this` --
 * every parameter comes from the mangled name, the same shape as
 * include/G2x.h.
 */
#ifndef G3X_H
#define G3X_H
#include "types.h"

struct G3X {
#ifdef __cplusplus
    /* All static -- see the header note. */
    static void SetClearColor(unsigned short a, int b, int c, int d, bool e);
    static void SetFogTable(void *table);
    static void SetFog(int enable, int a, int b, int c);
#endif
};

#endif
