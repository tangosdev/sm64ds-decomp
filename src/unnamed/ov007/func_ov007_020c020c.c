#include "Ov007Camera.h"
#include "Ov007MenuMem.h"

/* The free veneers that sit just ahead of each tween's allocator; both
 * reach the free hook func_ov007_020c3d1c with the pointer still in r0. */
extern void func_ov007_020c8f58(void *p);
extern void func_ov007_020c9080(void *p);

/* Frees a camera and its three tweens. */
void func_ov007_020c020c(Ov007Camera *cam) {
    func_ov007_020c8f58(cam->eyeTween);
    func_ov007_020c8f58(cam->targetTween);
    func_ov007_020c9080(cam->distanceTween);
    func_ov007_020c3d1c(cam);
}
