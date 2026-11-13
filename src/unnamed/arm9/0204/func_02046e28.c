#include "types.h"
#include "nitro/os/system.h"
struct MatAnimComp { u8 pad0; u8 which; u16 base; };
struct MatAnimEntry { u16 matIdx; u8 pad2[6]; struct MatAnimComp comp[13]; };
struct Material { u8 pad0[0x24]; u32 polyAttr; u32 diffAmb; u32 specEmi; };
struct MatAnimData { u8 pad0[4]; u8 *values; u16 count; u8 pad_a[2]; struct MatAnimEntry *entries; };
struct Model { u8 pad0[4]; struct Material *materials; };

/* dExtAnmMaterial_c::Update's body: for each animated material, set its
   13 colour channels for this frame. Each channel's value is
   values[base + sel[which]], with sel {0, frame}, so a channel is either
   constant or one value per frame. They are packed in GX's layout:
   diffuse and ambient into diffAmb (with bit 15, set vertex colour),
   specular and emission into specEmi, and the alpha into polygon
   attribute bits 16-20. A material index of 0xffff crashes. */
void func_02046e28(struct Model *model, struct MatAnimData *anim, unsigned short frame) {
    u16 sel[2];
    int i;
    struct MatAnimEntry *e;
    struct Material *mat;
    u32 c;
    u32 cb; /* channel 11 only: reusing c moves 7 words of register
               allocation, and inlining every read into its |= moves 53 */
    sel[0] = 0;
    sel[1] = frame;

    for (i = 0; i < anim->count; i++) {
        e = &anim->entries[i];
        if (e->matIdx == 0xffff)
            _Z12OS_Terminatev();

        mat = &model->materials[e->matIdx];
        mat->diffAmb = 0x8000;

        c = anim->values[e->comp[0].base + sel[e->comp[0].which]];
        mat->diffAmb |= c;
        c = anim->values[e->comp[1].base + sel[e->comp[1].which]];
        mat->diffAmb |= c << 5;
        c = anim->values[e->comp[2].base + sel[e->comp[2].which]];
        mat->diffAmb |= c << 10;
        c = anim->values[e->comp[3].base + sel[e->comp[3].which]];
        mat->diffAmb |= c << 16;
        c = anim->values[e->comp[4].base + sel[e->comp[4].which]];
        mat->diffAmb |= c << 21;
        c = anim->values[e->comp[5].base + sel[e->comp[5].which]];
        mat->diffAmb |= c << 26;

        mat->specEmi = 0;
        c = anim->values[e->comp[6].base + sel[e->comp[6].which]];
        mat->specEmi |= c;
        c = anim->values[e->comp[7].base + sel[e->comp[7].which]];
        mat->specEmi |= c << 5;
        c = anim->values[e->comp[8].base + sel[e->comp[8].which]];
        mat->specEmi |= c << 10;
        c = anim->values[e->comp[9].base + sel[e->comp[9].which]];
        mat->specEmi |= c << 16;
        c = anim->values[e->comp[10].base + sel[e->comp[10].which]];
        mat->specEmi |= c << 21;
        cb = anim->values[e->comp[11].base + sel[e->comp[11].which]];
        mat->specEmi |= cb << 26;

        mat->polyAttr &= 0xffe0ffffU;
        c = anim->values[e->comp[12].base + sel[e->comp[12].which]];
        mat->polyAttr |= c << 16;
    }
}
