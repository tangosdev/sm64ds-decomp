//cpp
#include "dExtModel_c.h"
#include "Ov007Model.h"

extern "C" {
extern void func_ov007_020c9110(void *);
extern void func_02046bbc(void *, void *, unsigned int);
extern void func_020469e8(void *, void *, int);

/* Ticks the model's bone, texture-sequence and texture-SRT players and
 * applies each one's current frame of its animation. */
void func_ov007_020bc894(Ov007Model *m) {
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
