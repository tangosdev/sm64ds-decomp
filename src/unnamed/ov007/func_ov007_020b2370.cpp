//cpp
#include "types.h"
#include "Ov007Work.h"
extern "C" {
void func_ov007_020bc02c(void* a, void* b);
void func_ov007_020be9ac(void* a, void* b, void* c);
void func_ov007_020bbff0(void* c);
}
extern char* data_ov007_0210342c;
#define LAUNDER(x) ((char*)(int)(x))

/* The 0xd8-byte camera func_ov007_020c023c allocates; names as in
 * func_ov007_020c0128, only the slot this file reads. */
struct Ov007Camera {
    char pad_00[0x44];
    int viewMatrix[12]; /* 0x44: Matrix4x3 */
};

/* The 0xa0-byte model record func_ov007_020bc6d4 allocates; names as in
 * func_ov007_020bc434, only the slot this file writes. */
struct Ov007Model {
    char pad_00[0x40];
    Vector3 scale; /* 0x40: ModelComponents::Render's scale (func_ov007_020bbff0) */
};

/* The 0x1c-byte record func_ov007_020beaa0 builds around an Ov007Model;
 * names as in func_ov007_020b0da0, medium-confidence. */
struct Ov007Figure {
    char pad_00[4];
    Ov007Model *model; /* 0x04: func_ov007_020bc6d4 */
};

typedef char Ov007Model_size_must_be_0x4c[sizeof(Ov007Model) == 0x4c ? 1 : -1];

/* Poses the manager model and both figures against camera 0's view matrix,
 * scales the model by modelGrowth, then renders all three. */
extern "C" void func_ov007_020b2370(void) {
    Ov007Work *g = *(Ov007Work **)&data_ov007_0210342c;
    Ov007Model *f4 = g->model;
    Ov007Figure **arr = (Ov007Figure **)LAUNDER(&g->figure);
    func_ov007_020bc02c(f4, g->cameras[0]->viewMatrix);
    {
        Ov007Work *g2 = *(Ov007Work **)&data_ov007_0210342c;
        func_ov007_020be9ac(arr[0], g2->model, g2->cameras[0]->viewMatrix);
    }
    {
        Ov007Work *g2 = *(Ov007Work **)&data_ov007_0210342c;
        func_ov007_020be9ac(arr[1], g2->model, g2->cameras[0]->viewMatrix);
    }
    {
        Ov007Work *g2 = *(Ov007Work **)&data_ov007_0210342c;
        int t = g2->modelGrowth;
        long long v = ((long long)(0x1000 - t) << 8) + ((long long)t << 12);
        int res = (int)(v >> 12);
        f4->scale.z = res;
        f4->scale.y = f4->scale.z;
        f4->scale.x = f4->scale.y;
    }
    func_ov007_020bbff0(f4);
    func_ov007_020bbff0(arr[0]->model);
    func_ov007_020bbff0(arr[1]->model);
}
