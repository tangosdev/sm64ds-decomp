// @symbol func_ov007_020c2390
/* recovered: shared common types */
#include "common.h"
extern void SubVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern void NormalizeVec3(struct Vector3 *a, struct Vector3 *b);

/* The 0x28-byte light func_ov007_020c241c builds; dirX/dirY/dirZ as in
 * func_ov007_020c232c, only the slots this function touches. */
typedef struct Ov007Light {
    char pad_00[4];
    /* 0x04/0x10, medium confidence: the light shines from `source` towards
     * `target` (this function's direction is target - source).
     * func_ov007_020c241c seeds target at the origin and source at
     * data_ov007_020d7dac, (0, 0, 1.0); nothing else writes them. */
    struct Vector3 target;
    struct Vector3 source;
    /* 0x1c: the s16 unit direction, each axis clamped to 0xfff */
    s16 dirX;
    s16 dirY;
    s16 dirZ;
} Ov007Light;

typedef char Ov007Light_dirZ_must_be_at_0x20[(int)&((Ov007Light *)0)->dirZ == 0x20 ? 1 : -1];

/* Rederives the light's direction from its two points. t keeps the char *
 * spelling of func_ov007_020c241c's declaration; `light` is the typed view. */
void func_ov007_020c2390(char *t)
{
    Ov007Light *light = (Ov007Light *)t;
    struct Vector3 v;
    SubVec3(&light->target, &light->source, &v);
    NormalizeVec3(&v, &v);
    light->dirX = v.x;
    light->dirY = v.y;
    light->dirZ = v.z;
    if (light->dirX > 0xfff)
        light->dirX = 0xfff;
    if (light->dirY > 0xfff)
        light->dirY = 0xfff;
    if (light->dirZ > 0xfff)
        light->dirZ = 0xfff;
}
