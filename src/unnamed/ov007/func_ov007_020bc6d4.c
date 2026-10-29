#include "types.h"
#include "dExtModel_c.h"
extern void *func_ov007_020c3df4(int a, int b);
extern int func_02046564(void *p);
extern int func_020462bc(void *p);
extern void *func_ov007_020c92ac(void);
extern void func_ov007_020bc47c(void *thiz);

/* The 0xa0-byte model record this function allocates; names as in
 * func_ov007_020bc02c, only the slots this file touches. The three
 * animation slots are (count, table, func_ov007_020c92ac player) triples
 * that func_ov007_020bc6b4, func_ov007_020bc658 and func_ov007_020bc5fc
 * bind. */
typedef struct Ov007Model {
    struct ModelComponents *model; /* 0x00 */
    void *transforms; /* 0x04: func_02046564-sized, as dExtSimpleModel_c's transformsBuf */
    void *nextTransforms;          /* 0x08: flags & 2 only */
    void *blendBuffer;  /* 0x0c: flags & 4 only; func_020462bc-sized, as dExtBlendAnmModel_c's */
    int numBoneAnims;   /* 0x10 */
    void *boneAnims;    /* 0x14: BCA_File table (UpdateBones) */
    void *boneAnim;     /* 0x18 */
    int numTexSrtAnims; /* 0x1c */
    void *texSrtAnims;  /* 0x20: BTA_File table (func_020469e8, dExtAnmTexSRT_c) */
    void *texSrtAnim;   /* 0x24 */
    int numTexSeqAnims; /* 0x28 */
    void *texSeqAnims;  /* 0x2c: BTP_File table (func_02046bbc, dExtAnmTexPat_c) */
    void *texSeqAnim;   /* 0x30 */
    char pad_34[0x50];
    /* 0x84, medium confidence: one word per BMD material, allocated here;
     * func_ov007_020bc47c copies each material's +0x1c into it, the word
     * func_02044b30 writes to REG_TEXIMAGE_PARAM (0x040004a8). Nothing in
     * ov007 reads it back; func_ov007_020bc53c frees it. */
    int *materialTexParams;
    void *blendAnim; /* 0x88: flags & 4 only; func_ov007_020bc7dc's cross-fade source */
    char pad_8c[0x10];
    int flags; /* 0x9c: 1/2 half-rate animation, 4 cross-fade, 8 no LoadTexAndPal */
} Ov007Model;

typedef char Ov007Model_size_must_be_0xa0[sizeof(Ov007Model) == 0xa0 ? 1 : -1];

/* r6: the BMD file; r7: the flags. */
void *func_ov007_020bc6d4(char *r6, void *r7)
{
    char *o;
    Ov007Model *m;
    struct ModelComponents *mc;
    int n;

    o = (char *)func_ov007_020c3df4(0, 0xa0);
    m = (Ov007Model *)o;
    m->model = func_ov007_020c3df4(0, 0x14);
    mc = m->model;
    m->materialTexParams = func_ov007_020c3df4(0, ((struct BMD_File *)r6)->numMaterials << 2);
    m->flags = (int)r7;
    m->numBoneAnims = 0;
    m->boneAnims = 0;
    m->boneAnim = 0;
    m->numTexSrtAnims = 0;
    m->texSrtAnims = 0;
    m->texSrtAnim = 0;
    m->numTexSeqAnims = 0;
    m->texSeqAnims = 0;
    m->texSeqAnim = 0;
    n = func_02046564(r6);
    m->transforms = func_ov007_020c3df4(0, n);
    if (m->flags & 2)
        m->nextTransforms = func_ov007_020c3df4(0, n);
    else
        m->nextTransforms = 0;
    if (m->flags & 4) {
        m->blendBuffer = func_ov007_020c3df4(0, func_020462bc(r6));
        m->blendAnim = func_ov007_020c92ac();
    } else {
        m->blendBuffer = 0;
        m->blendAnim = 0;
    }
    mc->modelFile = (struct BMD_File *)r6;
    func_ov007_020bc47c(o);
    return o;
}
