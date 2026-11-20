//cpp
#include "Ov007Work.h"
#include "Ov007Model.h"
#include "Ov007Figure.h"

extern "C" void func_ov007_020bc374(Ov007Model *m, int i, int startFrame, int fadeFrames);
extern "C" void func_ov007_020bc3dc(Ov007Model *m, int i, int startFrame);
extern "C" void func_ov007_020bc2e4(Ov007Model *m, int i, int startFrame);
extern "C" void func_ov007_020bc32c(Ov007Model *m, int i, int startFrame);

/* Starts animation `anim` of the menu model at startFrame: the bone
 * animation, cross-faded over fadeFrames when that is positive and
 * otherwise started outright with the model shown, then the
 * texture-pattern and texture-SRT animations. Animation 0 also starts
 * animation 0 on both figures; any other holds theirs
 * (func_ov007_020be964). */
extern "C" void func_ov007_020b2614(int anim, int startFrame, int fadeFrames) {
    if (fadeFrames > 0) {
        func_ov007_020bc374(gMenuWork->model, anim, startFrame, fadeFrames);
    } else {
        func_ov007_020bc3dc(gMenuWork->model, anim, startFrame);
        gMenuWork->model->visible = 1;
        gMenuWork->model->animTick = 1;
    }
    func_ov007_020bc2e4(gMenuWork->model, anim, startFrame);
    func_ov007_020bc32c(gMenuWork->model, anim, startFrame);
    if (anim == 0) {
        gMenuWork->figure->func_ov007_020be980(0, startFrame);
        gMenuWork->altFigure->func_ov007_020be980(0, startFrame);
    } else {
        gMenuWork->figure->func_ov007_020be964();
        gMenuWork->altFigure->func_ov007_020be964();
    }
}
