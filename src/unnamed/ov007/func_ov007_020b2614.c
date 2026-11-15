#include "Ov007Work.h"
#include "Ov007Model.h"

extern void func_ov007_020bc374(Ov007Model *m, int i, int startFrame, int fadeFrames);
extern void func_ov007_020bc3dc(Ov007Model *m, int i, int startFrame);
extern void func_ov007_020bc2e4(Ov007Model *m, int i, int startFrame);
extern void func_ov007_020bc32c(Ov007Model *m, int i, int startFrame);
extern void _ZN11Ov007Figure19func_ov007_020be980Eii(struct Ov007Figure *fig, int anim, int a2);
extern int _ZN11Ov007Figure19func_ov007_020be964Ev(struct Ov007Figure *fig);

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
        _ZN11Ov007Figure19func_ov007_020be980Eii(WORK->figure, 0, startFrame);
        _ZN11Ov007Figure19func_ov007_020be980Eii(WORK->altFigure, 0, startFrame);
    } else {
        _ZN11Ov007Figure19func_ov007_020be964Ev(WORK->figure);
        _ZN11Ov007Figure19func_ov007_020be964Ev(WORK->altFigure);
    }
}
