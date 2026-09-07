//cpp
// @symbol _ZN11RickshawBdw13InitResourcesEv
#include "RickshawBdw.h"

/* RickshawBdw::InitResources -- vtable slot 0, ov043 0x021114c4.
 *
 * A tail call into daObjKurumajiku_c's shared cross-overlay file-load
 * helper func_ov002_020b6c54(this, filetable, actorID) -- out of this
 * task's scope, kept under its existing name, same cross-overlay-veneer
 * idiom as the daObjKaitendai_c leaves in ov015/ov022/ov036. */
struct Arg { void *m[3]; };
extern "C" {
extern int func_ov002_020b6c54(void *self, Arg *arg, unsigned int actorID);
extern Arg data_ov043_02112344;
}

int RickshawBdw::InitResources()
{
    func_ov002_020b6c54(this, &data_ov043_02112344, 0x88u);
}
