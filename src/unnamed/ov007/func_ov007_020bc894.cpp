//cpp
#include "dExtModel_c.h"

extern "C" {
extern void func_ov007_020c9110(void*);
extern void func_02046bbc(void*, void*, int);
extern void func_020469e8(void*, void*, int);

/* The 0x1c-byte animation player func_ov007_020c92ac allocates and
 * func_ov007_020c9110 ticks; names as in func_ov007_020bc02c, only the slots
 * this file reads. */
struct Ov007AnimPlayer {
    char pad_00[0xc];
    short anim; /* 0x0c: index into the owner's animation table; -1 none */
    char pad_0e[2];
    unsigned short frame; /* 0x10: the whole frame func_ov007_020c9110 last reached */
};

/* The 0xa0-byte model record func_ov007_020bc6d4 allocates; names as there,
 * only the slots this file reads. Each animation slot is a (table, player)
 * pair. */
struct Ov007Model {
    ModelComponents *model; /* 0x00 */
    char pad_04[0x10];
    BCA_File **boneAnims;      /* 0x14: UpdateBones */
    Ov007AnimPlayer *boneAnim; /* 0x18 */
    char pad_1c[4];
    void **texSrtAnims;          /* 0x20: BTA_File table (func_020469e8, dExtAnmTexSRT_c) */
    Ov007AnimPlayer *texSrtAnim; /* 0x24 */
    char pad_28[4];
    void **texSeqAnims;          /* 0x2c: BTP_File table (func_02046bbc, dExtAnmTexPat_c) */
    Ov007AnimPlayer *texSeqAnim; /* 0x30 */
};

typedef char Ov007Model_size_must_be_0x34[sizeof(Ov007Model) == 0x34 ? 1 : -1];

/* Ticks the model's bone, texture-sequence and texture-SRT players and
 * applies each one's current frame of its animation. c keeps the char *
 * spelling of the callers' declarations; `m` is the typed view. */
void func_ov007_020bc894(char* c){
    Ov007Model *m = (Ov007Model *)c;
    void *model = m->model;
    Ov007AnimPlayer *p;
    short idx;
    p = m->boneAnim;
    if (p) {
        func_ov007_020c9110(p);
        p = m->boneAnim;
        idx = p->anim;
        if (idx != -1) {
            ((ModelComponents *)model)->UpdateBones(m->boneAnims[idx], p->frame);
        }
    }
    p = m->texSeqAnim;
    if (p) {
        func_ov007_020c9110(p);
        p = m->texSeqAnim;
        idx = p->anim;
        if (idx != -1) {
            func_02046bbc(model, m->texSeqAnims[idx], p->frame);
        }
    }
    p = m->texSrtAnim;
    if (p) {
        func_ov007_020c9110(p);
        p = m->texSrtAnim;
        idx = p->anim;
        if (idx != -1) {
            func_020469e8(model, m->texSrtAnims[idx], p->frame);
        }
    }
}
}
