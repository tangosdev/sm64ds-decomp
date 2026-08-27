//cpp
// @symbol _ZN16daObjCtMecha04_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daObjCtMecha04_c.h"

int daObjCtMecha04_c::Render()
{
    mTextureTransformer.Update(mModel.data);
    mModel.Render(0);
    return 1;
}
