//cpp
#include "Ov007Model.h"

extern "C" void func_ov007_020c92a0(void *p);
extern "C" void func_ov007_020c3d1c(void *p);

/* Frees a model record: the four animation players go back through
   func_ov007_020c92a0, the buffers and the record through
   func_ov007_020c3d1c. */
void func_ov007_020bc53c(Ov007Model *m) {
    if (m->boneAnim) {
        func_ov007_020c92a0(m->boneAnim);
        m->boneAnim = 0;
    }
    if (m->texSrtAnim) {
        func_ov007_020c92a0(m->texSrtAnim);
        m->texSrtAnim = 0;
    }
    if (m->texSeqAnim) {
        func_ov007_020c92a0(m->texSeqAnim);
        m->texSeqAnim = 0;
    }
    if (m->blendAnim) {
        func_ov007_020c92a0(m->blendAnim);
        m->blendAnim = 0;
    }
    if (m->blendBuffer) {
        func_ov007_020c3d1c(m->blendBuffer);
        m->blendBuffer = 0;
    }
    if (m->nextTransforms) {
        func_ov007_020c3d1c(m->nextTransforms);
        m->nextTransforms = 0;
    }
    func_ov007_020c3d1c(m->materialTexParams);
    func_ov007_020c3d1c(m->transforms);
    func_ov007_020c3d1c(m->model);
    func_ov007_020c3d1c(m);
}
