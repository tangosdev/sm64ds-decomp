//cpp
#include "Ov007Picker.h"
#include "Ov007Camera.h"

extern "C" void func_ov007_020bfe4c(Ov007Camera *o, int x, int y, int z, Vector3 *out);

/* Unprojects the camera's screen rectangle at the camera distance into the
   picker's clamp rectangle; screen y runs down, so the two y bounds swap. */
void Ov007Picker::func_ov007_020c510c() {
    Ov007Picker *p = this;
    int neg;
    Ov007Camera *o = p->camera;
    neg = -o->distance;
    func_ov007_020bfe4c(o, o->viewportMinX << 0xc, o->viewportMinY << 0xc, neg, &p->boundsMin);
    func_ov007_020bfe4c(o, o->viewportMaxX << 0xc, o->viewportMaxY << 0xc, neg, &p->boundsMax);
    {
        int t = p->boundsMin.y;
        p->boundsMin.y = p->boundsMax.y;
        p->boundsMax.y = t;
    }
}
