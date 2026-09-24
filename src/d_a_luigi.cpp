//cpp
#include "daLuigi_c.h"
extern "C" {
extern void* _ZN7fBase_cnwEj(unsigned int sz);
extern void _ZN8dActor_cC2Ev(void* c);
extern void _ZN9ModelAnimC1Ev(void* c);
extern void _ZN5ModelC1Ev(void* c);
extern void _ZN11ShadowModelC1Ev(void* c);
extern void* __cxa_vec_ctor(void* a, int b, int n, void* ctor, void* dtor);
extern int _ZTV9daLuigi_c[];
extern void _ZN15TextureSequenceD1Ev(void*);
extern void _ZN15TextureSequenceC1Ev(void*);
/* Reconstructed source-style name: SM64DS proves daLuigi_c through RTTI,
 * allocation size, vtable identity, and the LUIGI registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MirrorLuigi_Spawn. */
daLuigi_c* daLuigi_c_classInit(void){
  daLuigi_c* c = (daLuigi_c*)_ZN7fBase_cnwEj(sizeof(daLuigi_c));
  if (c) {
    _ZN8dActor_cC2Ev(c);
    *(int**)c = _ZTV9daLuigi_c;
    _ZN9ModelAnimC1Ev(&c->mModelAnim);
    _ZN5ModelC1Ev(&c->mModel);
    _ZN11ShadowModelC1Ev(&c->mShadowModel);
    __cxa_vec_ctor(&c->mTextureSequences[0], 2, sizeof(TextureSequence),
                  (void*)_ZN15TextureSequenceC1Ev,
                  (void*)_ZN15TextureSequenceD1Ev);
  }
  return c;
}
}
