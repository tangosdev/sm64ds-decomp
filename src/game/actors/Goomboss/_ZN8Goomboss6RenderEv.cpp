//cpp
// @symbol _ZN8Goomboss6RenderEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Goomboss.h"
extern "C" {
extern void _ZN15TextureSequence6UpdateER15ModelComponents(void* a, void* b);
extern void _ZN15MaterialChanger6UpdateER15ModelComponents(void* a, void* b);
extern void _ZN18TextureTransformer6UpdateER15ModelComponents(void* a, void* b);
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
  _ZN15TextureSequence6UpdateER15ModelComponents(&mTextureSequence, &mModelAnim.data);
  _ZN15MaterialChanger6UpdateER15ModelComponents(&mMaterialChanger, &mModelAnim.data);
  _ZN18TextureTransformer6UpdateER15ModelComponents(&mTextureTransformer, &mModelAnim.data);
  return 1;
}
