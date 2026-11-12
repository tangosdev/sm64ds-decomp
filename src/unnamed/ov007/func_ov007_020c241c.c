#include "types.h"
#include "Ov007Light.h"
#include "Ov007MenuMem.h"

extern Vector3 data_ov007_020d7dac;
extern void func_ov007_020c2390(Ov007Light *light);

/* Builds hardware light `id`: white, enabled, shining from
 * data_ov007_020d7dac towards the origin. */
Ov007Light *func_ov007_020c241c(int id) {
    Ov007Light *light = func_ov007_020c3df4(0, sizeof(Ov007Light));
    light->id = id;
    light->source = data_ov007_020d7dac;
    {
        Vector3 zero = {0, 0, 0};
        light->target = zero;
    }
    light->color = 0x7fff;
    light->enabled = 1;
    func_ov007_020c2390(light);
    return light;
}
