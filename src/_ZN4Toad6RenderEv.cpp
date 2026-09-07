//cpp
// @symbol _ZN4Toad6RenderEv
/* recovered: named members + shared header, real C++ method
 *
 * One call. The `struct Model` stand-in this file used to declare was
 * mModelAnim: ModelAnim derives from Model, so `Render(const Vector3*)` is
 * reached through the base and the compiler applies no adjustment -- Model is
 * ModelAnim's FIRST base, at +0x00.
 *
 * The null argument is the scale, and passing 0 is what makes Toad render at
 * his own size rather than a caller-supplied one.

 * THE CALL IS QUALIFIED, AND THAT IS LOAD-BEARING. Model::Render is virtual
 * (slot 5 of _ZTV5Model) and ModelAnim overrides it, so a plain
 * `mModelAnim.Render(0)` emits the vtable dispatch -- `ldr r,[r]` /
 * `ldr r,[r,#0x14]` / `blx`, three words where the ROM has one `bl`.
 * `mModelAnim.Model::Render(0)` names the base explicitly, which suppresses
 * the dispatch and reproduces the ROM exactly. The shadow `struct Model` this
 * file used to carry hid the distinction by declaring Render non-virtual.
 */
#include "Toad.h"

int Toad::Render()
{
    mModelAnim.Model::Render(0);
    return 1;
}
