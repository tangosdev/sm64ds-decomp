// @symbol _ZN8Particle9RenderAllEv
#include "Particle.h"
extern void func_02049ee8(void* manager, void* b);
extern struct Particle* data_0209ee74;
extern int data_0209b3ec;

void _ZN8Particle9RenderAllEv(void)
{
    struct Particle* tracker = data_0209ee74;
    if (tracker == 0) return;
    func_02049ee8(tracker->mManager, &data_0209b3ec);
}
