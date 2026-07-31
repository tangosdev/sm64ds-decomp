typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct MatAnimComp { u8 pad0; u8 which; u16 base; };
struct MatAnimEntry { u16 matIdx; u8 pad2[6]; struct MatAnimComp comp[13]; };
struct Material { u8 pad0[0x24]; u32 polyAttr; u32 diffAmb; u32 specEmi; };
struct MatAnimData { u8 pad0[4]; u8 *values; u16 count; u8 pad_a[2]; struct MatAnimEntry *entries; };
struct Model { u8 pad0[4]; struct Material *materials; };

extern void Crash(void);

#define LAUNDER(p) ((volatile u32 *)(int)(((long long)(int)(p))))

void func_02046e28(struct Model *model, struct MatAnimData *anim, short frame)
{
    u32 zero[1];
    u16 sel[2];
    int i;
    struct MatAnimEntry *e;
    int k;
    struct Material *mat;
    volatile u32 *pAttr;
    volatile u32 *pDiffAmb;
    volatile u32 *pSpecEmi;
    u32 c;
    u32 cb;
    sel[0] = 0;
    sel[1] = frame;

    i = 0;
    if (i < anim->count) {
    k = 0;
    zero[0] = 0;

    do {
        e = (struct MatAnimEntry *)((char *)anim->entries + k);
        if (e->matIdx == 0xffff)
            Crash();

        mat = &model->materials[e->matIdx];
        mat->diffAmb = 0x8000;
        pDiffAmb = LAUNDER(&mat->diffAmb);

        c = (u32)anim->values[e->comp[0].base + sel[e->comp[0].which]];
        *pDiffAmb |= c;
        c = (u32)anim->values[e->comp[1].base + sel[e->comp[1].which]];
        *pDiffAmb |= c << 5;
        c = (u32)anim->values[e->comp[2].base + sel[e->comp[2].which]];
        *pDiffAmb |= c << 10;
        c = (u32)anim->values[e->comp[3].base + sel[e->comp[3].which]];
        *pDiffAmb |= c << 16;
        c = (u32)anim->values[e->comp[4].base + sel[e->comp[4].which]];
        *pDiffAmb |= c << 21;
        c = (u32)anim->values[e->comp[5].base + sel[e->comp[5].which]];
        *pDiffAmb |= c << 26;

        mat->specEmi = zero[0];
        pSpecEmi = LAUNDER(&mat->specEmi);
        c = (u32)anim->values[e->comp[6].base + sel[e->comp[6].which]];
        *pSpecEmi |= c;
        c = (u32)anim->values[e->comp[7].base + sel[e->comp[7].which]];
        *pSpecEmi |= c << 5;
        c = (u32)anim->values[e->comp[8].base + sel[e->comp[8].which]];
        *pSpecEmi |= c << 10;
        c = (u32)anim->values[e->comp[9].base + sel[e->comp[9].which]];
        *pSpecEmi |= c << 16;
        c = (u32)anim->values[e->comp[10].base + sel[e->comp[10].which]];
        *pSpecEmi |= c << 21;
        cb = (u32)anim->values[e->comp[11].base + sel[e->comp[11].which]];
        *pSpecEmi |= cb << 26;

        pAttr = LAUNDER(&mat->polyAttr);
        *pAttr &= 0xffe0ffffU;
        c = (u32)anim->values[e->comp[12].base + sel[e->comp[12].which]];
        *pAttr |= c << 16;

        i++;
        k += 0x3c;
    } while (i < anim->count);
    }
}
