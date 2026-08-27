//cpp
// @symbol _ZN5Model17UpdateFileOffsetsER8BMD_File
#include "Model.h"

/* Every offset in a freshly loaded BMD file is file-relative; this walks the
   header and rebases each one into a real pointer. */

#define REBASE(p) ((char *)(p) + base)

void Model::UpdateFileOffsets(BMD_File &file)
{
    u32 i;
    int k;
    int base = (int)&file;

    if (file.bones) file.bones = (BMD_Bone *)REBASE(file.bones);
    for (i = 0; i < file.numBones; i++) {
        BMD_Bone *b = file.bones + i;
        if (b->unk_04) b->unk_04 = REBASE(b->unk_04);
        if (b->materialIds) b->materialIds = (u8 *)REBASE(b->materialIds);
        if (b->unk_38) b->unk_38 = REBASE(b->unk_38);
    }

    if (file.displayListGroups)
        file.displayListGroups = (BMD_DisplayListGroup *)REBASE(file.displayListGroups);
    for (i = 0; i < file.numDisplayListGroups; i++) {
        BMD_DisplayListGroup *g = file.displayListGroups + i;
        if (g->lists) g->lists = (BMD_DisplayList *)((int)g->lists + base);
        for (k = 0; k < (int)g->numLists; k++) {
            BMD_DisplayList *d = g->lists + k;
            if (d->unk_04) d->unk_04 = (void *)((int)d->unk_04 + base);
            if (d->data) d->data = (void *)((int)d->data + base);
        }
    }

    if (file.textures) file.textures = (BMD_Texture *)REBASE(file.textures);
    for (i = 0; i < file.numTextures; i++) {
        BMD_Texture *e = file.textures + i;
        if (e->name) e->name = REBASE(e->name);
        if (e->data) e->data = REBASE(e->data);
    }

    if (file.palettes) file.palettes = (BMD_Palette *)REBASE(file.palettes);
    for (i = 0; i < file.numPalettes; i++) {
        BMD_Palette *e = file.palettes + i;
        if (e->name) e->name = REBASE(e->name);
        if (e->data) e->data = REBASE(e->data);
    }

    if (file.materials) file.materials = (BMD_Material *)REBASE(file.materials);
    for (i = 0; i < file.numMaterials; i++) {
        BMD_Material *e = file.materials + i;
        if (e->name) e->name = REBASE(e->name);
    }

    if (file.unk_2c) file.unk_2c = REBASE(file.unk_2c);
    if (file.unk_30 == 0) return;
    if (file.unk_34) file.unk_34 = REBASE(file.unk_34);
    {
        void **e = (void **)file.unk_34;
        if (e[0]) e[0] = REBASE(e[0]);
        if (e[1]) e[1] = REBASE(e[1]);
        if (e[2]) e[2] = REBASE(e[2]);
        if (e[3]) e[3] = REBASE(e[3]);
    }
}
