#include "Ov007Camera.h"
#include "Ov007MenuMem.h"

extern Ov007VecTween *func_ov007_020c8f64(Vector3 *out);
extern Ov007Tween *func_ov007_020c908c(s32 *out);
extern void func_ov007_020c0128(Ov007Camera *cam);

/* Allocates a camera, binds each of its three tweens to the field it drives
 * and resets it (func_ov007_020c0128). */
Ov007Camera *func_ov007_020c023c(void) {
    Ov007Camera *cam = func_ov007_020c3df4(0, sizeof(Ov007Camera));
    cam->eyeTween = func_ov007_020c8f64(&cam->eye);
    cam->targetTween = func_ov007_020c8f64(&cam->target);
    cam->distanceTween = func_ov007_020c908c(&cam->distance);
    func_ov007_020c0128(cam);
    return cam;
}
