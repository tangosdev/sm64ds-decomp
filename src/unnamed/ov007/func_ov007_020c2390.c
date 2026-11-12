/* recovered: shared common types */
#include "common.h"
#include "Ov007Light.h"
#include "math/CalcLib.h"
extern void VEC_Normalize(const struct Vector3 *a, struct Vector3 *b);

/* Rederives the light's direction from its two points. */
void func_ov007_020c2390(Ov007Light *light) {
    struct Vector3 v;
    VEC_Subtract(&light->target, &light->source, &v);
    VEC_Normalize(&v, &v);
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
