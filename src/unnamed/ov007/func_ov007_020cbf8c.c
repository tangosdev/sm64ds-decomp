#include "types.h"
#include "Ov007Mesh.h"
#include "Ov007MenuMem.h"

/* Allocates a material, copied from src when given, else the defaults:
 * opaque, white diffuse, no texture. */
Ov007MeshMaterial *func_ov007_020cbf8c(Ov007MeshMaterial *src) {
    Ov007MeshMaterial *mat = func_ov007_020c3df4(0, sizeof(Ov007MeshMaterial));
    if (src != 0) {
        *mat = *src;
    } else {
        mat->texture = 0;
        mat->texT = 0;
        mat->texS = mat->texT;
        mat->warpT = 0;
        mat->warpS = mat->warpT;
        mat->alpha = 0x1f;
        mat->diffuse = 0x7fff;
        mat->ambient = 0;
        mat->polygonMode = 0;
        mat->polygonId = 0;
        mat->texRepeat = 0;
        mat->texColor0 = 0;
    }
    return mat;
}
