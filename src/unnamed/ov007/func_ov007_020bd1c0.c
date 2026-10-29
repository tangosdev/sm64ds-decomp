#include "types.h"
#include "Ov007Work.h"
typedef struct { int a, b, c; } S12;

/* One of the overlay's two lights (func_ov007_020c241c, Work lights[0/1]);
 * color as in func_ov007_020b1718, only the slots this file writes. */
typedef struct Ov007Light {
    char pad_00[0x10];
    /* 0x10, medium confidence: the point the light shines from;
     * func_ov007_020c2390 aims it along +0x04 - +0x10 */
    S12 source;
    char pad_1c[6];
    u16 color; /* 0x22: func_ov007_020c241c seeds 0x7fff */
} Ov007Light;

/* The 0x3c-byte record at the manager's +0x28; only +0x3a is known here. */
typedef struct Ov007Settings {
    char pad_00[0x3a];
    u8 language; /* 0x3a: func_ov007_020b6a84 seeds it from the owner-info language */
} Ov007Settings;

extern char* data_ov007_0210342c;
extern S12 data_ov007_020d7c34[];
extern S12 data_ov007_020d7c28;

/* The extern keeps the tree's char * spelling (the declaration-agreement
 * gate); the typed view is reached through a cast macro. */
#define WORK ((Ov007Work *)data_ov007_0210342c)

extern void func_ov007_020bce70(short n, int m);
extern void* func_ov007_020b3f20(int idx);
extern void func_ov007_020b22e0(int* a, int* b, int c);
extern void func_ov007_020b1fa4(int self);
extern void func_ov007_020bd4a0(int r0);
extern void func_ov007_020b2998(void);
extern void func_ov007_020bfbdc(void* self);
extern void func_ov007_020c14b8(void* t);
extern void func_ov007_020b77ac(void);
extern void func_ov007_020b9880(int a, int b);
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
        func_ov007_020b22e0((int *)WORK->textures[i], (int *)h, i);
    }

    {
        int t = WORK->settings->language - 2;
        if (t >= 0) {
            void* h2;
            res = t + 0x20;
            h2 = func_ov007_020b3f20(res);
            func_ov007_020b22e0((int *)WORK->textures[2], (int *)h2, 0);
            func_ov007_020b22e0((int *)WORK->textures[6], (int *)h2, 1);
        }
    }

    switch (n) {
    case 0:
        {
            volatile u16* reg = (volatile u16*)0x4000060;
            func_ov007_020b1fa4(1);
            *reg = *reg & ~0x3002;
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
        func_ov007_020b9880(WORK->transitionMode, *(int *)(data_ov007_0210342c + 0xec));
        break;
    case 2:
        {
            volatile u16* reg = (volatile u16*)0x4000060;
            func_ov007_020b1fa4(3);
            *reg = (*reg & ~0x3002) | 2;
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
