#include "Ov007Work.h"
#include "Ov007Model.h"
#include "Ov007Figure.h"

/* Holds the manager model's three animation players where they are playing,
 * and stops both figures' bone animations (state 2). */
void func_ov007_020b257c(void) {
    int* p;
    p = &sMenuWork->model->boneAnim->state;
    if (*p == 0) *p = 1;
    p = &sMenuWork->model->texSrtAnim->state;
    if (*p == 0) *p = 1;
    p = &sMenuWork->model->texSeqAnim->state;
    if (*p == 0) *p = 1;
    sMenuWork->figure->model->boneAnim->state = 2;
    sMenuWork->altFigure->model->boneAnim->state = 2;
}
