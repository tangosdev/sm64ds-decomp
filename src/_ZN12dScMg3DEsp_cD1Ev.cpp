//cpp
// @symbol _ZN12dScMg3DEsp_cD1Ev
/* dScMg3DEsp_c's destructor; the build keeps D1 from this file. The members
   are raw storage (see dScMg3DEsp_c.h), so each is destroyed by an explicit
   call, in reverse declaration order. The vtable stores, mSysTracker's
   destruction and the chain to ~dScMgBase_c are compiler-generated. */
#include "dScMg3DEsp_c.h"
#include "dMg3DEspModel_c.h"
#include "TextureTransformer.h"
dScMg3DEsp_c::~dScMg3DEsp_c()
{
    ((TextureTransformer *)mTextureTransformer)->TextureTransformer::~TextureTransformer();
    ((dMg3DEspModel_c *)pad_4fd8)->~dMg3DEspModel_c();
    ((Model *)mModel2)->Model::~Model();
    ((Model *)mModel1)->Model::~Model();
}
