//cpp
#include "UnknownVsEntry.h"

/* The array runtime passes each element address and ignores lifecycle results.
 * Explicit function-pointer casts mark that runtime ABI boundary. */
extern "C" {
extern void* _ZN7fBase_cnwEj(unsigned int sz);
extern void *_ZN7fBase_cC2Ev(void* p);
extern void _ZN8Particle10SysTrackerC1Ev(void* p);
extern void *_ZN5ModelC1Ev(void* p);
extern void *_ZN9ModelAnimC1Ev(void* p);
extern void __cxa_vec_ctor(void *base, unsigned int count, unsigned int stride,
                           void (*ctor)(void *), void (*dtor)(void *));
extern void* data_0208e4b8;
extern void* _ZTV14UnknownVsEntry;
extern UnknownVsPlayer *_ZN15UnknownVsPlayerD1Ev(UnknownVsPlayer *object);
extern UnknownVsPlayer *_ZN15UnknownVsPlayerC1Ev(UnknownVsPlayer *object);
}
/* Reconstructed source-style name: SM64DS proves dEntObj_c through RTTI,
 * allocation size, vtable identity, and the ENTRY_OBJECT registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: UnknownVsEntry_Spawn. */
extern "C" UnknownVsEntry* dEntObj_c_classInit(void);
extern "C" UnknownVsEntry* dEntObj_c_classInit(void){
  UnknownVsEntry* p = (UnknownVsEntry*)_ZN7fBase_cnwEj(sizeof(UnknownVsEntry));
  if (p) {
    _ZN7fBase_cC2Ev(p);
    *(void**)p = &data_0208e4b8;
    *(void**)p = &_ZTV14UnknownVsEntry;
    _ZN8Particle10SysTrackerC1Ev(&p->mParticles);
    _ZN5ModelC1Ev(&p->mModel);
    _ZN9ModelAnimC1Ev(&p->mModelAnim);
    __cxa_vec_ctor(p->mPlayers, 4, sizeof(UnknownVsPlayer),
                  (void (*)(void *))_ZN15UnknownVsPlayerC1Ev,
                  (void (*)(void *))_ZN15UnknownVsPlayerD1Ev);
  }
  return p;
}
