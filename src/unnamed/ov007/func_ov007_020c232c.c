#include "types.h"
#include "Ov007Light.h"
#include "nitro/gx/g3imm_light.h"

/* Loads an enabled light into the geometry engine: its direction and its
 * color, under the light number it was built for. */
void func_ov007_020c232c(Ov007Light *light) {
    if (light->enabled == 0)
        return;
    G3_LightVector(light->id, light->dirX, light->dirY, light->dirZ);
    G3_LightColor(light->id, light->color);
}
