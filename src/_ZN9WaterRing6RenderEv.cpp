//cpp
// @symbol _ZN9WaterRing6RenderEv
/* recovered: named members + shared header, real C++ method
 *
 * Two calls, and naming the members identifies both. The `Sub` stand-in this
 * file used to carry -- six pure virtuals, called through slot 5 -- was
 * Model's vtable: `Model::Render(const Vector3 *scale)` IS slot 5 in
 * _ZTV5Model, so the dispatch is now spelt as the member call it is.
 *
 * The second argument of the Update call, `this + 0x314`, is mModel's own
 * ModelComponents: mModel sits at 0x30c and Model::data at +0x08. So the
 * texture transformer is handed the components of the model it animates,
 * not a separate object.
 *
 * mTextureTransformer is still a u8 marker in the header, because a real
 * TextureTransformer is 0x14 bytes and the fields the header records at
 * 0x368 and 0x36c fall inside that span -- adopting the type is a header
 * reconstruction of its own, so the cast stays.
 */
#include "WaterRing.h"
#include "TextureTransformer.h"

int WaterRing::Render()
{
    ((TextureTransformer *)&mTextureTransformer)->Update(mModel.data);
    mModel.Render((const Vector3 *)&unk_080);
    return 1;
}
