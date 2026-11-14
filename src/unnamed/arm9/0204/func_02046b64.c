#include "types.h"
#include "BMD_File.h"
#include "private/BTA_File.h"

int _Z13func_02046590PKvPKc(const void *model, const char *name);

/* dExtAnmTexSRT_c::Prepare's body (see dExtAnmTexSRT_c.h): give each of
   the texture-SRT file's material records the index of the BMD material
   its name names (func_02046590). */
void func_02046b64(struct BMD_File *model, BTA_File *file) {
    int i;
    for (i = 0; i < (int)file->numMaterialRecords; i++) {
        BTAMaterialRecord *rec = &file->materialRecords[i];
        rec->materialId = _Z13func_02046590PKvPKc(model, rec->name);
    }
}
