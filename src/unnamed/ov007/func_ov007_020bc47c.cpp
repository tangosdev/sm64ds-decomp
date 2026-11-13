//cpp
#include "dExtSimpleModel_c.h"
#include "Ov007Model.h"

void func_020462d0(void *a, void *b, void *c);
void func_020462b4(int *p, int v);
extern "C" void func_ov007_020bc434(Ov007Model *m);

/* Binds the model's file, loads its textures and palettes unless flags & 8,
   resets the placement, snapshots each BMD material's +0x1c word (no name in
   BMD_File.h's pad) into materialTexParams, and shows the model with no
   cross-fade running. */
extern "C" void func_ov007_020bc47c(Ov007Model *self) {
    ModelComponents *m = self->model;

    if ((self->flags & 8) == 0) {
        dExtSimpleModel_c::LoadTexAndPal(*m->modelFile);
    }
    func_020462d0(m, m->modelFile, self->transforms);
    if ((self->flags & 4) != 0) {
        func_020462b4((int *)m, (int)self->blendBuffer);
    }
    func_ov007_020bc434(self);

    {
        int cnt = self->model->modelFile->numMaterials;
        int i = 0;
        if (cnt > 0) {
            do {
                self->materialTexParams[i] =
                    *(int *)((unsigned char *)self->model->materials + i * 0x30 + 0x1c);
                i++;
            } while (i < cnt);
        }
    }

    self->visible = 1;
    self->animTick = 1;
    self->blendFrameCount = 0;
    self->blendFrames = self->blendFrameCount;
}
