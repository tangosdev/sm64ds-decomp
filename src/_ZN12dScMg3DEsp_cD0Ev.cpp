//cpp
// @symbol _ZN12dScMg3DEsp_cD0Ev
/* dScMg3DEsp_c's destructor; the build keeps D0 from this file. The
   definition is the same as in the D1 file. */
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
