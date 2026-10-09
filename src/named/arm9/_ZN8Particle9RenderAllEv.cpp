//cpp
// @symbol _ZN8Particle9RenderAllEv
#include "Particle__SysTracker.h"

extern "C" {
void func_02049ee8(void* manager, void* renderState);
extern Particle__SysTracker* data_0209ee74;
extern int data_0209b3ec;
}

namespace Particle {

void RenderAll()
{
    Particle__SysTracker* tracker = data_0209ee74;
    if (!tracker)
        return;

    func_02049ee8(tracker->mManager, &data_0209b3ec);
}

}
