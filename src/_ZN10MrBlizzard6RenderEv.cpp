//cpp
// @symbol _ZN10MrBlizzard6RenderEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Model.h"
/* recovered: named members + shared header, real C++ method */
#include "MrBlizzard.h"
extern "C" {
extern void _ZN5Model6RenderEPK7Vector3(void*,void*);
}

int MrBlizzard::Render()
{
  int s=mType;
  if(s==3) return 1;
  if(s==2){
    if(unk_400)
      _ZN5Model12HideMaterialEii(((char *)this)+0x30c,0,2);
    else
      _ZN5Model12ShowMaterialEii(((char *)this)+0x30c,0,2);
  }
  _ZN5Model6RenderEPK7Vector3(((char *)this)+0x30c, ((char *)this)+0x80);
  return 1;
}
