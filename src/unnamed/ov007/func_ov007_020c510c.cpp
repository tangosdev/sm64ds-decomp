//cpp
#include "Ov007Picker.h"
#include "Ov007Camera.h"

extern "C" void func_ov007_020bfe4c(Ov007Camera *o, int x, int y, int z, VecFx32 *out);

/* Unprojects the camera's screen rectangle at the camera distance into the
   picker's clamp rectangle; screen y runs down, so the two y bounds swap. */
void Ov007Picker::func_ov007_020c510c() {
    int neg;
    Ov007Camera *o = camera;
    neg = -o->distance;
    func_ov007_020bfe4c(o, o->viewportMinX << 12, o->viewportMinY << 12, neg, &boundsMin);
    func_ov007_020bfe4c(o, o->viewportMaxX << 12, o->viewportMaxY << 12, neg, &boundsMax);
    {
        int t = boundsMin.y;
        boundsMin.y = boundsMax.y;
        boundsMax.y = t;
    }
}
