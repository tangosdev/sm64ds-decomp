//cpp
/* unit020bf690 -- ov007 unit 0x020bf690..0x020bfd70: 7 functions, .text only.
 *
 * The title menu's screen fade. ov007 is the title and file-select menu
 * program that dScDSMT_c hosts; outside dScDSMT_c it has no ROM RTTI or
 * vtable, so no class name is claimed and the file keeps the unit<addr>
 * name.
 *
 * What the code does. The fade is one 0x14-byte record, sScreenFade
 * (bss 0x02104bd8), holding a screen transition (Transition_Create's
 * Ov007Transition) and a full-screen model (menu file 0x24 built by
 * func_ov007_020bc6d4). ScreenFade_Create (called from the init
 * dispatcher, func_ov007_020b7138) builds both; ScreenFade_Destroy (called
 * from func_ov007_020b6f4c, which tears every menu module down) frees them.
 * RequestScreenFade, which the rest of the menu calls, queues a fade kind
 * with its flags and end callback; ScreenFade_Update (called from
 * func_ov007_020b6eb4) starts a queued fade through ScreenFade_Start,
 * which loads the kind's effect, curve and step into the transition, then
 * runs the transition one frame and, while flag 1 is set, scales the model
 * by its progress. func_ov007_020bfbdc places the model in front of a
 * camera and ScreenFade_Draw sizes it to fill that camera's view and
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
 * The ScreenFade_* names are coined from the bodies, after the module's
 * RequestScreenFade entry; ov007 records no names. All keep C linkage.
 * func_ov007_020bfbdc keeps its address name while a file another branch
 * holds calls it.
 */
#include "types.h"
#include "common.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "nitro/gx/g2.h"
#include "Ov007Camera.h"
#include "Ov007Transition.h"
#include "Ov007Model.h"
#include "dExtModel_c.h"
#include "nitro/hw/registers.h"
#include "nitro/fx/fx_cp.h"
#include "math/MtxStack.h"

extern "C" {

/* The 0x14-byte screen-fade record sScreenFade points at. */
typedef struct Ov007Fade {
    Ov007Transition *transition; /* 0x00 */
    Ov007Model *model;           /* 0x04 */
    int flags;                   /* 0x08: RequestScreenFade's second argument */
    void (*onEnd)(void);         /* 0x0c: RequestScreenFade's third */
    /* 0x10: RequestScreenFade's first, the fade kind; ScreenFade_Update
     * starts it and clears it */
    int request;
} Ov007Fade;

typedef char Ov007Fade_size_must_be_0x14[sizeof(Ov007Fade) == 0x14 ? 1 : -1];

extern Ov007Fade *sScreenFade;

extern void InitialiseVramGlobals(void);
extern void func_ov007_020b3edc(int size);
extern void *func_ov007_020b3f20(int idx);
extern void func_ov007_020bbff0(void *c);
extern void func_ov007_020bc02c(void *a, void *b);
extern void func_ov007_020bc47c(void *thiz);
extern void func_ov007_020bc53c(void *p);
extern Ov007Model *func_ov007_020bc6d4(void *file, int flags);
extern int func_ov007_020bdeb0(int a);
extern void func_ov007_020c3d1c(void *self);
extern void *func_ov007_020c3df4(const char *name, int size);
extern void Transition_Update(Ov007Transition *tr);
extern void Transition_StartBackward(Ov007Transition *tr, int v);
extern void Transition_StartForward(Ov007Transition *tr, int v);
extern void Transition_Destroy(void *p);
extern Ov007Transition *Transition_Create(void);

void ScreenFade_Create(void) {
    void *file;

    sScreenFade = (Ov007Fade *)func_ov007_020c3df4(0, sizeof(Ov007Fade));
    sScreenFade->transition = Transition_Create();
    InitialiseVramGlobals();
    file = func_ov007_020b3f20(0x24);
    sScreenFade->model = func_ov007_020bc6d4(file, 0);
    sScreenFade->model->visible = 0;
    sScreenFade->request = 0;
    sScreenFade->flags = 0;
    sScreenFade->onEnd = 0;
}

void ScreenFade_Destroy(void) {
    func_ov007_020b3edc(0x24);

    if (sScreenFade->transition != 0) {
        Transition_Destroy(sScreenFade->transition);
        sScreenFade->transition = 0;
    }

    if (sScreenFade->model != 0) {
        func_ov007_020bc53c(sScreenFade->model);
        sScreenFade->model = 0;
    }

    if (sScreenFade == 0)
        return;

    func_ov007_020c3d1c(sScreenFade);
    sScreenFade = 0;
}

/* Turns the model to face the screen and puts it 0x1064 in front of the
 * camera: the point (0, 0, -0x1064) taken back through the inverse of the
 * camera's view matrix. */
void func_ov007_020bfbdc(void *self) {
    Ov007Camera *cam = (Ov007Camera *)self;
    Ov007Model *model = sScreenFade->model;
    Vector3 vec;
    Matrix4x3 mat;

    func_ov007_020bc47c(model);
    model->rotX = FX_DivS32(0x5a0000, 0x168);
    model->visible = 0;

    vec.y = 0;
    vec.x = 0;
    vec.z = -0x1064;
    MTX_Inverse43(&cam->viewMatrix, &mat);
    MTX_MultVec43(&vec, &mat, &model->pos);
}

/* While the model is visible, scales it in x and z to fill the camera's view
 * at its fovy, then updates it under the camera's view matrix and renders
 * it. */
void ScreenFade_Draw(Ov007Camera *cam) {
    Ov007Model *model = sScreenFade->model;
    if (model->visible == 0)
        return;
    int fovyIdx = cam->fovy >> 4;
    Fix12i sinDist = FX_MUL(FX_SinIdx(fovyIdx), 0x1064);
    Fix12i size = model->scale.x * 0xd800 + 0x12c000;
    Fix12i scaled = FX_MUL(size, sinDist);
    int cosScaled = FX_CosIdx(fovyIdx) * 0x60;
    Fix12i extent = FX_Div(scaled, cosScaled);
    model->scale.z = FX_Div(extent, 0x5000);
    model->scale.x = model->scale.z;
    func_ov007_020bc02c(model, &cam->viewMatrix);
    func_ov007_020bbff0(model);
}

/* Queues fade kind `kind` (1..9, see ScreenFade_Start) with flags and
 * the end callback onEnd (0 for none); ScreenFade_Update starts it on its
 * next run. */
void RequestScreenFade(int kind, int flags, void (*onEnd)(void)) {
    sScreenFade->request = kind;
    sScreenFade->flags = flags;
    sScreenFade->onEnd = onEnd;
}

/* Starts the queued fade: loads kind `request` (1..9) into the transition,
 * shows or hides the model by flag 1 (with sound 0x30 / 0x31 by flag 2),
 * and runs the transition forwards (flag 2) or backwards on the screens the
 * flags pick. */
void ScreenFade_Start(void) {
    Ov007Fade *fade = sScreenFade;
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
        ModelComponents *parts = sScreenFade->model->model;
        if (kind == 1) {
            color = 0x7FFF;
        } else {
            color = 0;
        }
        color = color | (color << 0x10);
        parts->materials->diffuseAmbient = color;
        parts->materials->specularEmission = color;
        sScreenFade->model->visible = 1;
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
        sScreenFade->model->visible = 0;
    }

    if (flags & 0x10) {
        G2_SetBlendBrightness(0x3F, 0);
    }

    if (flags & 2) {
        Transition_StartForward(tr, screens);
    } else {
        Transition_StartBackward(tr, screens);
    }

    sScreenFade->transition->onEnd = sScreenFade->onEnd;
}

/* One frame: starts a queued fade, runs the transition, and while flag 1 is
 * set scales the model in x and z by the transition's progress; when the
 * transition holds at its end it hides the model, clears flag 1 and, for a
 * reversed run, restores full brightness-down. */
void ScreenFade_Update(void) {
    Ov007Fade *fade = sScreenFade;
    Ov007Transition *tr = fade->transition;
    int flags = fade->flags;
    int started = 0;
    if (fade->request != 0) {
        ScreenFade_Start();
        sScreenFade->request = 0;
        started = 1;
    }
    Transition_Update(tr);
    if ((flags & 1) == 0)
        return;
    int progress = tr->t;
    if (started == 0 && (flags & 2) != 0) {
        G2_SetBlendBrightness(0x3f, 0);
    }
    if (tr->reversed != 0)
        progress = 0x1000 - progress;
    sScreenFade->model->scale.z = progress;
    sScreenFade->model->scale.x = sScreenFade->model->scale.z;
    if (tr->state != 3)
        return;
    sScreenFade->model->visible = 0;
    {
        const Ov007Fade *cur = sScreenFade;
        sScreenFade->flags = cur->flags & ~1;
    }
    if (tr->reversed == 0)
        return;
    G2_SetBlendBrightness(0x3f, -0x10);
}
}
