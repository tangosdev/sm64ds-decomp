#include "types.h"

/* The 0x28-byte light func_ov007_020c241c allocates; color as in
 * func_ov007_020b1718 and func_ov007_020beeb0, the rest from
 * func_ov007_020c241c and func_ov007_020c2390. Only the slots this file
 * reads are spelled. */
typedef struct Ov007Light {
    int id; /* 0x00: func_ov007_020c241c's argument; the hardware light, 0..3 */
    char pad_04[0x18];
    /* 0x1c: the s16 unit direction func_ov007_020c2390 derives from the
     * +0x04/+0x10 points, each axis clamped to 0xfff */
    s16 dirX;
    s16 dirY;
    s16 dirZ;
    u16 color;   /* 0x22: func_ov007_020c241c seeds 0x7fff */
    int enabled; /* 0x24: func_ov007_020c241c sets 1; nothing is loaded while 0 */
} Ov007Light;

typedef char Ov007Light_size_must_be_0x28[sizeof(Ov007Light) == 0x28 ? 1 : -1];

/* Loads an enabled light into the geometry engine: its direction as three
 * 10-bit fractions into LIGHT_VECTOR (0x040004c8) and its color into
 * LIGHT_COLOR (0x040004cc), each tagged with the light number in bits 30-31.
 * o keeps the char * spelling of the definition; `light` is the typed view. */
void func_ov007_020c232c(char* o) {
    Ov007Light *light = (Ov007Light *)o;
    int a, b, c;
    if (light->enabled == 0)
        return;
    a = (light->dirX >> 3) & 0x3ff;
    b = (light->dirY >> 3) & 0x3ff;
    c = (light->dirZ >> 3) & 0x3ff;
    *(volatile unsigned *)0x40004c8 = a | (b << 10) | (c << 20) | (light->id << 30);
    *(volatile unsigned *)0x40004cc = (light->id << 30) | light->color;
}
