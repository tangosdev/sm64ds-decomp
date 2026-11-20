//cpp
/* unit020bed60 -- ov007 unit 0x020bed60..0x020bf690: 7 functions, .text only.
 *
 * The title menu's rippling mesh backdrop. ov007 is the title and
 * file-select menu program that dScDSMT_c hosts; outside dScDSMT_c it has no
 * ROM RTTI or vtable, so no class name is claimed and the file keeps the
 * unit<addr> name.
 *
 * What the code does. The module owns one 0x30-byte record,
 * sMeshScene (Ov007MeshScene): a grid mesh (func_ov007_020cbbb0),
 * two materials it is drawn with, a wave that ripples it, and eight swings,
 * one per menu item, that tie item sprites to the mesh.
 * func_ov007_020bf428 builds it and func_ov007_020bf304 tears it down;
 * func_ov007_020beeb0 runs each frame: it pokes the mesh under the stylus
 * or the cursor's item (func_ov007_020bed60, through func_ov007_020bf630's
 * pick), ripples it, lets each item's swing move its sprite, and blends the
 * second material and the first light by the menu state.
 * func_ov007_020bf57c binds a swing to its item's sprite and
 * func_ov007_020bee14 draws the mesh with both materials.
 *
 * Edges: the bss word 0x02104bd4 is referenced by these functions and
 * nothing else in the overlay; func_ov007_020bf630 references no data and
 * is called only by func_ov007_020bed60. The previous function,
 * func_ov007_020beb80, references no ov007 data and is called only from
 * func_ov007_020be9ac; the next one, func_ov007_020bf690, starts the
 * screen-fade module (unit020bf690, bss 0x02104bd8).
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 *
 */
#include "common.h"
#include "nitro/fx/fx.h"
#include "types.h"
#include "Ov007MenuState.h"
#include "Ov007Sprite.h"
#include "Ov007Touch.h"
#include "Ov007Work.h"
#include "Ov007Swing.h"
#include "Ov007Camera.h"
#include "Ov007Item.h"
#include "Ov007Light.h"
#include "nitro/fx/fx_cp.h"
#include "math/ProgressRatio.h"
#include "Ov007MenuMem.h"

/* sMeshScene: the 0x30-byte record func_ov007_020bf428 allocates
 * and fills; medium-confidence name. The slot names follow their builders. */
typedef struct Ov007MeshScene {
    Ov007Mesh *mesh;                 /* 0x00: func_ov007_020cbbb0 */
    Ov007MeshMaterial *materials[2]; /* 0x04: func_ov007_020bee14 draws the mesh with each */
    Ov007MeshWave *wave;             /* 0x0c: func_ov007_020ca2a8 */
    Ov007Swing *swings[8];           /* 0x10: func_ov007_020ca010, one per item */
} Ov007MeshScene;

typedef char Ov007MeshScene_size_must_be_0x30[sizeof(Ov007MeshScene) == 0x30 ? 1 : -1];

extern Ov007MeshScene *sMeshScene;
extern VecFx32 data_ov007_020d7d60;
extern Ov007MeshParams data_ov007_020d7cb0;
extern u8 data_ov007_020d7c98[];
extern Ov007MeshWaveParams data_ov007_020d7c9c;
extern Ov007SwingParams data_ov007_020d7cc8;
extern Ov007MeshMaterial data_ov007_020d7cf8[2];

extern "C" Ov007MenuState *func_ov007_020b2cdc(void);
extern Ov007Sprite *NumberSprite_Create(int index);
extern "C" void func_ov007_020bce70(short n, int frames);
extern "C" void func_ov007_020bfe4c(Ov007Camera *camera, int x, int y, int z, VecFx32 *out);
extern int func_ov007_020c1da0(int i);
extern "C" int func_ov007_020c30ac(VecFx32 *arr, int count, VecFx32 *target, int maxDist, int *out);
extern "C" unsigned short func_ov007_020c3be0(int t, int a, int b);
extern void func_ov007_020ca1d4(Ov007MeshWave *wave, Ov007Mesh *mesh);
extern void func_ov007_020ca26c(Ov007MeshWave *wave);
extern Ov007MeshWave *func_ov007_020ca2a8(Ov007MeshWaveParams *params);
extern "C" void func_ov007_020ca578(Ov007Mesh *mesh, int i, int j, int t);
extern "C" void func_ov007_020ca5f0(Ov007Mesh *mesh, Ov007MeshMaterial *mat, VecFx32 *pos,
                                    int scaleZ, int smooth);
extern "C" void func_ov007_020cb3dc(Ov007Mesh *mesh);
extern "C" void func_ov007_020cba78(Ov007Mesh *mesh, int shift);
extern void func_ov007_020cbb04(Ov007Mesh *mesh);
extern "C" Ov007Mesh *func_ov007_020cbbb0(int columns, int rows, int width,
                                          Ov007MeshParams *params);
extern "C" Ov007MeshMaterial *func_ov007_020cbf8c(Ov007MeshMaterial *src);

/* Defined below, after their callers (ROM order). */
void func_ov007_020bed60(int x, int y, int strength);
extern "C" void func_ov007_020beeb0(void);

/* Picks the grid point of the mesh under screen point (x, y): the point is
 * unprojected through the camera at the camera's depth, then
 * func_ov007_020c30ac finds the mesh vertex nearest it. Returns its index,
 * or -1. */
int Ov007Mesh::func_ov007_020bf630(Ov007Camera *camera, int x, int y, int maxDist) {
    VecFx32 local;
    func_ov007_020bfe4c(camera, x << 0xc, y << 0xc, -camera->distance, &local);
    {
        Ov007Grid *grid = this->grid;
        return func_ov007_020c30ac(this->verts, grid->columns * grid->rows, &local, maxDist, 0);
    }
}

/* Places swing idx on item view idx + 0xe's sprite: a copy of the sprite,
 * set the camera's distance in front of it (camera is cameras[0] for both
 * callers), is sized and moved into the world, and the swing takes its
 * position and world size and is bound to the mesh. */
void func_ov007_020bf57c(Ov007Camera *camera, int idx) {
    Ov007Sprite *e = gMenuWork->itemViews[idx + 0xe]->sprite;
    Ov007Swing *swing = sMeshScene->swings[idx];

    Ov007Sprite buf = *e;
    buf.depth = -camera->distance;
    func_ov007_020c43bc(&buf, camera);
    func_ov007_020c4388(&buf, camera);
    swing->func_ov007_020c9f10(&buf.pos, buf.worldWidth, buf.worldHeight);
    swing->func_ov007_020c9d90(sMeshScene->mesh);
}

/* Builds the mesh scene once: the 0xf x 0xe grid mesh, its two materials
 * (each given one of the menu's textures), the wave and the eight swings
 * (each bound to its item), then the manager's three number sprites, and
 * runs the frame 0x14 times to settle the mesh. */
extern "C" void func_ov007_020bf428(void) {
    int i;
    Ov007MeshMaterial *w;
    u8 *bp;
    Ov007MeshMaterial *ret;

    if (sMeshScene != 0)
        return;

    sMeshScene = (Ov007MeshScene *)func_ov007_020c3df4(0, sizeof(Ov007MeshScene));
    sMeshScene->mesh = func_ov007_020cbbb0(15, 14, 0x6000, &data_ov007_020d7cb0);

    w = data_ov007_020d7cf8;
    bp = data_ov007_020d7c98;
    for (i = 0; i < 2; i++) {
        ret = func_ov007_020cbf8c(w);
        w++;
        sMeshScene->materials[i] = ret;
        sMeshScene->materials[i]->texture = (struct Ov007Texture *)gMenuWork->textures[*bp];
        bp++;
    }

    sMeshScene->wave = func_ov007_020ca2a8(&data_ov007_020d7c9c);

    for (i = 0; i < 8; i++) {
        sMeshScene->swings[i] = data_ov007_020d7cc8.func_ov007_020ca010();
        func_ov007_020bf57c(gMenuWork->cameras[0], i);
    }

    for (i = 0; i <= 2; i++) {
        gMenuWork->numberSprites[i] = NumberSprite_Create(i);
    }

    for (i = 0; i < 20; i++) {
        func_ov007_020beeb0();
    }
}

/* Tears down the mesh scene: frees the mesh, the wave, both materials and
 * the eight swings, then the manager's number sprites and the scene itself. */
extern "C" void func_ov007_020bf304(void) {
    int i;
    Ov007MeshScene *g;

    g = sMeshScene;
    if (g == 0) {
        return;
    }

    {
        Ov007Mesh *mesh = g->mesh;
        if (mesh != 0) {
            func_ov007_020cbb04(mesh);
            sMeshScene->mesh = 0;
        }
    }

    {
        Ov007MeshWave *wave = sMeshScene->wave;
        if (wave != 0) {
            func_ov007_020ca26c(wave);
            sMeshScene->wave = 0;
        }
    }

    for (i = 0; i < 2; i++) {
        if (sMeshScene->materials[i] != 0) {
            func_ov007_020c3d1c(sMeshScene->materials[i]);
            sMeshScene->materials[i] = 0;
        }
    }

    for (i = 0; i < 8; i++) {
        sMeshScene->swings[i]->func_ov007_020c9fa0();
    }

    for (i = 0; i <= 2; i++) {
        if (gMenuWork->numberSprites[i] != 0) {
            func_ov007_020c44c4(gMenuWork->numberSprites[i]);
            gMenuWork->numberSprites[i] = 0;
        }
    }

    func_ov007_020c3d1c(sMeshScene);
    sMeshScene = 0;
}

/* One frame of the backdrop: pokes the mesh under the stylus (harder for a
 * fast stroke) or, with no touch, under the cursor's settled item; ripples
 * and steps the mesh; lets each visible item's swing move its sprite; then
 * blends the second material towards the colours of the current main state
 * and the first light towards grey in the file-select states 0xa / 0xb. */
void func_ov007_020beeb0(void) {
    s16 palette;
    int diffuseTarget;
    int ambientTarget;
    Ov007Mesh *mesh;

    mesh = sMeshScene->mesh;

    if (gMenuWork->touch->cur.touch != 0) {
        int strength;
        if (func_ov007_020c1da0(0) == 0)
            strength = 0x1000;
        else
            strength = 0xbb8;
        if (gMenuWork->touch->heldFrames != 0) {
            int dy = gMenuWork->touch->deltaY;
            int dx = gMenuWork->touch->deltaX;
            int speed = FX_DivS32(FX_Sqrt((dx * dx + dy * dy) << 12), 30);
            int t = FX_MUL(strength, speed);
            if (t >= 0x1000)
                t = 0x1000;
            strength = (((s64)t * t) >> 12);
        }
        func_ov007_020bed60(gMenuWork->touch->cur.x, gMenuWork->touch->cur.y, strength);
    } else {
        /* the menu cursor's state (func_ov007_020b2cdc returns
         * Ov007Cursor::state) */
        Ov007MenuState *cursor = func_ov007_020b2cdc();
        int state;
        if (cursor->stateTicks == 0) {
            state = cursor->currentState;
            if (state >= 0 && state < 0x18) {
                state = cursor->previousState;
                if (state >= 0 && state < 0x18) {
                    Ov007ItemView *view = gMenuWork->itemViews[state];
                    Ov007Item *item = view->item;
                    func_ov007_020bed60(item->posX >> 0xc, item->posY >> 0xc, 0x7d0);
                }
            }
        }
    }

    func_ov007_020ca1d4(sMeshScene->wave, mesh);
    func_ov007_020cba78(mesh, 2);

    for (int i = 0; i < 8; i++) {
        Ov007Swing *swing = sMeshScene->swings[i];
        Ov007ItemView *view = gMenuWork->itemViews[i + 0xe];
        if (view->item->state->stateWork == 2)
            func_ov007_020bf57c(gMenuWork->cameras[0], i);
        if (view->item->visible != 0) {
            swing->func_ov007_020c9c24(2);
            swing->func_ov007_020c9a84(&view->sprite->posOffset, &view->sprite->rotX,
                                       &view->sprite->rotY);
            view->sprite->posOffset.x = FX_MUL(view->sprite->posOffset.x, 100);
            view->sprite->posOffset.y = FX_MUL(view->sprite->posOffset.y, 100);
        }
    }

    int fadeFrames;
    switch (gMenuWork->states[1]->currentState) {
    case 0:
    case 1:
    case 11:
    case 12:
        palette = 4;
        fadeFrames = 30;
        diffuseTarget = 0x7fff;
        ambientTarget = 0x4800;
        break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        palette = 5;
        fadeFrames = 60;
        diffuseTarget = 0x575c;
        ambientTarget = 0x178;
        break;
    case 7:
    case 8:
        palette = 6;
        fadeFrames = 60;
        diffuseTarget = 0x631a;
        ambientTarget = 0x7fbf;
        break;
    case 9:
    case 10:
        fadeFrames = 60;
        diffuseTarget = 0x7fff;
        ambientTarget = 0x49e0;
        palette = 7;
        break;
    default:
        break;
    }

    func_ov007_020bce70(palette, fadeFrames);

    {
        Ov007MeshMaterial *mat = sMeshScene->materials[1];
        int ticks = gMenuWork->paletteState->stateTicks;
        u16 *diffuse = &mat->diffuse;
        u16 *ambient = &mat->ambient;
        int ratio = ProgressRatio(ticks, fadeFrames);
        *diffuse = func_ov007_020c3be0(ratio, *diffuse, diffuseTarget);
        *ambient = func_ov007_020c3be0(ratio, *ambient, ambientTarget);
    }

    {
        int ratio;
        Ov007MenuState *fileState = gMenuWork->states[2];
        int color = 0x7fff;
        ratio = 0x1000;
        int state = fileState->currentState;
        if (state == 0xb) {
            color = 0x3def;
            ratio = ProgressRatio(fileState->stateTicks, 30);
        } else if (state == 0xa) {
            ratio = ProgressRatio(fileState->stateTicks, 30);
        }
        gMenuWork->lights[0]->color =
            func_ov007_020c3be0(ratio, gMenuWork->lights[0]->color, color);
    }
}

/* Draws the mesh twice, once with each material, 0x200 and 0x400 in front
 * of the template point data_ov007_020d7d60. */
void func_ov007_020bee14(void) {
    VecFx32 v = data_ov007_020d7d60;
    v.z += 0x200;
    func_ov007_020ca5f0(sMeshScene->mesh, sMeshScene->materials[0], &v, 0x3e8, 1);
    func_ov007_020cb3dc(sMeshScene->mesh);
    v.z += 0x200;
    func_ov007_020ca5f0(sMeshScene->mesh, sMeshScene->materials[1], &v, 0x3e8, 1);
}

/* Pokes the mesh at screen point (x, y): func_ov007_020bf630 picks the
 * grid point under it through the camera, func_ov007_020ca578 pushes that
 * point with that strength, and the waves hold off for 0x78 ticks. */
void func_ov007_020bed60(int x, int y, int strength) {
    Ov007Mesh *mesh = sMeshScene->mesh;
    Ov007Camera *camera = gMenuWork->cameras[0];
    int res = mesh->func_ov007_020bf630(camera, x, y, -1);
    if (res == -1)
        return;
    {
        int m = FX_ModS32(res, mesh->grid->columns);
        int d = FX_DivS32(res, mesh->grid->columns);
        func_ov007_020ca578(mesh, m, d, strength);
    }
    sMeshScene->wave->delay = 120;
}
