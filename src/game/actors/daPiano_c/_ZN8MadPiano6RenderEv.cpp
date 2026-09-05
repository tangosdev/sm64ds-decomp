//cpp
// @symbol _ZN8MadPiano6RenderEv
/* ModelAnim's real virtual API expresses the same slot-5 dispatch as the ROM. */
#include "MadPiano.h"

int MadPiano::Render()
{
    mModelAnim.Render(0);
    return 1;
}
