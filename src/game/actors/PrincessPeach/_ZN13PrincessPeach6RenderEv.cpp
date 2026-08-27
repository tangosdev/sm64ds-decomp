//cpp
// @symbol _ZN13PrincessPeach6RenderEv
/* recovered: named members + shared header, real C++ method
 *
 * The hand-spelt _ZN5Model6RenderEPK7Vector3 on `this + 0xd4` is a member call
 * on mModelAnim, whose Model base sits at +0x00 -- so the compiler emits the
 * same instruction without the cast or the extern.

 * THE CALL IS QUALIFIED, AND THAT IS LOAD-BEARING. Model::Render is virtual
 * (slot 5 of _ZTV5Model) and ModelAnim overrides it, so a plain
 * `mModelAnim.Render(0)` emits the vtable dispatch -- `ldr r,[r]` /
 * `ldr r,[r,#0x14]` / `blx`, three words where the ROM has one `bl`.
 * `mModelAnim.Model::Render(0)` names the base explicitly, which suppresses
 * the dispatch and reproduces the ROM exactly. The shadow `struct Model` this
 * file used to carry hid the distinction by declaring Render non-virtual.
 */
#include "PrincessPeach.h"

int PrincessPeach::Render()
{
    mModelAnim.Model::Render(0);
    return 1;
}
