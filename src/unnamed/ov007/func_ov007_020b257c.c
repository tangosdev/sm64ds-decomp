#include "Ov007Work.h"
#include "Ov007Model.h"
#include "Ov007Figure.h"

/* Holds the manager model's three animation players where they are playing,
 * and stops both figures' bone animations (state 2). */
void func_ov007_020b257c(void) {
    int* p;
    p = &gMenuWork->model->boneAnim->state;
    if (*p == 0) *p = 1;
    p = &gMenuWork->model->texSrtAnim->state;
    if (*p == 0) *p = 1;
    p = &gMenuWork->model->texSeqAnim->state;
    if (*p == 0) *p = 1;
    gMenuWork->figure->model->boneAnim->state = 2;
    gMenuWork->altFigure->model->boneAnim->state = 2;
}
