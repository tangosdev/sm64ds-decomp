#include "dExtModel_c.h"
#include "Ov007Model.h"

extern void *func_ov007_020c92ac(void);
extern void func_02046d50(struct BMD_File *model, struct BTP_File *file);

/* Binds the model's texture-sequence animations: n BTP files, a fresh
 * func_ov007_020c92ac player, and each nonzero file prepared against the
 * model's BMD file. */
void func_ov007_020bc5fc(Ov007Model *m, struct BTP_File **files, int n) {
    int i;

    m->numTexSeqAnims = n;
    m->texSeqAnims = files;
    m->texSeqAnim = func_ov007_020c92ac();
    for (i = 0; i < n; i++) {
        if (m->texSeqAnims[i])
            func_02046d50(m->model->modelFile, m->texSeqAnims[i]);
    }
}
