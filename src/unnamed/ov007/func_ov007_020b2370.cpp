//cpp
#include "types.h"
#include "Ov007Work.h"
#include "Ov007Camera.h"
#include "Ov007Model.h"
#include "Ov007Figure.h"

void func_ov007_020bc02c(Ov007Model *a, void *b);
void func_ov007_020be9ac(Ov007Figure *a, Ov007Model *b, void *c);
void func_ov007_020bbff0(Ov007Model *c);

/* Poses the manager model and both figures against camera 0's view matrix,
 * scales the model by modelGrowth, then renders all three. */
void func_ov007_020b2370(void) {
    Ov007Work *g = WORK;
    Ov007Model *model = g->model;
    Ov007Figure **figures = &g->figure; /* figure, altFigure */
    func_ov007_020bc02c(model, &g->cameras[0]->viewMatrix);
    func_ov007_020be9ac(figures[0], WORK->model, &WORK->cameras[0]->viewMatrix);
    func_ov007_020be9ac(figures[1], WORK->model, &WORK->cameras[0]->viewMatrix);
    {
        int t = WORK->modelGrowth;
        long long v = ((long long)(0x1000 - t) << 8) + ((long long)t << 12);
        int res = (int)(v >> 12);
        model->scale.z = res;
        model->scale.y = model->scale.z;
        model->scale.x = model->scale.y;
    }
    func_ov007_020bbff0(model);
    func_ov007_020bbff0(figures[0]->model);
    func_ov007_020bbff0(figures[1]->model);
}
