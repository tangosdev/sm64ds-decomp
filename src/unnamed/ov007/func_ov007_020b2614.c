#include "Ov007Work.h"
#include "Ov007Model.h"

extern void func_ov007_020bc374(Ov007Model *m, int i, int startFrame, int fadeFrames);
extern void func_ov007_020bc3dc(Ov007Model *m, int i, int startFrame);
extern void func_ov007_020bc2e4(Ov007Model *m, int i, int startFrame);
extern void func_ov007_020bc32c(Ov007Model *m, int i, int startFrame);
#define func_ov007_020be980 _Z19func_ov007_020be980P11Ov007Figureii
#define func_ov007_020be964 _Z19func_ov007_020be964P11Ov007Figure
extern void func_ov007_020be980(struct Ov007Figure *fig, int anim, int a2);
extern int func_ov007_020be964(struct Ov007Figure *fig);

/* Starts animation `anim` of the menu model at startFrame: the bone
 * animation, cross-faded over fadeFrames when that is positive and
 * otherwise started outright with the model shown, then the
 * texture-pattern and texture-SRT animations. Animation 0 also starts
 * animation 0 on both figures; any other holds theirs
 * (func_ov007_020be964). */
void func_ov007_020b2614(int anim, int startFrame, int fadeFrames) {
    if (fadeFrames > 0) {
        func_ov007_020bc374(WORK->model, anim, startFrame, fadeFrames);
    } else {
        func_ov007_020bc3dc(WORK->model, anim, startFrame);
        WORK->model->visible = 1;
        WORK->model->animTick = 1;
    }
    func_ov007_020bc2e4(WORK->model, anim, startFrame);
    func_ov007_020bc32c(WORK->model, anim, startFrame);
    if (anim == 0) {
        func_ov007_020be980(WORK->figure, 0, startFrame);
        func_ov007_020be980(WORK->altFigure, 0, startFrame);
    } else {
        func_ov007_020be964(WORK->figure);
        func_ov007_020be964(WORK->altFigure);
    }
}
