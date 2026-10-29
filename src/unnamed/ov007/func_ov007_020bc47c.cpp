//cpp
#include "dExtSimpleModel_c.h"

extern "C" void func_020462d0(void *a, void *b, int c);
extern "C" void func_020462b4(int *p, int v);
extern "C" void func_ov007_020bc434(void *c);

/* The 0xa0-byte model record func_ov007_020bc6d4 allocates; names as in
   func_ov007_020bc6d4, func_ov007_020bc02c and func_ov007_020bc7dc, only the
   slots this function sets. +0x84 (one word per BMD material, filled below)
   has no name in func_ov007_020bc6d4 either and stays raw on `c`. */
struct Ov007Model {
    ModelComponents *model; /* 0x00 */
    void *transforms;       /* 0x04: the pose model->transforms points at */
    char pad_08[4];
    void *blendBuffer; /* 0x0c: flags & 4 only */
    char pad_10[0x7c];
    short blendFrames;     /* 0x8c: frames left of func_ov007_020bc7dc's cross-fade */
    short blendFrameCount; /* 0x8e: the cross-fade's length */
    char pad_90[4];
    /* 0x94: func_ov007_020bbff0 renders the model only while it is nonzero;
       func_ov007_020be980 sets it with the animation, func_ov007_020b066c
       clears it to hide the model */
    int visible;
    int animTick; /* 0x98: 1 on the frames that step the animations */
    /* 0x9c: func_ov007_020bc6d4's second argument; 4 cross-fade buffers,
       8 skips dExtSimpleModel_c::LoadTexAndPal */
    int flags;
};

typedef char Ov007Model_size_must_be_0xa0[sizeof(Ov007Model) == 0xa0 ? 1 : -1];

/* Binds the model's file, loads its textures and palettes unless flags & 8,
   resets the placement, snapshots each BMD material's +0x1c word (no name in
   BMD_File.h's pad) into +0x84, and shows the model with no cross-fade
   running. thiz keeps the void * spelling of the callers' declarations;
   `self` is the typed view. */
extern "C" void func_ov007_020bc47c(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    Ov007Model *self = (Ov007Model *)thiz;
    ModelComponents *m = self->model;

    if ((self->flags & 8) == 0) {
        dExtSimpleModel_c::LoadTexAndPal(*m->modelFile);
    }
    func_020462d0(m, m->modelFile, (int)self->transforms);
    if ((self->flags & 4) != 0) {
        func_020462b4((int *)m, (int)self->blendBuffer);
    }
    func_ov007_020bc434(c);

    {
        int cnt = self->model->modelFile->numMaterials;
        int i = 0;
        if (cnt > 0) {
            do {
                int *arr = *(int **)(c + 0x84);
                arr[i] = *(int *)((unsigned char *)self->model->materials + i * 0x30 + 0x1c);
                i++;
            } while (i < cnt);
        }
    }

    self->visible = 1;
    self->animTick = 1;
    self->blendFrameCount = 0;
    self->blendFrames = self->blendFrameCount;
}
