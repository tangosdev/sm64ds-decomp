#include "Ov007Model.h"
#include "dExtModel_c.h"
#include "BMD_File.h"

void func_ov007_020bbf98(Ov007Model *self, unsigned int lights) {
    int i;
    struct ModelComponents *m = self->model;
    int n = m->modelFile->numMaterials;
    for (i = 0; i < n; i++) {
        u32 *flags = &m->materials[i].flags;
        *flags = (*flags & ~15) | lights;
    }
}
