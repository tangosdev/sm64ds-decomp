//cpp
// @symbol _ZN14QuestionSwitchD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Own vptr, then ModelAnim at 0x6b4, then the two MovingMeshCollider members
 * in reverse declaration order (0x4ec, then 0x324), then dBgActor_c's vptr
 * -- inlined, because dBgActor_c's destructor is defined in its class body
 * -- then dBgActor_c's own MeshCollider and Model, then dActor_c.
 */
#include "QuestionSwitch.h"

QuestionSwitch::~QuestionSwitch()
{
}
