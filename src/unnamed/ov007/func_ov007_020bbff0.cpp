//cpp
#include "dExtModel_c.h"
#include "Ov007Model.h"



/* Renders the model with its matrix and scale while it is visible. */
void func_ov007_020bbff0(Ov007Model *m) {
    if (m->visible == 0)
        return;
    m->model->Render(&m->matrix, &m->scale);
}

