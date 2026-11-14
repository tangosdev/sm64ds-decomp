#include "types.h"
#include "BMD_File.h"
#include "private/BTP_File.h"

int func_020471ac(const struct BMD_File *model, const char *name);
int func_02047144(const struct BMD_File *model, const char *name);
int func_02046590(const void *model, const char *name);

/* dExtAnmTexPat_c::Prepare's body (see dExtAnmTexPat_c.h): resolve the
   texture-pattern file's three name tables against the BMD, the first
   to texture indices (func_020471ac), the second to palette indices
   (func_02047144) and the third to material indices (func_02046590). */
void func_02046d50(struct BMD_File *model, BTP_File *file) {
    int i;
    BTPNameRecord *rec;

    rec = file->texRecords;
    for (i = 0; i < file->numTexRecords; i++, rec++) {
        rec->resourceId = func_020471ac(model, rec->name);
    }
    rec = file->palRecords;
    for (i = 0; i < file->numPalRecords; i++, rec++) {
        rec->resourceId = func_02047144(model, rec->name);
    }
    for (i = 0; i < file->numSeqRecords; i++) {
        file->seqRecords[i].materialId = func_02046590(model, file->seqRecords[i].name);
    }
}
