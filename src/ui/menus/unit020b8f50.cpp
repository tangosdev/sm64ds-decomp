//cpp
/* unit020b8f50 -- ov007 unit 0x020b8f50..0x020bbe60: 24 functions, .text only.
 *
 * The title menu's drawing-board scene. ov007 is the title and file-select
 * menu program that dScDSMT_c hosts; outside dScDSMT_c it has no ROM RTTI
 * or vtable, so no class name is claimed and the file keeps the unit<addr>
 * name.
 *
 * What the code does. PickerScene_Create builds the scene: the 0x4c-byte
 * scene record data_ov007_02104ba0, the 0x14-byte sprite record
 * data_ov007_02104b9c, the 0xd0-byte touch picker (func_ov007_020c690c),
 * four menu states and two tweens; PickerScene_Destroy frees all of it.
 * PickerScene_Update runs the states[0] machine: the intro
 * (PickerScene_UpdateIntro), the drawing phase (PickerScene_StartDrawing restarts it,
 * PickerScene_UpdateDrawing turns stylus input into strokes and
 * PickerScene_HandleDrawingKeys handles keys: zoom, undo and redo) and the edit phase
 * (PickerScene_UpdateEdit picks and drags points, PickerScene_HandleEditKeys turns
 * held buttons into squash, wobble and spin). PickerScene_UpdatePicker runs the
 * states[2] and states[3] animations (PickerScene_UpdateScaleAnim,
 * PickerScene_UpdateMoveAnim) and binds the picker's spring constants.
 * func_ov007_020b91b4 runs the states[1] display machine and draws the
 * picker; PickerScene_UpdateReplay replays a drawing; func_ov007_020b8fd4 places
 * and draws the scene's sprites and the point-budget bar. The five getters
 * at the bottom hand the menu states, the awaiting-tap flag and the idle
 * timeout to the rest of the menu.
 *
 * Edges: every function here touches this scene's bss block
 * 0x02104b98..0x02104ba4 or calls one that does, and nothing below the range
 * touches it. The next function, func_ov007_020bbe60, also uses the block
 * (it loads a portrait drawing into the picker), but it stays in its own
 * file, the intact object ov007/AnimationPathSets that owns the portrait
 * stroke tables. The .rodata layout does not let one TU hold both: the
 * tables these functions read (0x020d7b54..0x020d7c28) lie between the
 * ones unit020b797c and func_ov007_020bd1c0 read, in text order, while
 * the stroke counts at 0x020cdb34 lie inside the portrait coordinate
 * block that follows the .rodata unit020b2764's functions read. The
 * previous function, MenuText_MessageIDToIndex, touches no data and is called
 * only by the module whose bss block ends at 0x02104b98.
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 *
 * The PickerScene_ names are coined from the
 * bodies, after the in-tree Ov007PickerScene.
 */
#include "types.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "Ov007Work.h"
#include "Ov007Sprite.h"
#include "Ov007MenuState.h"
#include "Ov007Tween.h"
#include "Ov007StrokePath.h"
#include "Ov007Touch.h"
#include "Ov007Picker.h"
#include "Ov007Camera.h"
#include "Ov007Item.h"
#include "Ov007Pad.h"
#include "private/ov007_picker_scene.h"
#include "nitro/fx/fx_cp.h"
#include "math/ProgressRatio.h"

/* data_ov007_02104b9c: the 0x14-byte sprite record. */
typedef struct Ov007PickerSprites {
    /* 0x00, medium confidence: Q12; while it is <= 0x1000,
     * func_ov007_020b8fd4 slides sprites[1] in along the intro's axis;
     * PickerScene_UpdateIntro eases it up */
    s32 slideProgress;
    /* 0x04: func_ov007_020c44e4; [0] and [1] the board, [2] and [3] the
     * point-budget bar */
    Ov007Sprite *sprites[4];
} Ov007PickerSprites;

/* PickerScene_UpdateMoveAnim copies pos as three words. */
typedef struct Vec3Words {
    Fix12i w[3];
} Vec3Words;

extern char *data_ov007_02104ba0;
extern Ov007PickerSprites *data_ov007_02104b9c;
/* PickerScene_UpdateIntro's settle counter, -1 until the drawing is complete */
extern s16 data_ov007_02104b98[];

#define SCENE ((Ov007PickerScene *)data_ov007_02104ba0)
#define SPRITES data_ov007_02104b9c

extern u16 data_ov007_020d7b54[];
extern int data_ov007_020d7b60;
extern char data_ov007_020d7b80[];

extern "C" void MTX_Rot22_(int *p, int a, int b);
extern "C" void func_ov007_020bbe60(int idx);
extern void func_ov007_020bda38(int v);
extern void func_ov007_020bda8c(int a, int b);
extern void func_ov007_020bdbcc(int a0, int a1);
extern void func_ov007_020bde2c(int idx);
int func_ov007_020bdeb0(int a);
void func_ov007_020bfe4c(Ov007Camera *cam, int x, int y, int z, Vector3 *out);
extern int func_ov007_020c1da0(int i);
int func_ov007_020c3ba8(int);
unsigned short func_ov007_020c3be0(int t, int a, int b);
extern "C" void func_ov007_020c3d1c(void *);
extern "C" void *func_ov007_020c3df4(const char *name, int size);
extern "C" void func_ov007_020c4f50(void *c, int a1, int a2, int a3, int a4);
extern "C" void func_ov007_020c4e98(Ov007Picker *c, int a1, int a2, int a3, int a4,
                                    unsigned short a5, int a6);
extern "C" void func_ov007_020c5188(void *, int);
extern "C" void func_ov007_020c5490(void *obj, int *m);
extern "C" void func_ov007_020c55bc(Ov007Picker *self, int m1, int m2);
extern "C" void func_ov007_020c56bc(void *c, int a, int b, int d, int e, int f);
extern void func_ov007_020c5a6c(Ov007Picker *, void *, int);
extern "C" void func_ov007_020c5c14(void *o, void *p);
extern "C" int func_ov007_020c5d5c(void *o);
extern "C" int func_ov007_020c5db0(void *p);
extern void func_ov007_020c5dec(Ov007Picker *c);
extern "C" void func_ov007_020c5ef0(void *o);
extern "C" int func_ov007_020c8f98(void *p);
extern "C" void func_ov007_020c9080(void *);
extern "C" Ov007Tween *func_ov007_020c908c(s32 *out);
void func_ov007_020c92d0(Ov007MenuState *state);
extern "C" void func_ov007_020c937c(void *state);
void *func_ov007_020c9388(int a);

/* Replays the drawing one frame: strokeTime advances so that it covers
 * totalStrokeTime in replayFrames frames (param 0) or 0x168 frames, by at
 * least 0x1000 (one frame) a frame, and func_ov007_020c5014 moves the
 * picker's draw limit to it (strokeTime restarts when the limit steps on to
 * the next stroke). For param 0, sprites[0] also fades in up to alpha
 * 0x1f000. Returns 1 once the replay has passed the last stroke (for
 * param 0, only once sprites[0] is fully opaque). */
int PickerScene_UpdateReplay(int param) {
    int r4;
    int r5;

    if (param == 0)
        r4 = SCENE->replayFrames;
    else
        r4 = 0x168;

    r4 = FX_DivS32(SCENE->totalStrokeTime << 0xc, r4);
    if (r4 < 0x1000)
        r4 = 0x1000;

    r5 = SCENE->picker->func_ov007_020c5014(SCENE->strokeTime >> 0xc);
    if (r5 == 1) {
        SCENE->strokeTime = 0;
    } else if (r5 == 0) {
        int *fp = (int *)(((int)&SCENE->strokeTime));
        *fp += r4;
    }

    func_ov007_020bdbcc(4, 4);

    switch (param) {
    case 0: {
        int *q = (int *)(((int)&SPRITES->sprites[0]->alpha));
        int v = *q;
        if (v <= 0x5000) {
            *q = v + 0x7d0;
            break;
        }
        if (v <= 0x14000) {
            *q = v + 0x190;
            break;
        }
        if (v <= 0x1f000) {
            *q = v + 0xbb8;
            break;
        }
        *q = 0x1f000;
        return r5 == 2;
    }
    case 1:
        return r5 == 2;
    }
    return 0;
}

/* Modes 0 and 1 put every link back on its rest shape at unit scale (1
 * re-anchors pos first); modes 2 and 3 jitter `percent` percent of the
 * links by up to `range` (func_ov007_020c56bc). */
void PickerScene_SetDeform(int mode, int percent, int range) {
    Ov007Picker *p;

    switch (mode) {
    case 0:
    case 1:
        if (mode == 1) {
            func_ov007_020c5c14(SCENE->picker, &SCENE->picker->pos);
        }
        p = SCENE->picker;
        func_ov007_020c64c4(p, 0);
        func_ov007_020c62cc(p, 1);
        func_ov007_020c55bc(SCENE->picker, 0x1000, 0x1000);
        SCENE->picker->posOffset.y = 0;
        SCENE->picker->posOffset.x = SCENE->picker->posOffset.y;
        break;

    case 2:
        func_ov007_020c56bc(SCENE->picker, 1, percent, range / 2, range / 2, 8);
        break;

    case 3:
        func_ov007_020c56bc(SCENE->picker, 2, percent, range, range, 6);
        break;
    }
}

static inline int pressed(Ov007Pad *pad, u16 h, int m1, int m2) {
    int ok, n3, n2, n5;
    n3 = 0;
    ok = 1;
    n2 = 0;
    n5 = ((h & m1) == 0) ? ok : n3;
    if (n5 != 0) {
        if (pad->prevKeys & m1) {
            n2 = 1;
        }
    }
    if (n2 != 0) {
        if ((h & m2) == 0) {
            n3 = 1;
        }
    }
    if (n3 == 0) {
        if ((h & m2) || !(pad->prevKeys & m2) || n5 == 0) {
            ok = 0;
        }
    }
    return ok;
}

void PickerScene_HandleEditKeys(void) {
    int t1;
    Ov007Picker *s;
    Ov007Pad *pad;
    int mat[4];
    u16 h;

    s = SCENE->picker;
    pad = WORK->pad;

    {
        u32 w = pad->heldFrames[9];
        u32 v = pad->heldFrames[8];
        if (v <= w) {
            v = w;
        }
        if (FX_ModS32(v, 5) == 1) {
            PickerScene_SetDeform(2, 0xa, 0x1000);
            SCENE->springSet = 0;
        } else {
            h = pad->keys;
            if (pressed(pad, h, 0x200, 0x100)) {
                PickerScene_SetDeform(1, 0, 0);
                SCENE->springSet = 3;
                func_ov007_020bdeb0(0x24);
            }
        }
    }

    {
        u32 lim = pad->heldFrames[6];
        int t2 = pad->heldFrames[1];
        int dir;
        t1 = pad->heldFrames[0];
        if ((u32)t1 <= lim) {
            t1 = lim;
        }
        lim = pad->heldFrames[5];
        if ((u32)t2 <= lim) {
            t2 = lim;
        }
        dir = (t1 > t2) ? 1 : -1;
        if (t1 <= t2) {
            t1 = t2;
        }
        if (t1 > 0) {
            int extra;
            int a;
            int b;
            a = 0x1000;
            extra = 0;
            b = a;
            int ratio, ease, v2, half, fpv;
            SCENE->springSet = 0;
            ratio = ProgressRatio(t1, 0x64);
            ease = (int)(((s64)ratio * (0x2000 - ratio)) >> 0xc);
            v2 = RangeRatio(t1, 0x50, 0xc8);
            half = ((int)(((s64)v2 * (0x2000 - v2)) >> 0xc)) / 2;
            fpv = half + 0x1000;
            if (t1 >= 0xa0) {
                int m = FX_ModS32(t1 - 0xa0, 5);
                int mc, v3, v3sq;
                s16 tbl;
                mc = ProgressRatio(m, 5);
                v3 = RangeRatio(t1, 0xa0, 0xc8);
                v3sq = (int)(((s64)v3 * v3) >> 0xc);
                tbl = FX_SinIdx((u16)((mc * 0xffff) >> 12) >> 4);
                extra = FX_MUL(v3sq, (tbl * (s16)0xa));
            }
            if (dir > 0) {
                a -= ease;
                b += extra + FX_MUL(ease, fpv);
            } else {
                a += extra + FX_MUL(ease, fpv);
                b -= ease;
            }
            func_ov007_020c55bc(s, a, b);
            func_ov007_020bde2c(0x38);
        } else {
            h = pad->keys;
            if (pressed(pad, h, 0x40, 0x400) || pressed(pad, h, 0x80, 2)) {
                PickerScene_SetDeform(0, 0, 0);
                SCENE->springSet = 3;
                func_ov007_020bdeb0(0x24);
            }
        }
    }

    {
        int c2;
        int c1;
        int amp;
        int dir2;
        u32 lim2;
        lim2 = pad->heldFrames[4];
        c1 = pad->heldFrames[2];
        c2 = pad->heldFrames[3];
        if ((u32)c1 <= lim2) {
            c1 = lim2;
        }
        lim2 = pad->heldFrames[7];
        if ((u32)c2 <= lim2) {
            c2 = lim2;
        }
        dir2 = (c1 > c2) ? 1 : -1;
        c1 = (c1 > c2) ? c1 : c2;
        if (c1 > 0) {
            int md = FX_ModS32(c1, 5);
            if (c1 < 0x64) {
                int r;
                r = ProgressRatio(c1, 0x64);
                amp = r / 2;
            } else {
                int v, sq;
                v = RangeRatio(c1, 0x64, 0x12c);
                sq = (int)(((s64)v * v) >> 0xc);
                amp = sq / 2 + 0x800;
                if (c1 >= 0x12c) {
                    int m2 = FX_ModS32(c1, 0x64);
                    int mc2;
                    int t2v;
                    mc2 = ProgressRatio(m2, 0x64);
                    t2v = FX_SinIdx((u16)((mc2 * 0xffff) >> 12) >> 4);
                    if (t2v < 0) {
                        t2v = -t2v;
                    }
                    amp = FX_MUL((t2v / 4 + 0xc00), amp);
                }
            }
            if (md == 0) {
                int idx = (u16)(((dir2 * amp) * 0x3555) >> 12) >> 4;
                MTX_Rot22_(mat, FX_SinIdx(idx), FX_CosIdx(idx));
                func_ov007_020c5490(s, mat);
            } else {
                int n = FX_DivS32(s->groupCount, 4) + 1;
                if (n == 1) {
                    n = s->groupCount;
                }
                dir2 = (md - 1) * n;
                md = dir2 + n;
                if (md > s->groupCount) {
                    md = s->groupCount;
                }
                for (; dir2 < md; dir2++) {
                    func_ov007_020c6314(SCENE->picker, dir2, 0);
                    func_ov007_020c6174(SCENE->picker, dir2, 1);
                }
            }
            func_ov007_020bda38(amp);
            SCENE->springSet = 2;
        } else {
            h = pad->keys;
            if (pressed(pad, h, 0x10, 1) || pressed(pad, h, 0x20, 0x800)) {
                PickerScene_SetDeform(0, 0, 0);
                SCENE->springSet = 0;
            }
        }
    }
}

/* The scene's key handling (fresh presses, keys & ~prevKeys): X/up and
 * B/down step tweens[0] between its three stops, R/L request menu state 3 or
 * 4 by sprites[1]'s alpha, and Y/left and A/right undo or redo the last
 * committed stroke. */
int PickerScene_HandleDrawingKeys(void) {
    Ov007Picker *r5 = SCENE->picker;
    Ov007Pad *r4 = WORK->pad;
    unsigned short r2 = r4->keys;
    int r3 = 0;

    if ((r2 & 0x400) && !(r4->prevKeys & 0x400)) {
        r3 = 1;
    }
    if (r3 != 0 || ((r2 & 0x40) && !(r4->prevKeys & 0x40))) {
        int r1;
        int ip;
        Ov007Tween *r3p = SCENE->tweens[0];
        r1 = r3p->to;
        switch (r1) {
        case 0:
            ip = 0x800;
            break;
        case 0x800:
            ip = 0x1000;
            break;
        case 0x1000:
            ip = -1;
            break;
        default:
            ip = 0;
            break;
        }
        if (ip != -1) {
            int t = SCENE->tweenOut[0];
            r3p->state = 1;
            r3p->mode = 1;
            r3p->t = 0;
            r3p->step = 0x100;
            r3p->from = t;
            *r3p->out = r3p->from;
            r3p->to = ip;
            func_ov007_020bdeb0(0x23);
        }
    } else if (((r2 & 2) && !(r4->prevKeys & 2)) || ((r2 & 0x80) && !(r4->prevKeys & 0x80))) {
        int r1;
        int ip;
        Ov007Tween *r3p = SCENE->tweens[0];
        r1 = r3p->to;
        switch (r1) {
        case 0:
            ip = -1;
            break;
        case 0x800:
            ip = 0;
            break;
        case 0x1000:
            ip = 0x800;
            break;
        default:
            ip = 0;
            break;
        }
        if (ip != -1) {
            int t = SCENE->tweenOut[0];
            r3p->state = 1;
            r3p->mode = 1;
            r3p->t = 0;
            r3p->step = 0x100;
            r3p->from = t;
            *r3p->out = r3p->from;
            r3p->to = ip;
            func_ov007_020bdeb0(0x22);
        }
    }

    {
        int r3b = 0;
        Ov007Pad *r2p = WORK->pad;
        unsigned short r1 = r2p->keys;
        if ((r1 & 0x100) && !(r2p->prevKeys & 0x100)) {
            r3b = 1;
        }
        if (r3b != 0 || ((r1 & 0x200) && !(r2p->prevKeys & 0x200))) {
            if (SPRITES->sprites[1]->alpha == 0x1f000) {
                SCENE->states[1]->requestedState = 3;
                func_ov007_020bdeb0(0x29);
            } else {
                SCENE->states[1]->requestedState = 4;
                func_ov007_020bdeb0(0x2a);
            }
        }
    }

    {
        Ov007Touch *r1 = WORK->touch;
        if (r1->cur.touch != 0) {
            return r1->cur.touch;
        }
        if (r1->releasedFrames < 1U) {
            return r1->releasedFrames;
        }
    }

    {
        int rv = func_ov007_020c1da0(0);
        if (rv != 0) {
            return rv;
        }
    }

    {
        Ov007PickerScene *b = *(Ov007PickerScene *volatile *)&data_ov007_02104ba0;
        short st = b->states[1]->currentState;
        if (st == 6)
            return st;
        if (st == 7)
            return st;
        if (b->awaitingTap != 0) {
            return b->awaitingTap;
        }
    }

    {
        unsigned short r1 = r4->keys;
        int r2b = 0;
        if ((r1 & 0x800) && !(r4->prevKeys & 0x800)) {
            r2b = 1;
        }
        if (r2b != 0 || ((r1 & 0x20) && !(r4->prevKeys & 0x20))) {
            int n = r5->groupCount;
            if (n <= 0)
                return n;
            {
                Ov007StrokePath *e = r5->groups[n - 1];
                int *p14 = &r5->totalPoints;
                int *p8 = &r5->groupCount;
                *p14 = *p14 - e->capacity;
                *p8 = *p8 - 1;
                SCENE->states[1]->requestedState = 6;
                return func_ov007_020bdeb0(0x20);
            }
        }
        if ((r1 & 1) && !(r4->prevKeys & 1)) {
            /* fallthrough to block_66 */
        } else {
            if (!(r1 & 0x10))
                return r1 & 0x10;
            if (r4->prevKeys & 0x10)
                return r4->prevKeys & 0x10;
        }
    }

    {
        Ov007PickerScene *b = *(Ov007PickerScene *volatile *)&data_ov007_02104ba0;
        int n = r5->groupCount;
        int lim = b->strokeCount;
        if (n >= lim)
            return lim;
        {
            Ov007StrokePath *e = r5->groups[n];
            int *p14 = &r5->totalPoints;
            int *p8 = &r5->groupCount;
            *p14 = *p14 + e->capacity;
            *p8 = *p8 + 1;
            (*(Ov007PickerScene *volatile *)&data_ov007_02104ba0)->states[1]->requestedState = 7;
            return func_ov007_020bdeb0(0x21);
        }
    }
}

void PickerScene_UpdateScaleAnim(void) {
    Ov007PickerScene *mgr;
    Ov007MenuState *obj;
    Ov007Picker *r6;
    int r5;
    u16 period;
    int phase;
    int t;
    s16 sw;

    mgr = SCENE;
    r6 = mgr->picker;
    func_ov007_020c92d0(mgr->states[2]);

    obj = SCENE->states[2];
    r5 = obj->stateTicks;
    period = data_ov007_020d7b54[obj->currentState];
    t = FX_ModS32(r5, period);
    phase = ProgressRatio(t, period);

    mgr = SCENE;
    obj = mgr->states[2];
    sw = obj->currentState;

    switch (sw) {
    case 0:
        if (r5 != 0) {
            return;
        }
        PickerScene_SetDeform(1, 0, 0);
        return;
    case 1:
        mgr->springSet = 0;
        if (FX_ModS32(r5, 6) != 0) {
            return;
        }
        {
            int v;
            if (phase >= 0x800) {
                v = 0x1000 - ((phase - 0x800) * 2);
            } else {
                v = phase * 2;
            }
            PickerScene_SetDeform(3, 0xa, (int)(((long long)v * (0x2000 - v)) >> 0xc));
        }
        return;
    case 2: {
        int v;
        int amt;
        if (phase >= 0x800) {
            v = 0x1000 - ((phase - 0x800) * 2);
        } else {
            v = phase * 2;
        }
        amt = (int)(((long long)v * (0x2000 - v)) >> 0xc) + 0x1000;
        func_ov007_020c55bc(r6, amt, amt);
        obj = SCENE->states[2];
        if (r5 >= data_ov007_020d7b54[obj->currentState]) {
            obj->requestedState = 0;
        }
        if (phase <= 0x800) {
            SCENE->springSet = 4;
        } else {
            SCENE->springSet = 0;
        }
        return;
    }
    case 3: {
        int d;
        u16 p3;
        int m;
        int wobble;

        d = r5 - 0x14;
        if (d < 0) {
            return;
        }
        if (d > data_ov007_020d7b54[sw] * 3) {
            return;
        }
        p3 = data_ov007_020d7b54[3];
        m = FX_ModS32(d, p3);
        wobble = ProgressRatio(m, p3);
        {
            int idx = (u16)((wobble * 0xffff) >> 12) >> 4;
            short tblval;
            int amt;
            tblval = FX_SinIdx(idx);
            amt = FX_MUL(tblval, 0x258);
            func_ov007_020c55bc(r6, amt + 0x1000, 0x1000 - amt);
        }
        SCENE->springSet = 2;
        return;
    }
    case 4: {
        int m = FX_ModS32(r5, 0x3c);
        SCENE->springSet = 2;
        if (m == 0) {
            PickerScene_SetDeform(1, 0, 0);
            return;
        }
        if (m != 0x1e) {
            return;
        }
        PickerScene_SetDeform(3, 0x1e, FX_MUL(phase, 0x7d0) + 0xc8);
        return;
    }
    case 5:
    default:
        return;
    }
}

/* Each menu state drives the picker's per-step offset across the clamp
 * rectangle; state 5 puts pos back at its rest position. */
void PickerScene_UpdateMoveAnim(void) {
    Ov007Picker *a;
    s16 state;
    int dx;
    int dy;
    int s;
    Ov007MenuState *menu;
    int counter;

    a = SCENE->picker;
    menu = SCENE->states[3];
    dx = a->boundsMax.x - a->boundsMin.x;
    dy = a->boundsMax.y - a->boundsMin.y;
    func_ov007_020c92d0(menu);
    menu = SCENE->states[3];
    state = menu->currentState;
    counter = menu->stateTicks;

    switch (state) {
    case 0:
        if (counter == 0) {
            a->posOffset.y = 0;
            a->posOffset.x = a->posOffset.y;
        }
        return;
    case 1:
    case 2: {
        int v;
        int m = FX_ModS32(counter, 0x3c);
        v = ProgressRatio(m, 0x3c);
        s = FX_SinIdx((u16)((v * 0xffff) >> 12) >> 4);
        if (state == 2) {
            a->posOffset.x = -dx / 48 + func_ov007_020c3ba8(dx / 48 - -dx / 48);
            a->posOffset.y = FX_MUL(s, (dy / 2 - dy / 8));
        } else if (state == 1) {
            a->posOffset.x = FX_MUL(s, (dx / 3));
            a->posOffset.y = -dy / 32 + func_ov007_020c3ba8(dy / 32 - -dy / 32);
        }
        return;
    }
    case 3:
    case 4: {
        int v;
        int idx;
        int m = FX_ModS32(counter, 0x3c);
        v = ProgressRatio(m, 0x3c);
        if (SCENE->states[3]->currentState == 4)
            v = 0x1000 - v;
        idx = (u16)((v * 0xffff) >> 12) >> 4;
        a->posOffset.x = FX_MUL((dx / 4), FX_CosIdx(idx));
        a->posOffset.y = FX_MUL((dy / 4), FX_SinIdx(idx));
        return;
    }
    case 5:
        if (counter == 0) {
            *(Vec3Words *)&a->pos = *(Vec3Words *)&a->restPos;
            a->posOffset.y = 0;
            a->posOffset.x = a->posOffset.y;
        }
        return;
    case 6:
        if (counter == 0) {
            int t;
            t = -dx / 2 + func_ov007_020c3ba8(dx / 2 - -dx / 2);
            a->pos.x = FX_MUL(t, 0x5dcLL);
            t = -dy / 2 + func_ov007_020c3ba8(dy / 2 - -dy / 2);
            a->pos.y = FX_MUL(t, 0x5dcLL);
            a->posOffset.y = 0;
            a->posOffset.x = a->posOffset.y;
        }
        return;
    }
}

void PickerScene_UpdatePicker(void) {
    PickerScene_UpdateScaleAnim();
    PickerScene_UpdateMoveAnim();
    SCENE->picker->springConstants = (int *)(data_ov007_020d7b80 + SCENE->springSet * 0x18);
    func_ov007_020c5188(SCENE->picker, 0);
}

int PickerScene_UpdateIntro(int t) {
    Ov007Picker *s;
    int done = 0;
    int state;
    int local[3];

    {
        Ov007PickerScene *m = SCENE;
        s = m->picker;
        m->springSet = 0;
    }

    if (t == 0) {
        int *p = &SCENE->introVariant;
        *p += 1;
        if (SCENE->introVariant >= 5) {
            SCENE->introVariant = 0;
        }
    }

    if (s->groupCount == 0 || SCENE->totalPoints == 0) {
        int ratio;
        ratio = ProgressRatio(t, 0x1e);
        SPRITES->sprites[1]->alpha = ratio * 0x1f;
        return ratio >= 0x1000;
    }

    state = SCENE->introVariant;
    switch (state) {
    case 0:
    case 3: {
        int *p1 = (state == 0) ? &s->boundsMin.x : &s->boundsMin.y;
        int *p2 = (state == 0) ? &s->boundsMax.x : &s->boundsMax.y;
        int d, ratio, ease, v1, v2;
        v1 = *p1;
        v2 = *p2;
        d = v2 - v1;
        /* ProgressRatio(t, 0x28), spelled out: the inline re-colours this case. */
        if (t <= 0) {
            ratio = 0;
        } else if (t >= 0x28) {
            ratio = 0x1000;
        } else {
            ratio = FX_DivS32(t << 0xc, 0x28);
        }
        ease = (int)(((s64)ratio * (0x2000 - ratio)) >> 0xc);
        SCENE->springSet = 1;
        if (t == 0) {
            SPRITES->sprites[1]->alpha = 0x1f000;
            func_ov007_020bdeb0(0x1b);
        }
        *p2 = d / 2 - FX_MUL(ease, d);
        *p1 = *p2 - d * 2;
        PickerScene_UpdatePicker();
        *p1 = v1;
        *p2 = v2;
        SPRITES->slideProgress = ease;
        done = ratio >= 0x1000;
        break;
    }
    case 1:
    case 4: {
        int r8arg, v, ratio;
        ratio = ProgressRatio(t, 0x78);
        if (t == 0) {
            SPRITES->slideProgress = 0x1000;
            data_ov007_02104b98[0] = -1;
        }
        if (t >= 0x96) {
            SCENE->springSet = 6;
        } else {
            SCENE->springSet = 0;
        }
        {
            Ov007Camera *o = WORK->cameras[0];
            func_ov007_020bfe4c(o, 0xe8000, 0xa8000, -o->distance, (Vector3 *)local);
        }
        PickerScene_UpdatePicker();
        v = RangeRatio(t, 0xc8, 0x258);
        ratio += FX_MUL(v, 0x1e000);
        r8arg = 0x1000;
        if (SCENE->introVariant == 4) {
            if (t <= 0xc8) {
                if (t <= 0x64) {
                    ratio = FX_MUL(ratio, 0x258);
                    r8arg = -0xa;
                } else {
                    ratio = FX_MUL(ratio, 0x320);
                    r8arg = -0x32;
                }
            } else if (t <= 0x12c) {
                ratio = FX_MUL(ratio, 0xa);
            }
        }
        {
            int ret = func_ov007_020c5854(s, (Vector3 *)local, (ratio * 0xc8) >> 0xc, 0x2000,
                                          ratio * 0x12c, r8arg);
            int dv = FX_DivS32(ret << 0xc, SCENE->totalPoints);
            SPRITES->sprites[1]->alpha = dv * 0x1f;
            if (dv >= 0x1000) {
                s16 c = data_ov007_02104b98[0];
                if (c < 0) {
                    data_ov007_02104b98[0] = 0;
                    func_ov007_020bdeb0(0x1e);
                } else {
                    data_ov007_02104b98[0] = c + 1;
                    if (data_ov007_02104b98[0] >= 0x1e) {
                        done = 1;
                    }
                }
                func_ov007_020bda8c(data_ov007_02104b98[0], 1);
            } else {
                func_ov007_020bda8c(t, 0);
            }
        }
        break;
    }
    case 2: {
        int a = 0x1000;
        int b = 0x1000;
        if (t == 0) {
            SPRITES->slideProgress = 0x1000;
        }
        if (t <= 0x50) {
            int ratio, m, mc, r2c, ease1, ease2, term, sinval;
            int idx;
            short tblval;
            ratio = ProgressRatio(t, 0x28);
            ease1 = (int)(((s64)ratio * (0x2000 - ratio)) >> 0xc);
            m = FX_ModS32(t, 6);
            /* ProgressRatio(m, 6), spelled out for the same reason. */
            if (m <= 0) {
                mc = 0;
            } else if (m >= 6) {
                mc = 0x1000;
            } else {
                mc = FX_DivS32(m << 0xc, 6);
            }
            r2c = RangeRatio(t, 0x14, 0x3c);
            ease2 = (int)(((s64)r2c * (0x2000 - r2c)) >> 0xc);
            idx = (u16)((mc * 0xffff) >> 12) >> 4;
            tblval = FX_SinIdx(idx);
            term = FX_MUL(ease2, 0x3e8);
            sinval = FX_MUL(tblval, term);
            if (t <= 0x4d) {
                sinval += FX_MUL(ease1, 0x4b0);
                a += sinval;
                b += sinval;
            } else {
                a = 0x2000;
                b = 0x2000;
            }
            SCENE->springSet = 2;
        } else {
            int v;
            v = RangeRatio(t, 0x50, 0x64);
            a -= v;
            b -= v;
            if (t == 0x51) {
                func_ov007_020bdeb0(0x1f);
            }
            SCENE->springSet = 5;
            SPRITES->sprites[1]->alpha = v * 0x1f;
            if (v >= 0x1000) {
                done = 1;
            }
        }
        func_ov007_020c55bc(s, a, b);
        PickerScene_UpdatePicker();
        break;
    }
    }
    return done;
}

void PickerScene_StartDrawing(void) {
    func_ov007_020c5dec(SCENE->picker);
    SCENE->awaitingTap = 1;
    SCENE->tweenOut[1] = 0x1000;
    SCENE->tweens[0]->to = 0x800;
    SCENE->tweenOut[0] = SCENE->tweens[0]->to;
    SCENE->restartCount += 1;
    SCENE->states[1]->requestedState = 5;
}

int PickerScene_UpdateDrawing(void) {
    Ov007PickerScene *mgr = SCENE;
    int r6 = 0;
    int r5;
    Ov007Touch *r4 = WORK->touch;

    if (mgr->awaitingTap != 0 && r4->cur.touch != 0 && r4->prev.touch == 0 &&
        r4->releasedFrames >= 1 && func_ov007_020c1da0(0) == 0) {
        SCENE->awaitingTap = 0;
        func_ov007_020c6724(SCENE->picker, 0);
        SCENE->strokeCount = 0;
    }
    {
        Ov007PickerScene *m = SCENE;
        Ov007StrokePath *o = m->picker->path;
        if (o->pointCount == 0)
            m->strokeTime = 0;
        else
            m->strokeTime += 0x1000;
    }
    if (func_ov007_020c1da0(0) == 0) {
        char *r3 = data_ov007_02104ba0;
        int st = ((Ov007PickerScene *)r3)->states[1]->currentState;
        if (st != 6 && st != 7 && ((Ov007PickerScene *)r3)->awaitingTap == 0) {
            if (r4->cur.touch != 0) {
                unsigned int cnt;
                Ov007StrokePath *o;
                int flag;
                int x;
                int y;
                o = ((Ov007PickerScene *)r3)->picker->path;
                r5 = 0;
                flag = 0;
                cnt = o->pointCount;
                x = r4->cur.x;
                y = r4->cur.y;
                if (cnt >= 2) {
                    int idx = cnt - 2;
                    int dy = o->yCoords[idx] >> 12;
                    int moveY = r4->deltaY;
                    int dx = (o->xCoords[idx] >> 12) - x;
                    int dd = r4->deltaX * r4->deltaX + moveY * moveY;
                    int dd2;
                    dy -= y;
                    dd2 = dx * dx + dy * dy;
                    if (dd >= 0x100 && dd2 <= 4)
                        flag = 1;
                }
                if (flag == 0) {
                    r5 = func_ov007_020c65bc(((Ov007PickerScene *)r3)->picker, x, y,
                                             ((Ov007PickerScene *)r3)->strokeTime >> 12);
                    func_ov007_020bdbcc(r4->deltaX, r4->deltaY);
                } else {
                    Ov007Touch *obj = WORK->touch;
                    ((u32 *)obj)[11] = obj->flags | 1;
                }
                if (r5 == 2) {
                    func_ov007_020c6550(SCENE->picker);
                    SCENE->strokeTime = 0;
                    r6 = 1;
                } else if (r5 == 1) {
                    Ov007Touch *obj = WORK->touch;
                    obj->countdown = 0;
                }
            } else if (r4->cur.touch == 0) {
                Ov007Touch *obj = WORK->touch;
                if (obj->releasedFrames == 1) {
                    int res = func_ov007_020c6550(((Ov007PickerScene *)r3)->picker);
                    if (res != 0) {
                        Ov007PickerScene *s;
                        if (res == 2)
                            r6 = 1;
                        s = SCENE;
                        s->strokeCount = s->picker->groupCount;
                        SCENE->strokeTime = 0;
                        obj = WORK->touch;
                        ((u32 *)obj)[11] = obj->flags | 1;
                    }
                }
            }
        }
    }
    PickerScene_HandleDrawingKeys();
    return r6;
}

int PickerScene_FadeToEdit(void) {
    Ov007PickerSprites *base = SPRITES;
    Ov007Sprite *p1 = base->sprites[0];
    Ov007Sprite *p2 = base->sprites[1];
    s32 *addr4 = &p2->alpha;
    s32 *addr1 = &p1->alpha;

    if (*addr1 < 0x1f000) {
        Ov007PickerScene *c = SCENE;
        if (c->states[1]->currentState == 5) {
            *addr1 = 0x1f000;
        }
    } else {
        *addr4 -= 0x1000;
        if (*addr4 <= 0) {
            *addr4 = 0;
        }
    }

    SCENE->tweenOut[1] = FX_DivS32(*addr4, 0x1f);

    return (*addr4 == 0) ? 1 : 0;
}

/* Commits a stroke still being drawn, settles the picker, clears the
 * requests on states[2] and [3] and presets idleFrames to its 0xf0 limit. */
void PickerScene_BeginEdit(void) {
    if (SCENE->picker->path->pointCount != 0) {
        func_ov007_020c6550(SCENE->picker);
    }
    func_ov007_020c5c14(SCENE->picker, &SCENE->picker->pos);
    func_ov007_020c5ef0(SCENE->picker);
    SCENE->picker->func_ov007_020c510c();

    SCENE->idleFrames = 0;
    SCENE->states[2]->requestedState = 0;
    SCENE->states[3]->requestedState = 0;
    SCENE->idleFrames = 0xf0;
    SPRITES->slideProgress = 0x1000;
}

void PickerScene_UpdateEdit(void) {
    Ov007Picker *r5 = SCENE->picker;
    Ov007Touch *r4 = WORK->touch;
    int v[3];

    PickerScene_HandleEditKeys();

    if (r4->cur.touch != 0 && r4->heldFrames >= 1 && func_ov007_020c1da0(0) == 0) {
        Ov007Camera *camera;
        SCENE->springSet = 0;
        camera = WORK->cameras[0];
        func_ov007_020bfe4c(camera, r4->cur.x << 12, r4->cur.y << 12, -camera->distance,
                            (Vector3 *)v);
        if (r4->heldFrames == 1) {
            int s;
            func_ov007_020c5a6c(r5, v, 0);
            s = 0x25;
            if (FX_ModS32(SCENE->restartCount, 3) == 2) {
                switch (FX_ModS32(SCENE->restartCount, 9)) {
                case 2:
                    s = 0x26;
                    break;
                case 5:
                    s = 0x27;
                    break;
                case 8:
                    s = 0x28;
                    break;
                }
            }
            func_ov007_020bdeb0(s);
        }
    } else if (r4->releasedFrames == 0) {
        u32 t = r4->heldFrames;
        if (t <= 5) {
            PickerScene_SetDeform(0, 0, 0);
        } else if (SCENE->picker->hitGroup != -1) {
            int f;
            Ov007Picker *picker;
            if ((int)t <= 5)
                f = 0;
            else if ((int)t >= 0x12c)
                f = 0x1000;
            else
                f = FX_DivS32((t - 5) << 12, 0x127);
            picker = SCENE->picker;
            if (picker->groupCount >= 2)
                func_ov007_020c6314(picker, picker->hitGroup, 2);
            func_ov007_020c5fc8(SCENE->picker, SCENE->picker->hitGroup, f);
        }
        r5->hitIndex = -1;
        r5->hitGroup = r5->hitIndex;
        r5->hitOffset.z = 0;
        r5->hitOffset.y = r5->hitOffset.z;
        r5->hitOffset.x = r5->hitOffset.y;
    }

    func_ov007_020c5948(r5, (Vector3 *)v);

    if (r4->cur.touch != 0 || WORK->pad->keys != 0) {
        SCENE->idleFrames = 0;
    } else {
        SCENE->idleFrames++;
    }
}

void PickerScene_Create(int mode, int drawingIndex) {
    int centerY, size, spriteMode, startAlpha;
    int i;
    Ov007Sprite *sprite;
    Ov007Sprite *itemSprite;
    Ov007Sprite *bar;

    if (data_ov007_02104ba0 != 0)
        return;

    data_ov007_02104ba0 = (char *)func_ov007_020c3df4(0, 0x4c);
    data_ov007_02104b9c = (Ov007PickerSprites *)func_ov007_020c3df4(0, 0x14);

    SCENE->picker = func_ov007_020c690c(0xc8, 0x1f4, 0x1f4, WORK->cameras[0], &data_ov007_020d7b60,
                                        data_ov007_020d7b80);

    SCENE->states[0] = (Ov007MenuState *)func_ov007_020c9388(0);
    SCENE->states[1] = (Ov007MenuState *)func_ov007_020c9388(-1);
    SCENE->states[2] = (Ov007MenuState *)func_ov007_020c9388(0);
    SCENE->states[3] = (Ov007MenuState *)func_ov007_020c9388(0);

    SCENE->springSet = 0;
    SCENE->introVariant = 0;
    SCENE->idleFrames = 0;
    SCENE->restartCount = 0;
    SCENE->strokeCount = 0;
    SCENE->replayFrames = 0;

    SCENE->tweens[0] = func_ov007_020c908c(&SCENE->tweenOut[0]);
    SCENE->tweens[1] = func_ov007_020c908c(&SCENE->tweenOut[1]);

    startAlpha = 0;
    centerY = 0x60000;
    size = 0x100;
    spriteMode = 1;

    for (i = 0; i < 2; i++) {
        SPRITES->sprites[i] = func_ov007_020c44e4(spriteMode);
        sprite = SPRITES->sprites[i];

        if (i == 0) {
            sprite->frames[0] = WORK->textures[3];
            sprite->depth = 0xfffcdf01;
            sprite->polygonId = 0x18;
        } else {
            sprite->frames[0] = WORK->textures[4];
            sprite->depth = -0x32000;
            sprite->polygonId = 0x17;
        }

        sprite->repeatV = 8;
        sprite->repeatU = sprite->repeatV;
        sprite->screenX = 0x80000;
        sprite->screenY = centerY;
        sprite->width = (s16)size;
        sprite->height = (s16)size;
        sprite->alpha = startAlpha;

        func_ov007_020c43bc(sprite, WORK->cameras[1]);
        func_ov007_020c4388(sprite, WORK->cameras[1]);
    }

    SPRITES->sprites[2] = func_ov007_020c44e4(1);
    bar = SPRITES->sprites[2];
    bar->width = 0x48;
    bar->height = 8;
    bar->cornerColors[3] = 0x7f8a;
    bar->cornerColors[0] = bar->cornerColors[3];
    bar->cornerColors[2] = 0x6e02;
    bar->cornerColors[1] = bar->cornerColors[2];
    bar->drawFlags = bar->drawFlags | 2;
    bar->polygonId = 0x19;

    SPRITES->sprites[3] = func_ov007_020c44e4(1);
    bar = SPRITES->sprites[3];
    bar->width = 0x48;
    bar->height = 8;
    bar->color = 0x294a;
    bar->alpha = 0x5000;
    bar->drawFlags = bar->drawFlags | 1;
    bar->polygonId = 0x1a;

    itemSprite = WORK->itemViews[5]->sprite;
    SPRITES->sprites[2]->depth = itemSprite->depth - 0x10;
    SPRITES->sprites[3]->depth = itemSprite->depth - 0xff;

    func_ov007_020c43bc(SPRITES->sprites[2], WORK->cameras[1]);
    func_ov007_020c43bc(SPRITES->sprites[3], WORK->cameras[1]);

    SCENE->strokeTime = 0;
    SCENE->tweens[0]->to = 0x800;
    SCENE->tweenOut[0] = SCENE->tweens[0]->to;

    if (mode != 0) {
        if (mode == 1) {
            SCENE->awaitingTap = 0;
            SCENE->tweenOut[1] = 0;
            SCENE->states[1]->requestedState = 0;
        }
    } else {
        SCENE->awaitingTap = 1;
        SCENE->tweenOut[1] = 0x1000;
        SCENE->states[1]->requestedState = 2;
    }

    if (mode != 1)
        return;

    func_ov007_020bbe60(drawingIndex);
}

void PickerScene_Destroy(void) {
    int i;

    if (SCENE == 0)
        return;

    func_ov007_020c6684(SCENE->picker);
    func_ov007_020c937c(SCENE->states[0]);
    func_ov007_020c937c(SCENE->states[1]);
    func_ov007_020c937c(SCENE->states[2]);
    func_ov007_020c937c(SCENE->states[3]);
    func_ov007_020c9080(SCENE->tweens[0]);
    func_ov007_020c9080(SCENE->tweens[1]);

    for (i = 0; i < 2; i++)
        func_ov007_020c44c4(SPRITES->sprites[i]);

    func_ov007_020c44c4(SPRITES->sprites[2]);
    func_ov007_020c44c4(SPRITES->sprites[3]);
    func_ov007_020c3d1c(SCENE);
    data_ov007_02104ba0 = 0;
    func_ov007_020c3d1c(SPRITES);
    data_ov007_02104b9c = 0;
}

void PickerScene_Update(void) {
    Ov007MenuState *p;
    short st;
    int ticks;

    func_ov007_020c92d0(SCENE->states[0]);
    p = SCENE->states[0];
    st = p->currentState;
    ticks = p->stateTicks;
    switch (st) {
    case 1:
        if (PickerScene_UpdateIntro(ticks)) {
            SCENE->states[0]->requestedState = 2;
        }
        break;

    case 2:
        if (ticks == 0) {
            PickerScene_StartDrawing();
        }
        if (PickerScene_UpdateDrawing()) {
            SCENE->states[0]->requestedState = 3;
        }
        break;

    case 5:
        if (ticks == 0) {
            PickerScene_BeginEdit();
            SCENE->states[1]->requestedState = 4;
        } else if (PickerScene_FadeToEdit() != 0) {
            SCENE->states[0]->requestedState = 4;
            SCENE->states[1]->requestedState = 8;
        }

    case 4:
        PickerScene_UpdateEdit();
        PickerScene_UpdatePicker();
        break;
    }
}

void func_ov007_020b91b4(void) {
    int r7;
    Ov007Picker *r6;
    int r5;
    int r4;
    short st;
    int t;
    r6 = SCENE->picker;
    func_ov007_020c92d0(SCENE->states[1]);
    r4 = SCENE->states[1]->stateTicks;
    func_ov007_020c8f98(SCENE->tweens[0]);
    func_ov007_020c8f98(SCENE->tweens[1]);
    SCENE->totalPoints = func_ov007_020c5db0(SCENE->picker);
    r5 = func_ov007_020c3be0(SCENE->tweenOut[1], 0, 0x59e8);
    t = SCENE->tweenOut[0];
    if (t >= 0x1000) {
        r7 = 0x200;
    } else if (t <= 0) {
        r7 = 0xc0;
    } else {
        r7 = (int)(((long long)(0x1000 - t) * 0xc0 + (long long)t * 0x200) >> 12);
    }
    st = SCENE->states[1]->currentState;
    switch (st) {
    case 0:

        do {
        case 1:
            if ((SCENE->states[1]->stateTicks) == 0) {
                Ov007Picker *a0 = SCENE->picker;
                a0->limitGroup = 0;
                a0->limitIndex = 0;
                SCENE->strokeTime = 0;
                SCENE->totalStrokeTime = func_ov007_020c5d5c(SCENE->picker);
                SPRITES->slideProgress = 0;
            }
            {
                int b = PickerScene_UpdateReplay(
                    (int)(((SCENE->states[1]->currentState) == 0) ? ((char *)0) : ((char *)1)));
                func_ov007_020c4dfc(r6, r7, r5, 0x1f);
                if (b == 0) {
                    return;
                }
            }
            SCENE->awaitingTap = 0;
            SCENE->states[1]->requestedState = 8;
            return;
        case 2:
            if (r4 == 0) {
                SPRITES->sprites[0]->alpha = 0;
                SPRITES->sprites[1]->alpha = 0;
                SPRITES->slideProgress = 0x1000;
            }
            {
                int q;
                q = ProgressRatio(r4, 0x28);
                SPRITES->sprites[1]->alpha = q * 0xc;
                if (q < 0x1000) {
                    return;
                }
            }
            SCENE->states[1]->requestedState = 5;
            return;
        case 3:
        case 4:
        case 5: {
            int *p = &SPRITES->sprites[1]->alpha;
            if (st == 4) {
                *p += 0x1000;
                if ((*p) >= 0x1f000) {
                    *p = 0x1f000;
                    SPRITES->sprites[0]->alpha = 0x1f000;
                    SCENE->states[1]->requestedState = 5;
                }
            } else if (st == 3) {
                if (r4 == 0) {
                    SPRITES->sprites[0]->alpha = 0;
                }
                *p -= 0x1000;
                if ((*p) <= 0xc000) {
                    *p = 0xc000;
                    SCENE->states[1]->requestedState = 5;
                }
            }
        }
            if ((SCENE->awaitingTap) != 0) {
                return;
            }
            func_ov007_020c4f50(r6, r7, r5, 0x1f, 1);
            return;
        case 6:
        case 7: {
            int r8v;
            int sb;
            int q;
            if (st == 6) {
                r8v = r6->groupCount;
            } else {
                r8v = (r6->groupCount) - 1;
            }
            q = ProgressRatio(r4, 0xf);
            if ((SCENE->states[1]->currentState) == 6) {
                q = 0x1000 - q;
            }
            sb = (q * 0x1f) >> 0xc;
            if (sb <= 0) {
                sb = 1;
            }
            func_ov007_020c4e98(r6, r7, r5, 0x1f, r8v, func_ov007_020c3be0(q, 0x18df, r5), sb);
            if (r4 < 0xf) {
                return;
            }
        }
            SCENE->states[1]->requestedState = 5;
            {
                Ov007Touch *o = WORK->touch;
                /* the read through a const view is load-bearing: `o->flags |= 1`
                 * builds 4 bytes longer */
                o->flags = ((const Ov007Touch *)o)->flags | 1;
            }
            return;
        } while (0);
    case 8:
        if ((SCENE->awaitingTap) != 0) {
            SCENE->states[1]->requestedState = 1;
            return;
        }
        func_ov007_020c4f50(r6, r7, r5, 0x1f, 1);
        return;
    }
}

void func_ov007_020b8fd4(Ov007Camera *camera) {
    int v;
    int mode;
    int d;
    Ov007Sprite *b;
    int other;
    int scale;
    Ov007Sprite *p_c;
    Ov007Sprite *p_10;
    Ov007ItemView *p5;
    int f8;
    Ov007Picker *inner;
    Ov007Item *node;
    Ov007MenuState *ps;
    int flag;
    int f14;
    int sh;
    v = SPRITES->slideProgress;
    if (v <= 0x1000) {
        SPRITES->sprites[1]->screenX = 0x80000;
        b = SPRITES->sprites[1];
        b->screenY = 0x60000;
        mode = SCENE->introVariant;
        switch (mode) {
        case 0:
            b = SPRITES->sprites[1];
            b->screenX += (0x1000 - v) << 8;
            break;
        case 3:
            d = 0x1000 - v;
            b = SPRITES->sprites[1];
            b->screenY += (-d) << 8;
            break;
        }
        func_ov007_020c4388(SPRITES->sprites[1], WORK->cameras[1]);
    }
    func_ov007_020c421c(SPRITES->sprites[0]);
    func_ov007_020c421c(SPRITES->sprites[1]);

    {
        Ov007PickerScene *c;
        Ov007Work *g;
        f8 = (int)data_ov007_02104b9c;
        c = *((Ov007PickerScene *volatile *)&data_ov007_02104ba0);
        g = *((Ov007Work *volatile *)&data_ov007_0210342c);
        flag = c->awaitingTap;
        p5 = g->itemViews[5];
        inner = c->picker;
        node = p5->item;
        ps = node->state;
        p_c = ((Ov007PickerSprites *)f8)->sprites[2];
        p_10 = ((Ov007PickerSprites *)f8)->sprites[3];
        f8 = inner->groupCount;
        sh = ps->currentState;
        f14 = inner->totalPoints;
        if (flag) {
            f8 = 0;
            f14 = f8;
        }
    }

    if (sh == 1) {
        return;
    }
    scale = FX_DivS32(f14 << 12, 500);
    other = FX_DivS32(f8 << 12, 200);
    if (scale <= other) {
        scale = other;
    }
    p_c->screenX = p5->item->posX + (((int)(p_c->width * scale)) / 2);
    p_c->screenY = p5->item->posY;
    p_c->scaleX = 0x1000 - scale;
    p_10->screenX = p5->item->posX;
    p_10->screenY = p5->item->posY;
    func_ov007_020c4388(p_c, camera);
    func_ov007_020c4388(p_10, camera);
    func_ov007_020c421c(p_c);
    func_ov007_020c421c(p_10);
}

int PickerScene_IsIdle(void) {
    Ov007PickerScene *p = SCENE;
    return p->idleFrames >= 0xf0;
}

/* Getters for the rest of the menu. */
int PickerScene_IsAwaitingTap(void) {
    return SCENE->awaitingTap;
}

Ov007MenuState *PickerScene_GetMainState(void) {
    return SCENE->states[0];
}

Ov007MenuState *PickerScene_GetDisplayState(void) {
    return SCENE->states[1];
}

Ov007MenuState *PickerScene_GetScaleAnimState(void) {
    return SCENE->states[2];
}

Ov007MenuState *PickerScene_GetMoveAnimState(void) {
    return SCENE->states[3];
}
