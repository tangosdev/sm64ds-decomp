#include "Ov007Work.h"

extern char* data_ov007_0210342c;

/* The 0x1c-byte animation player func_ov007_020c92ac allocates and
 * func_ov007_020c9110 ticks; names as in func_ov007_020bc02c, only the slot
 * this file writes. */
typedef struct Ov007AnimPlayer {
    /* 0x00: 0 playing; 1 held (func_ov007_020c9110 leaves the frame alone);
     * func_ov007_020c9110 sets 2 when a one-shot ends */
    int state;
} Ov007AnimPlayer;

/* The 0xa0-byte model record func_ov007_020bc6d4 allocates; names as in
 * func_ov007_020bc6d4, only the slots this file touches. */
typedef struct Ov007Model {
    char pad_00[0x18];
    Ov007AnimPlayer *boneAnim; /* 0x18 */
    char pad_1c[8];
    Ov007AnimPlayer *texSrtAnim; /* 0x24 */
    char pad_28[8];
    Ov007AnimPlayer *texSeqAnim; /* 0x30 */
} Ov007Model;

/* The 0x1c-byte record func_ov007_020beaa0 builds around an Ov007Model;
 * names as in func_ov007_020b0da0, medium-confidence. */
typedef struct Ov007Figure {
    char pad_00[4];
    Ov007Model *model; /* 0x04: func_ov007_020bc6d4 */
} Ov007Figure;

typedef char Ov007Model_size_must_be_0x34[sizeof(Ov007Model) == 0x34 ? 1 : -1];

#define WORK ((Ov007Work*)data_ov007_0210342c)

/* Holds the manager model's three animation players where they are playing,
 * and stops both figures' bone animations (state 2). */
void func_ov007_020b257c(void) {
    int* p;
    p = &WORK->model->boneAnim->state;
    if (*p == 0) *p = 1;
    p = &WORK->model->texSrtAnim->state;
    if (*p == 0) *p = 1;
    p = &WORK->model->texSeqAnim->state;
    if (*p == 0) *p = 1;
    WORK->figure->model->boneAnim->state = 2;
    WORK->altFigure->model->boneAnim->state = 2;
}
