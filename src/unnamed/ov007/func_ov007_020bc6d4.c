#include "types.h"
#include "dExtModel_c.h"
#include "Ov007Model.h"
#include "Ov007MenuMem.h"
extern int _Z13func_02046564Pv(void *p);
extern void *_Z13func_020462bcPi(int *p);
extern void *func_ov007_020c92ac(void);
extern void func_ov007_020bc47c(Ov007Model *self);

/* Allocates a model record for the BMD file: its ModelComponents, one
 * texture word per material, the pose buffers the flags ask for, and no
 * animations bound yet; then func_ov007_020bc47c sets it up. */
Ov007Model *func_ov007_020bc6d4(struct BMD_File *file, int flags) {
    Ov007Model *m;
    struct ModelComponents *mc;
    int n;

    m = func_ov007_020c3df4(0, sizeof(Ov007Model));
    m->model = func_ov007_020c3df4(0, sizeof(struct ModelComponents));
    mc = m->model;
    m->materialTexParams = func_ov007_020c3df4(0, file->numMaterials << 2);
    m->flags = flags;
    m->numBoneAnims = 0;
    m->boneAnims = 0;
    m->boneAnim = 0;
    m->numTexSrtAnims = 0;
    m->texSrtAnims = 0;
    m->texSrtAnim = 0;
    m->numTexSeqAnims = 0;
    m->texSeqAnims = 0;
    m->texSeqAnim = 0;
    n = _Z13func_02046564Pv(file);
    m->transforms = func_ov007_020c3df4(0, n);
    if (m->flags & 2)
        m->nextTransforms = func_ov007_020c3df4(0, n);
    else
        m->nextTransforms = 0;
    if (m->flags & 4) {
        m->blendBuffer = func_ov007_020c3df4(0, _Z13func_020462bcPi(file));
        m->blendAnim = func_ov007_020c92ac();
    } else {
        m->blendBuffer = 0;
        m->blendAnim = 0;
    }
    mc->modelFile = file;
    func_ov007_020bc47c(m);
    return m;
}
