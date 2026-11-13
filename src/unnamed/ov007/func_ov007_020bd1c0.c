#include "types.h"
#include "Ov007Work.h"
#include "Ov007Settings.h"
#include "Ov007Light.h"
#include "nitro/gx/g3x.h"
#define PickerScene_Create _Z18PickerScene_Createii

/* The lights' source points: lights[0] per menu, lights[1] in menu 0. */
extern Vector3 data_ov007_020d7c34[];
extern Vector3 data_ov007_020d7c28;

#define func_ov007_020bce70 _Z19func_ov007_020bce70si
#define func_ov007_020b3f20 _Z19func_ov007_020b3f20i
#define func_ov007_020b1fa4 _Z19func_ov007_020b1fa4i
#define func_ov007_020bd4a0 _Z19func_ov007_020bd4a0i
#define func_ov007_020b2998 _Z19func_ov007_020b2998v
#define func_ov007_020bfbdc _Z19func_ov007_020bfbdcPv
#define func_ov007_020c14b8 _Z19func_ov007_020c14b8P12Ov007Texture
#define func_ov007_020b77ac _Z19func_ov007_020b77acv
#define func_ov007_020bf428 _Z19func_ov007_020bf428v
extern void func_ov007_020bce70(short n, int m);
extern void* func_ov007_020b3f20(int idx);
extern void func_ov007_020b22e0(int* a, int* b, int c);
extern void func_ov007_020b1fa4(int self);
extern void func_ov007_020bd4a0(int r0);
extern void func_ov007_020b2998(void);
extern void func_ov007_020bfbdc(void* self);
extern void func_ov007_020c14b8(struct Ov007Texture *t);
extern void func_ov007_020b77ac(void);
extern void PickerScene_Create(int a, int b);
extern void func_ov007_020bf428(void);
extern void func_ov007_020b3edc(int r0);

void func_ov007_020bd1c0(int n)
{
    int res = 0;
    int i;
    void* h;

    func_ov007_020bce70(0, 0);

    WORK->lights[0]->source = data_ov007_020d7c34[n];

    h = func_ov007_020b3f20(0x16);
    for (i = 0; i < 11; i++) {
        func_ov007_020b22e0(WORK->textures[i], h, i);
    }

    {
        int t = WORK->settings->language - 2;
        if (t >= 0) {
            void* h2;
            res = t + 0x20;
            h2 = func_ov007_020b3f20(res);
            func_ov007_020b22e0(WORK->textures[2], h2, 0);
            func_ov007_020b22e0(WORK->textures[6], h2, 1);
        }
    }

    switch (n) {
    case 0:
        {
            func_ov007_020b1fa4(1);
            G3X_SetShading(GX_SHADING_TOON);
            WORK->lights[1]->source = data_ov007_020d7c28;
            WORK->lights[1]->color = 0x18c6;
            func_ov007_020bd4a0(0);
            func_ov007_020b2998();
            WORK->modelGrowth = 0;
            func_ov007_020bfbdc(WORK->cameras[0]);
            func_ov007_020bd4a0(1);
            func_ov007_020c14b8(WORK->textures[8]);
            func_ov007_020c14b8(WORK->textures[7]);
            func_ov007_020c14b8(WORK->textures[6]);
            func_ov007_020c14b8(WORK->textures[9]);
            func_ov007_020c14b8(WORK->textures[10]);
            func_ov007_020b77ac();
        }
        break;
    case 1:
        func_ov007_020b1fa4(2);
        func_ov007_020bd4a0(0);
        func_ov007_020bfbdc(WORK->cameras[0]);
        func_ov007_020bd4a0(1);
        func_ov007_020c14b8(WORK->textures[1]);
        func_ov007_020c14b8(WORK->textures[3]);
        func_ov007_020c14b8(WORK->textures[4]);
        PickerScene_Create(WORK->transitionMode, WORK->figureIndex);
        break;
    case 2:
        {
            func_ov007_020b1fa4(3);
            G3X_SetShading(GX_SHADING_HIGHLIGHT);
            func_ov007_020bce70(4, 0);
            func_ov007_020bd4a0(0);
            func_ov007_020bfbdc(WORK->cameras[0]);
            func_ov007_020bd4a0(1);
            func_ov007_020c14b8(WORK->textures[2]);
            func_ov007_020c14b8(WORK->textures[0]);
            func_ov007_020c14b8(WORK->textures[10]);
            func_ov007_020bf428();
        }
        break;
    }

    func_ov007_020b3edc(0x16);
    if (res != 0) {
        func_ov007_020b3edc(res);
    }
}
