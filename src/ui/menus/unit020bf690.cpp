//cpp
/* unit020bf690 -- ov007 unit 0x020bf690..0x020bfd70: 7 functions, .text only.
 *
 * The title menu's screen fade. ov007 is the title and file-select menu
 * program that dScDSMT_c hosts; outside dScDSMT_c it has no ROM RTTI or
 * vtable, so no class name is claimed and the file keeps the unit<addr>
 * name.
 *
 * What the code does. The fade is one 0x14-byte record,
 * data_ov007_02104bd8, holding a screen transition (func_ov007_020c8b10's
 * Ov007Transition) and a full-screen model (menu file 0x24 built by
 * func_ov007_020bc6d4). func_ov007_020bfcec (called from the init
 * dispatcher, func_ov007_020b7138) builds both; func_ov007_020bfc54 (called
 * from func_ov007_020b6f4c, which tears every menu module down) frees them.
 * RequestScreenFade, which the rest of the menu calls, queues a fade kind
 * with its flags and end callback; func_ov007_020bf690 (called from
 * func_ov007_020b6eb4) starts a queued fade through func_ov007_020bf790,
 * which loads the kind's effect, curve and step into the transition, then
 * runs the transition one frame and, while flag 1 is set, scales the model
 * by its progress. func_ov007_020bfbdc places the model in front of a
 * camera and func_ov007_020bfaf0 sizes it to fill that camera's view and
 * renders it while it is visible.
 *
 * Edges: the bss word 0x02104bd8 is referenced by these functions and
 * nothing else in the overlay. The previous function, func_ov007_020bf630,
 * and the next one, func_ov007_020bfd70, reference no ov007 data; the next
 * one is called only from func_ov007_020be0dc / 020be7a0 (the module whose
 * bss block is 0x02104bc0), so the right edge is medium.
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 *
 * The functions keep their ROM names under C linkage.
 */
#include "types.h"
#include "common.h"
#include "cstd.h"
#include "nitro/fx/fx.h"
#include "G2x.h"
#include "Ov007Camera.h"
#include "Ov007Transition.h"
#include "Ov007Model.h"

extern "C" {

/* A BMD_Material's two colour words. */
typedef struct Ov007MaterialColors {
    char pad_00[0x28];
    u32 diffuseAmbient;   /* 0x28 */
    u32 specularEmission; /* 0x2c */
} Ov007MaterialColors;

/* ModelComponents' first two words, read through Ov007Model::model: BMD_File.h
 * leaves the material colour words unnamed (pad_28), so this view names them. */
typedef struct Ov007ModelParts {
    void *modelFile;                /* 0x00 */
    Ov007MaterialColors *materials; /* 0x04 */
} Ov007ModelParts;

/* The 0x14-byte screen-fade record at data_ov007_02104bd8. */
typedef struct Ov007Fade {
    Ov007Transition *transition; /* 0x00 */
    Ov007Model *model;           /* 0x04 */
    int flags;                   /* 0x08: RequestScreenFade's second argument */
    void (*onEnd)(void);         /* 0x0c: RequestScreenFade's third */
    /* 0x10: RequestScreenFade's first, the fade kind; func_ov007_020bf690
     * starts it and clears it */
    int request;
} Ov007Fade;

typedef char Ov007Fade_size_must_be_0x14[sizeof(Ov007Fade) == 0x14 ? 1 : -1];

extern Ov007Fade *data_ov007_02104bd8;
extern const short data_02082214[];

extern void InitialiseVramGlobals(void);
extern int InvMat4x3(int *src, int *dst);
extern void MulVec3Mat4x3(Vector3 *in, Matrix4x3 *mat, Vector3 *out);
extern void func_ov007_020b3edc(int size);
extern void *func_ov007_020b3f20(int idx);
extern void func_ov007_020bbff0(void *c);
extern void func_ov007_020bc02c(void *a, void *b);
extern void func_ov007_020bc47c(void *thiz);
extern void func_ov007_020bc53c(void *p);
extern Ov007Model *func_ov007_020bc6d4(void *file, int flags);
extern void func_ov007_020bdeb0(int a);
extern void func_ov007_020c3d1c(void *self);
extern void *func_ov007_020c3df4(const char *name, int size);
extern void func_ov007_020c8498(Ov007Transition *tr);
extern void func_ov007_020c8688(Ov007Transition *tr, int v);
extern void func_ov007_020c86a8(Ov007Transition *tr, int v);
extern void func_ov007_020c8b04(void *p);
extern Ov007Transition *func_ov007_020c8b10(void);

#define FADE data_ov007_02104bd8

// @symbol func_ov007_020bfcec
void func_ov007_020bfcec(void) {
    void *file;

    data_ov007_02104bd8 = (Ov007Fade *)func_ov007_020c3df4(0, sizeof(Ov007Fade));
    FADE->transition = func_ov007_020c8b10();
    InitialiseVramGlobals();
    file = func_ov007_020b3f20(0x24);
    FADE->model = func_ov007_020bc6d4(file, 0);
    FADE->model->visible = 0;
    FADE->request = 0;
    FADE->flags = 0;
    FADE->onEnd = 0;
}

// @symbol func_ov007_020bfc54
void func_ov007_020bfc54(void) {
    func_ov007_020b3edc(0x24);

    if (FADE->transition != 0) {
        func_ov007_020c8b04(FADE->transition);
        FADE->transition = 0;
    }

    if (FADE->model != 0) {
        func_ov007_020bc53c(FADE->model);
        FADE->model = 0;
    }

    if (data_ov007_02104bd8 == 0)
        return;

    func_ov007_020c3d1c(data_ov007_02104bd8);
    data_ov007_02104bd8 = 0;
}

// @symbol func_ov007_020bfbdc
/* Turns the model to face the screen and puts it 0x1064 in front of the
 * camera: the point (0, 0, -0x1064) taken back through the inverse of the
 * camera's view matrix. */
void func_ov007_020bfbdc(void *self) {
    Ov007Camera *cam = (Ov007Camera *)self;
    Ov007Model *model = FADE->model;
    Vector3 vec;
    Matrix4x3 mat;

    func_ov007_020bc47c(model);
    model->rotX = (short)cstd::div(0x5a0000, 0x168);
    model->visible = 0;

    vec.y = 0;
    vec.x = 0;
    vec.z = -0x1064;
    InvMat4x3((int *)&cam->viewMatrix, (int *)&mat);
    MulVec3Mat4x3(&vec, &mat, &model->pos);
}

// @symbol func_ov007_020bfaf0
/* While the model is visible, scales it in x and z to fill the camera's view
 * at its fovy, then updates it under the camera's view matrix and renders
 * it. */
void func_ov007_020bfaf0(Ov007Camera *cam) {
    Ov007Model *model = FADE->model;
    if (model->visible == 0)
        return;
    unsigned int idx = cam->fovy >> 4;
    Fix12i t1 = FX_MUL(data_02082214[idx * 2], 0x1064);
    Fix12i base = model->scale.x * 0xd800 + 0x12c000;
    Fix12i t2 = FX_MUL(base, t1);
    int denom = (short)data_02082214[idx * 2 + 1] * 0x60;
    Fix12i d = cstd::fdiv(t2, denom);
    model->scale.z = cstd::fdiv(d, 0x5000);
    model->scale.x = model->scale.z;
    func_ov007_020bc02c(model, &cam->viewMatrix);
    func_ov007_020bbff0(model);
}

// @symbol RequestScreenFade
/* Queues fade kind `kind` (1..9, see func_ov007_020bf790) with flags and
 * the end callback onEnd (0 for none); func_ov007_020bf690 starts it on its
 * next run. */
void RequestScreenFade(int kind, int flags, void (*onEnd)(void)) {
    FADE->request = kind;
    FADE->flags = flags;
    FADE->onEnd = onEnd;
}

// @symbol func_ov007_020bf790
/* Starts the queued fade: loads kind `request` (1..9) into the transition,
 * shows or hides the model by flag 1 (with sound 0x30 / 0x31 by flag 2),
 * and runs the transition forwards (flag 2) or backwards on the screens the
 * flags pick. */
void func_ov007_020bf790(void) {
    Ov007Fade *fade = FADE;
    Ov007Transition *tr = fade->transition;
    int flags = fade->flags;
    int kind = fade->request;
    int screens = 2;

    switch (kind) {
    default:
        break;
    case 1:
        tr->effect = 4;
        tr->endAction = 1;
        tr->curve = 0;
        tr->planes = 0x3F;
        tr->step = 0x3C;
        tr->fx.brightness.from = 0x10;
        tr->fx.brightness.to = 0;
        break;
    case 2:
        tr->effect = 4;
        tr->endAction = 1;
        tr->curve = 0;
        tr->planes = 0x3F;
        tr->step = 0x3C;
        tr->fx.brightness.from = -0x10;
        tr->fx.brightness.to = 0;
        break;
    case 3:
        tr->effect = 4;
        tr->endAction = 0;
        tr->curve = 0;
        tr->planes = 0x2F;
        tr->step = 0x3C;
        tr->fx.brightness.from = -0x10;
        tr->fx.brightness.to = 0;
        break;
    case 4:
        tr->effect = 4;
        tr->endAction = 0;
        tr->curve = 0;
        tr->planes = 0x3F;
        tr->step = 0x100;
        tr->fx.brightness.from = -0x10;
        tr->fx.brightness.to = 0;
        break;
    case 5:
        tr->effect = 4;
        tr->endAction = 1;
        tr->curve = 0;
        tr->planes = 0x3F;
        tr->step = 0x3C;
        tr->fx.brightness.from = 0;
        tr->fx.brightness.to = 0;
        break;
    case 6:
        tr->effect = 4;
        tr->endAction = 1;
        tr->curve = 0;
        tr->planes = 0x3F;
        tr->step = 0x1000;
        tr->fx.brightness.from = -0x10;
        tr->fx.brightness.to = -0x10;
        break;
    case 7:
        tr->effect = 4;
        tr->endAction = 1;
        tr->curve = 0;
        tr->planes = 0x2F;
        tr->step = 0x1000;
        tr->fx.brightness.from = -0x10;
        tr->fx.brightness.to = -0x10;
        break;
    case 8:
        tr->effect = 4;
        tr->endAction = 1;
        tr->curve = 0;
        tr->planes = 0x3D;
        tr->step = 0x100;
        tr->fx.brightness.from = 0;
        tr->fx.brightness.to = -9;
        break;
    case 9:
        tr->effect = 3;
        tr->endAction = 1;
        tr->curve = 0;
        tr->step = 0x190;
        tr->planes = 1;
        tr->fx.alpha.planes2 = 0x14;
        tr->fx.alpha.evaFrom = 0x10;
        tr->fx.alpha.evaTo = 0;
        tr->fx.alpha.evbFrom = 0;
        tr->fx.alpha.evbTo = 0;
        break;
    }

    if (flags & 1) {
        int color;
        Ov007ModelParts *parts = (Ov007ModelParts *)FADE->model->model;
        if (kind == 1) {
            color = 0x7FFF;
        } else {
            color = 0;
        }
        color = color | (color << 0x10);
        parts->materials->diffuseAmbient = color;
        parts->materials->specularEmission = color;
        FADE->model->visible = 1;
        if (flags & 2) {
            func_ov007_020bdeb0(0x30);
        } else {
            func_ov007_020bdeb0(0x31);
        }
    } else {
        if (flags & 4) {
            screens = 1;
        } else if (!(flags & 8)) {
            screens |= 1;
        }
        FADE->model->visible = 0;
    }

    if (flags & 0x10) {
        G2x::SetBlendBrightness((volatile unsigned short *)0x4000050, 0x3F, 0);
    }

    if (flags & 2) {
        func_ov007_020c86a8(tr, screens);
    } else {
        func_ov007_020c8688(tr, screens);
    }

    FADE->transition->onEnd = FADE->onEnd;
}

// @symbol func_ov007_020bf690
/* One frame: starts a queued fade, runs the transition, and while flag 1 is
 * set scales the model in x and z by the transition's progress; when the
 * transition holds at its end it hides the model, clears flag 1 and, for a
 * reversed run, restores full brightness-down. */
void func_ov007_020bf690(void) {
    Ov007Fade *fade = FADE;
    Ov007Transition *tr = fade->transition;
    int flags = fade->flags;
    int started = 0;
    if (fade->request != 0) {
        func_ov007_020bf790();
        FADE->request = 0;
        started = 1;
    }
    func_ov007_020c8498(tr);
    if ((flags & 1) == 0)
        return;
    int progress = tr->t;
    if (started == 0 && (flags & 2) != 0) {
        G2x::SetBlendBrightness((volatile unsigned short *)0x4000050, 0x3f, 0);
    }
    if (tr->reversed != 0)
        progress = 0x1000 - progress;
    FADE->model->scale.z = progress;
    FADE->model->scale.x = FADE->model->scale.z;
    if (tr->state != 3)
        return;
    FADE->model->visible = 0;
    {
        const Ov007Fade *cur = FADE;
        FADE->flags = cur->flags & ~1;
    }
    if (tr->reversed == 0)
        return;
    G2x::SetBlendBrightness((volatile unsigned short *)0x4000050, 0x3f, -0x10);
}
}
