//cpp
// @symbol _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c
/* recovered: real C++ method
 *
 * Two forwarding calls on the ray: set the probe position, then the owning
 * actor. Both helpers are still unnamed cartridge functions, so they stay
 * extern "C" declarations here rather than coming from decl_common.h -- that
 * header spells the first one with a non-const Vector3 *.
 */
#include "dBgCh_Gnd.h"

extern "C" void func_020374d4(void *ray, const Vector3 *pos);
extern "C" void func_020353b0(void *ray, dActor_c *actor);

void dBgCh_Gnd::SetObjAndPos(const Vector3 &vec_, dActor_c *actor_)
{
    func_020374d4(this, &vec_);
    func_020353b0(this, actor_);
}
