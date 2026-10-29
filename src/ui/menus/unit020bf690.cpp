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

extern "C" {

/* func_ov007_020c8b10's screen transition; names as in
 * func_ov007_020c8498. From +0x28 on it holds the effect's arguments. */
typedef struct Ov007Transition {
    s32 effect;    /* 0x00: 1 mosaic, 2/3 blend alpha, 4 brightness */
    u32 screens;   /* 0x04: bit 0 main engine, bit 1 sub */
    s32 state;     /* 0x08: 0 idle, 1 running, 3 held at the end */
    s32 endAction; /* 0x0c: on reaching t = 0x1000: 0 stop, 1 hold, 2/3 restart */
    s32 curve;     /* 0x10: 0 linear, 1 out-and-back, 2 squared, 3 none */
    s32 t;         /* 0x14: Q12 progress, clamped at 0x1000 */
    s32 step;      /* 0x18: added to t each frame */
    s32 reversed;  /* 0x1c: 1 runs the curve backwards */
    s32 request;   /* 0x20 */
    void *onEnd;   /* 0x24: called when t reaches the end */
    /* 0x28: the BLDCNT plane mask the effect applies to */
    s32 planes;
    /* 0x2c: brightness: the start level; blend: the second-target planes */
    s32 from;
    /* 0x30: brightness: the end level; blend: EVA/EVB start and end */
    union {
        s32 to;
        struct {
            u8 evaFrom;
            u8 evbFrom;
            u8 evaTo;
            u8 evbTo;
        } blend;
    } arg30;
} Ov007Transition;

/* The camera func_ov007_020c023c allocates; names as in
 * func_ov007_020c0128, only the slots this file reads. */
typedef struct Ov007Camera {
    char pad_00[0x44];
    Matrix4x3 viewMatrix; /* 0x44 */
    char pad_74[0x60];
    /* 0xd4: a u16 angle; this file, like func_ov007_020b2160, indexes the
     * sine table with it >> 4 */
    u16 fovy;
} Ov007Camera;

typedef char Ov007Camera_fovy_must_be_at_0xd4[(int)&((Ov007Camera *)0)->fovy == 0xd4 ? 1 : -1];

/* A BMD_Material's two colour words. */
typedef struct Ov007MaterialColors {
    char pad_00[0x28];
    u32 diffuseAmbient;   /* 0x28 */
    u32 specularEmission; /* 0x2c */
} Ov007MaterialColors;

/* ModelComponents' first two words. */
typedef struct Ov007ModelParts {
    void *modelFile;                /* 0x00 */
    Ov007MaterialColors *materials; /* 0x04 */
} Ov007ModelParts;

/* The 0xa0-byte model record func_ov007_020bc6d4 allocates; names as in
 * func_ov007_020bc434, only the slots this file touches. */
typedef struct Ov007Model {
    Ov007ModelParts *model; /* 0x00 */
    char pad_04[0x30];
    Vector3 pos;   /* 0x34: func_ov007_020bc968 copies it into the matrix translation */
    Vector3 scale; /* 0x40: ModelComponents::Render's scale (func_ov007_020bbff0) */
    u16 rotX;      /* 0x4c: func_ov007_020c39f8's angles (func_ov007_020bc968) */
    char pad_4e[0x46];
    int visible; /* 0x94: func_ov007_020bbff0 renders only while it is nonzero */
} Ov007Model;

typedef char Ov007Model_visible_must_be_at_0x94[(int)&((Ov007Model *)0)->visible == 0x94 ? 1 : -1];

/* The 0x14-byte screen-fade record at data_ov007_02104bd8. */
typedef struct Ov007Fade {
    Ov007Transition *transition; /* 0x00 */
    Ov007Model *model;           /* 0x04 */
    int flags;                   /* 0x08: RequestScreenFade's second argument */
    void *onEnd;                 /* 0x0c: RequestScreenFade's third */
    /* 0x10: RequestScreenFade's first, the fade kind; func_ov007_020bf690
     * starts it and clears it */
    int request;
} Ov007Fade;

typedef char Ov007Fade_size_must_be_0x14[sizeof(Ov007Fade) == 0x14 ? 1 : -1];

/* Vector3's layout as a POD. A Vector3 local would make this object emit
 * Vector3's empty out-of-line destructor, which the ROM does not place in
 * this range. */
typedef struct Ov007Point {
    Fix12i x, y, z;
} Ov007Point;

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
extern void *func_ov007_020bc6d4(char *r6, void *r7);
extern void func_ov007_020bdeb0(int a);
extern void func_ov007_020c3d1c(void *self);
extern int func_ov007_020c3df4(int a, void *b);
extern void func_ov007_020c8498(char *r5);
extern void func_ov007_020c8688(void *p, int a);
extern void func_ov007_020c86a8(void *c, int v);
extern void func_ov007_020c8b04(void *p);
extern void *func_ov007_020c8b10(void);

#define FADE data_ov007_02104bd8

// @symbol func_ov007_020bfcec
void func_ov007_020bfcec(void) {
    int r;

    data_ov007_02104bd8 = (Ov007Fade *)func_ov007_020c3df4(0, (void *)sizeof(Ov007Fade));
    FADE->transition = (Ov007Transition *)func_ov007_020c8b10();
    InitialiseVramGlobals();
    r = (int)func_ov007_020b3f20(0x24);
    r = (int)func_ov007_020bc6d4((char *)r, 0);
    FADE->model = (Ov007Model *)r;
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
    Ov007Model *o = FADE->model;
    Ov007Point vec;
    Matrix4x3 mat;

    func_ov007_020bc47c(o);
    o->rotX = (short)cstd::div(0x5a0000, 0x168);
    o->visible = 0;

    vec.y = 0;
    vec.x = 0;
    vec.z = 0xffffef9c;
    InvMat4x3((int *)&cam->viewMatrix, (int *)&mat);
    MulVec3Mat4x3((Vector3 *)&vec, &mat, &o->pos);
}

// @symbol func_ov007_020bfaf0
/* While the model is visible, scales it in x and z to fill the camera's view
 * at its fovy, then updates it under the camera's view matrix and renders
 * it. c keeps the char * spelling of the definition; `cam` is the typed
 * view. */
void func_ov007_020bfaf0(char *c) {
    Ov007Camera *cam = (Ov007Camera *)c;
    Ov007Model *r4 = FADE->model;
    if (r4->visible == 0)
        return;
    unsigned int idx = cam->fovy >> 4;
    Fix12i t1 = FX_MUL(data_02082214[idx * 2], 0x1064);
    Fix12i base = r4->scale.x * 0xd800 + 0x12c000;
    Fix12i t2 = FX_MUL(base, t1);
    int denom = (short)data_02082214[idx * 2 + 1] * 0x60;
    Fix12i d = cstd::fdiv(t2, denom);
    r4->scale.z = cstd::fdiv(d, 0x5000);
    r4->scale.x = r4->scale.z;
    func_ov007_020bc02c(r4, &cam->viewMatrix);
    func_ov007_020bbff0(r4);
}

// @symbol RequestScreenFade
/* Queues fade kind r0 (1..9, see func_ov007_020bf790) with flags r1 and end
 * callback r2; func_ov007_020bf690 starts it on its next run. */
void RequestScreenFade(int r0, int r1, int r2) {
    FADE->request = r0;
    FADE->flags = r1;
    FADE->onEnd = (void *)r2;
}

// @symbol func_ov007_020bf790
/* Starts the queued fade: loads kind `request` (1..9) into the transition,
 * shows or hides the model by flag 1 (with sound 0x30 / 0x31 by flag 2),
 * and runs the transition forwards (flag 2) or backwards on the screens the
 * flags pick. */
void func_ov007_020bf790(void) {
    Ov007Fade *g = data_ov007_02104bd8;
    Ov007Transition *r6 = g->transition;
    int r5 = g->flags;
    int state = g->request;
    int r4 = 2;

    switch (state) {
    default:
        break;
    case 1:
        r6->effect = 4;
        r6->endAction = 1;
        r6->curve = 0;
        r6->planes = 0x3F;
        r6->step = 0x3C;
        r6->from = 0x10;
        r6->arg30.to = 0;
        break;
    case 2:
        r6->effect = 4;
        r6->endAction = 1;
        r6->curve = 0;
        r6->planes = 0x3F;
        r6->step = 0x3C;
        r6->from = -0x10;
        r6->arg30.to = 0;
        break;
    case 3:
        r6->effect = 4;
        r6->endAction = 0;
        r6->curve = 0;
        r6->planes = 0x2F;
        r6->step = 0x3C;
        r6->from = -0x10;
        r6->arg30.to = 0;
        break;
    case 4:
        r6->effect = 4;
        r6->endAction = 0;
        r6->curve = 0;
        r6->planes = 0x3F;
        r6->step = 0x100;
        r6->from = -0x10;
        r6->arg30.to = 0;
        break;
    case 5:
        r6->effect = 4;
        r6->endAction = 1;
        r6->curve = 0;
        r6->planes = 0x3F;
        r6->step = 0x3C;
        r6->from = 0;
        r6->arg30.to = 0;
        break;
    case 6:
        r6->effect = 4;
        r6->endAction = 1;
        r6->curve = 0;
        r6->planes = 0x3F;
        r6->step = 0x1000;
        r6->from = -0x10;
        r6->arg30.to = -0x10;
        break;
    case 7:
        r6->effect = 4;
        r6->endAction = 1;
        r6->curve = 0;
        r6->planes = 0x2F;
        r6->step = 0x1000;
        r6->from = -0x10;
        r6->arg30.to = -0x10;
        break;
    case 8:
        r6->effect = 4;
        r6->endAction = 1;
        r6->curve = 0;
        r6->planes = 0x3D;
        r6->step = 0x100;
        r6->from = 0;
        r6->arg30.to = -9;
        break;
    case 9:
        r6->effect = 3;
        r6->endAction = 1;
        r6->curve = 0;
        r6->step = 0x190;
        r6->planes = 1;
        r6->from = 0x14;
        r6->arg30.blend.evaFrom = 0x10;
        r6->arg30.blend.evaTo = 0;
        r6->arg30.blend.evbFrom = 0;
        r6->arg30.blend.evbTo = 0;
        break;
    }

    if (r5 & 1) {
        int v;
        Ov007ModelParts *a = data_ov007_02104bd8->model->model;
        if (state == 1) {
            v = 0x7FFF;
        } else {
            v = 0;
        }
        v = v | (v << 0x10);
        a->materials->diffuseAmbient = v;
        a->materials->specularEmission = v;
        data_ov007_02104bd8->model->visible = 1;
        if (r5 & 2) {
            func_ov007_020bdeb0(0x30);
        } else {
            func_ov007_020bdeb0(0x31);
        }
    } else {
        if (r5 & 4) {
            r4 = 1;
        } else if (!(r5 & 8)) {
            r4 |= 1;
        }
        data_ov007_02104bd8->model->visible = 0;
    }

    if (r5 & 0x10) {
        G2x::SetBlendBrightness((volatile unsigned short *)0x4000050, 0x3F, 0);
    }

    if (r5 & 2) {
        func_ov007_020c86a8(r6, r4);
    } else {
        func_ov007_020c8688(r6, r4);
    }

    data_ov007_02104bd8->transition->onEnd = data_ov007_02104bd8->onEnd;
}

// @symbol func_ov007_020bf690
/* One frame: starts a queued fade, runs the transition, and while flag 1 is
 * set scales the model in x and z by the transition's progress; when the
 * transition holds at its end it hides the model, clears flag 1 and, for a
 * reversed run, restores full brightness-down. */
void func_ov007_020bf690(void) {
    Ov007Fade *g = data_ov007_02104bd8;
    Ov007Transition *obj6 = g->transition;
    int flags = g->flags;
    int did = 0;
    if (g->request != 0) {
        func_ov007_020bf790();
        data_ov007_02104bd8->request = did;
        did = 1;
    }
    func_ov007_020c8498((char *)obj6);
    if ((flags & 1) == 0)
        return;
    int v = obj6->t;
    if (did == 0 && (flags & 2) != 0) {
        G2x::SetBlendBrightness((volatile unsigned short *)0x4000050, 0x3f, 0);
    }
    if (obj6->reversed != 0)
        v = 0x1000 - v;
    data_ov007_02104bd8->model->scale.z = v;
    data_ov007_02104bd8->model->scale.x = data_ov007_02104bd8->model->scale.z;
    if (obj6->state != 3)
        return;
    data_ov007_02104bd8->model->visible = 0;
    {
        const Ov007Fade *gr = data_ov007_02104bd8;
        data_ov007_02104bd8->flags = gr->flags & ~1;
    }
    if (obj6->reversed == 0)
        return;
    G2x::SetBlendBrightness((volatile unsigned short *)0x4000050, 0x3f, -0x10);
}
}
