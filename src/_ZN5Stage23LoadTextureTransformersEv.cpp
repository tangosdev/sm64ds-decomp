//cpp
#include "types.h"
// @symbol _ZN5Stage23LoadTextureTransformersEv
/* recovered: named members + shared header, real C++ method */
#include "Stage.h"
struct Entry { int x; void* bta; char pad[4]; };
struct Info { char a[0x10]; struct Entry* entries; unsigned char count; };
extern struct Info* data_0209f340;
extern "C" {
extern void* _Znwj(u32);
extern void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(void* bmd, void* bta);
extern void* _ZN18TextureTransformerC1Ev(void*);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void* self, void* bta, int a, int b, u32 c);
}

void Stage::LoadTextureTransformers()
{
  struct Entry* e = data_0209f340->entries;
  void** slot = (void**)((char*)pad_8bc);
  void* bmd = *(void**)((char*)&mModel.data);
  int i;
  for(i=0;i<(int)data_0209f340->count;i++){
    if(e->bta){
      _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(bmd, e->bta);
      void* t = _Znwj(0x14);
      if(t) t = _ZN18TextureTransformerC1Ev(t);
      *slot = t;
      _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(*slot, e->bta, 0, 0x1000, 0);
    }
    e++;
    slot = (void**)((char*)slot + 0xc);
  }
}
