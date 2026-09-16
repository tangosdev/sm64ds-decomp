//cpp
#include "TextureTransformer.h"
#include "TextureSequence.h"
#include "MaterialChanger.h"
// @symbol _ZN8Goomboss6RenderEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Goomboss.h"
extern "C" {
}
struct Sub {
  virtual void v0(); virtual void v1(); virtual void v2();
  virtual void v3(); virtual void v4(); virtual void m(void*);
};

int Goomboss::Render()
{
  if(param1==0x1111) return func_ov074_021222e0(((char*)this));
  if(mShouldRender==0) return 1;
  Sub* s = (Sub*)((char*)&mModelAnim);
  s->m((char*)&mScaleX);
  mTextureSequence.Update(mModelAnim.data);
  mMaterialChanger.Update(mModelAnim.data);
  mTextureTransformer.Update(mModelAnim.data);
  return 1;
}
