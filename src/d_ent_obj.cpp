//cpp
#include "UnknownVsEntry.h"

extern "C" {
extern void* _ZN7fBase_cnwEj(unsigned int sz);
extern void _ZN7fBase_cC2Ev(void* p);
extern void _ZN8Particle10SysTrackerC1Ev(void* p);
extern void _ZN5ModelC1Ev(void* p);
extern void _ZN9ModelAnimC1Ev(void* p);
extern void func_020733a8(void* a, int b, int c, void* d, void* e);
extern void* data_0208e4b8;
extern void* _ZTV14UnknownVsEntry;
extern void _ZN15UnknownVsPlayerD1Ev(void);
extern void _ZN15UnknownVsPlayerC1Ev(void);
}
extern "C" UnknownVsEntry* UnknownVsEntry_Spawn(void);
extern "C" UnknownVsEntry* UnknownVsEntry_Spawn(void){
  UnknownVsEntry* p = (UnknownVsEntry*)_ZN7fBase_cnwEj(sizeof(UnknownVsEntry));
  if (p) {
    _ZN7fBase_cC2Ev(p);
    *(void**)p = &data_0208e4b8;
    *(void**)p = &_ZTV14UnknownVsEntry;
    _ZN8Particle10SysTrackerC1Ev(&p->mParticles);
    _ZN5ModelC1Ev(&p->mModel);
    _ZN9ModelAnimC1Ev(&p->mModelAnim);
    func_020733a8(p->mPlayers, 4, sizeof(UnknownVsPlayer),
                  (void*)_ZN15UnknownVsPlayerC1Ev,
                  (void*)_ZN15UnknownVsPlayerD1Ev);
  }
  return p;
}
