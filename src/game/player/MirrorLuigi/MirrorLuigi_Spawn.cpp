//cpp
#include "MirrorLuigi.h"
extern "C" {
extern void* _ZN7fBase_cnwEj(unsigned int sz);
extern void _ZN8dActor_cC2Ev(void* c);
extern void _ZN9ModelAnimC1Ev(void* c);
extern void _ZN5ModelC1Ev(void* c);
extern void _ZN11ShadowModelC1Ev(void* c);
extern void* func_020733a8(void* a, int b, int n, void* ctor, void* dtor);
extern int _ZTV11MirrorLuigi[];
extern void _ZN15TextureSequenceD1Ev(void*);
extern void _ZN15TextureSequenceC1Ev(void*);
MirrorLuigi* MirrorLuigi_Spawn(void){
  MirrorLuigi* c = (MirrorLuigi*)_ZN7fBase_cnwEj(sizeof(MirrorLuigi));
  if (c) {
    _ZN8dActor_cC2Ev(c);
    *(int**)c = _ZTV11MirrorLuigi;
    _ZN9ModelAnimC1Ev(&c->mModelAnim);
    _ZN5ModelC1Ev(&c->mModel);
    _ZN11ShadowModelC1Ev(&c->mShadowModel);
    func_020733a8(&c->mTextureSequences[0], 2, sizeof(TextureSequence),
                  (void*)_ZN15TextureSequenceC1Ev,
                  (void*)_ZN15TextureSequenceD1Ev);
  }
  return c;
}
}
