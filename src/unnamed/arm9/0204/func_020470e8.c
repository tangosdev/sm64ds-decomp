#include "types.h"
#include "BMD_File.h"
#include "private/BMA_File.h"

int func_02046590(const void *model, const char *name);

/* dExtAnmMaterial_c::Prepare's body (see dExtAnmMaterial_c.h): give each
   of the material-animation file's material records the index of the
   BMD material its name names (func_02046590). */
void func_020470e8(struct BMD_File *model, BMA_File *file) {
    int i = 0;
    BMAMaterialRecord *rec = file->materialRecords;
    while (i < file->numMaterialRecords) {
        rec->materialId = func_02046590(model, rec->name);
        i++;
        rec++;
    }
}
