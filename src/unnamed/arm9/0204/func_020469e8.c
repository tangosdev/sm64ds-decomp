#include "types.h"
#include "dExtModel_c.h"
#include "private/BTA_File.h"
#include "nitro/os/system.h"

/* Texture-SRT (BTA) step: for each material record of the BTA file, write
 * the frame's scale, rotation and translation keys into the runtime material
 * the record drives. dExtAnmTexSRT_c (src/engine/model/d_ext_model.cpp) calls
 * it with the model's ModelComponents, its BTA_File and the integer frame.
 *
 * The 0x30-byte runtime material (MaterialState here, as in func_02046bbc.c;
 * dExtModel_c.h types ModelComponents::materials as BMD_Material, but its
 * builder in Model4.cpp lays it out differently) holds the texture SRT words
 * at 0x08..0x18. */
struct MaterialState {
    char pad_00[8];
    Fix12i texScaleS; /* 0x08 */
    Fix12i texScaleT; /* 0x0c */
    u16 texRotation;  /* 0x10 */
    u16 pad_12;
    Fix12i texTransS; /* 0x14 */
    Fix12i texTransT; /* 0x18 */
    char pad_1c[0x30 - 0x1c];
};

/* Spelled as Model4.cpp defines it. */
extern void _Z13func_02045e44Pvji(void *self, unsigned int value, int index);

void func_020469e8(struct ModelComponents *model, BTA_File *file, int frame) {
    int i;
    for (i = 0; i < file->numMaterialRecords; i++) {
        BTAMaterialRecord *rec = &file->materialRecords[i];
        struct MaterialState *mat;
        if (rec->materialId == 0xffff)
            _Z12OS_Terminatev();
        mat = &((struct MaterialState *)model->materials)[rec->materialId];
        _Z13func_02045e44Pvji(model, 1, rec->materialId);
        if (rec->scaleS.numKeys == 1)
            mat->texScaleS = file->keyScales[rec->scaleS.firstKey];
        else
            mat->texScaleS = file->keyScales[rec->scaleS.firstKey + frame];
        if (rec->scaleT.numKeys == 1)
            mat->texScaleT = file->keyScales[rec->scaleT.firstKey];
        else
            mat->texScaleT = file->keyScales[rec->scaleT.firstKey + frame];
        if (rec->rotation.numKeys == 1)
            mat->texRotation = file->keyRotations[rec->rotation.firstKey];
        else
            mat->texRotation = file->keyRotations[rec->rotation.firstKey + frame];
        if (rec->transS.numKeys == 1)
            mat->texTransS = file->keyTranslations[rec->transS.firstKey];
        else
            mat->texTransS = file->keyTranslations[rec->transS.firstKey + frame];
        if (rec->transT.numKeys == 1)
            mat->texTransT = file->keyTranslations[rec->transT.firstKey];
        else
            mat->texTransT = file->keyTranslations[rec->transT.firstKey + frame];
    }
}
