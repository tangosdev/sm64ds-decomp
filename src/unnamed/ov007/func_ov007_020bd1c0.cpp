//cpp
#include "types.h"
#include "Ov007Work.h"
#include "Ov007Settings.h"
#include "Ov007Light.h"
#include "Ov007Texture.h"
#include "BMD_File.h"
#include "nitro/gx/g3x.h"
#include "nitro/fx/fx.h"
#include "Vector3Bits.h"
/* Kept (#821 r72): no C++ spelling of three named ints (VecFx32, Vector3, a
   plain or tagged struct, a temporary) copies memory to memory with ldm/stm;
   the same copy compiled as C does (measured on func_ov007_020c5dec). */
/* Kept (#821 r75): byte-identical as C too, but its 11 Ov007Texture member calls need C++. */

/* The lights' source points: lights[0] per menu, lights[1] in menu 0. */
extern "C" VecFx32 data_ov007_020d7c34[];
extern "C" VecFx32 data_ov007_020d7c28;

extern "C" void func_ov007_020bce70(short n, int m);
extern "C" void *func_ov007_020b3f20(int idx);
extern "C" void func_ov007_020b22e0(void *tex, void *file, int i);
extern "C" void func_ov007_020b1fa4(int self);
extern "C" void func_ov007_020bd4a0(int r0);
extern "C" void func_ov007_020b2998(void);
extern "C" void func_ov007_020bfbdc(void *self);
extern "C" void func_ov007_020b77ac(void);
extern "C" void PickerScene_Create(int a, int b);
extern "C" void func_ov007_020bf428(void);
extern "C" void func_ov007_020b3edc(int r0);

extern "C" void func_ov007_020bd1c0(int n) {
    int res = 0;
    int i;
    void *h;

    func_ov007_020bce70(0, 0);

    /* Kept (#821 r76): the Ov007Texture member calls keep this file C++ (r75: a C probe is
       identical), and C++ copies a VecFx32 memberwise (here and below). */
    *(Vector3Bits *)&gMenuWork->lights[0]->source = *(Vector3Bits *)&data_ov007_020d7c34[n];

    h = func_ov007_020b3f20(0x16);
    for (i = 0; i < 11; i++) {
        func_ov007_020b22e0(gMenuWork->textures[i], h, i);
    }

    {
        int t = gMenuWork->settings->language - 2;
        if (t >= 0) {
            void *h2;
            res = t + 0x20;
            h2 = func_ov007_020b3f20(res);
            func_ov007_020b22e0(gMenuWork->textures[2], h2, 0);
            func_ov007_020b22e0(gMenuWork->textures[6], h2, 1);
        }
    }

    switch (n) {
    case 0: {
        func_ov007_020b1fa4(1);
        G3X_SetShading(GX_SHADING_TOON);
        *(Vector3Bits *)&gMenuWork->lights[1]->source = *(Vector3Bits *)&data_ov007_020d7c28;
        gMenuWork->lights[1]->color = 0x18c6;
        func_ov007_020bd4a0(0);
        func_ov007_020b2998();
        gMenuWork->modelGrowth = 0;
        func_ov007_020bfbdc(gMenuWork->cameras[0]);
        func_ov007_020bd4a0(1);
        ((Ov007Texture *)gMenuWork->textures[8])->func_ov007_020c14b8();
        ((Ov007Texture *)gMenuWork->textures[7])->func_ov007_020c14b8();
        ((Ov007Texture *)gMenuWork->textures[6])->func_ov007_020c14b8();
        ((Ov007Texture *)gMenuWork->textures[9])->func_ov007_020c14b8();
        ((Ov007Texture *)gMenuWork->textures[10])->func_ov007_020c14b8();
        func_ov007_020b77ac();
    } break;
    case 1:
        func_ov007_020b1fa4(2);
        func_ov007_020bd4a0(0);
        func_ov007_020bfbdc(gMenuWork->cameras[0]);
        func_ov007_020bd4a0(1);
        ((Ov007Texture *)gMenuWork->textures[1])->func_ov007_020c14b8();
        ((Ov007Texture *)gMenuWork->textures[3])->func_ov007_020c14b8();
        ((Ov007Texture *)gMenuWork->textures[4])->func_ov007_020c14b8();
        PickerScene_Create(gMenuWork->transitionMode, gMenuWork->figureIndex);
        break;
    case 2: {
        func_ov007_020b1fa4(3);
        G3X_SetShading(GX_SHADING_HIGHLIGHT);
        func_ov007_020bce70(4, 0);
        func_ov007_020bd4a0(0);
        func_ov007_020bfbdc(gMenuWork->cameras[0]);
        func_ov007_020bd4a0(1);
        ((Ov007Texture *)gMenuWork->textures[2])->func_ov007_020c14b8();
        ((Ov007Texture *)gMenuWork->textures[0])->func_ov007_020c14b8();
        ((Ov007Texture *)gMenuWork->textures[10])->func_ov007_020c14b8();
        func_ov007_020bf428();
    } break;
    }

    func_ov007_020b3edc(0x16);
    if (res != 0) {
        func_ov007_020b3edc(res);
    }
}
