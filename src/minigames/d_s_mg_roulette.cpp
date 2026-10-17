//cpp
/* Mushroom Roulette scene factory (MG_ROULETTE), plus the empty array
 * element constructor. The scene's methods live in d_s_mg_roulette_methods.cpp.
 *
 * ROM: .text 0x0210a400..0x0210a4b0 — dScMgRoulette_c_classInit (the
 * registry factory) and Roulette_ConstructElement (the mArray element
 * constructor).
 *
 * leftovers:
 * - The factory still builds the object by hand: allocation, the base C2
 *   call and the vptr stores stay explicit because the ROM has no
 *   dScMgRoulette_c C1, so a native `new` expression cannot link.
 * - mModel1/mModel2 stay addressed off the mSlider base: naming them
 *   directly costs bytes (measured).
 * - The table constructor Dealer_Construct is still unnamed in
 *   symbols.txt; dScMgCard_c.h declares it.
 */

#include "dScMgRoulette_c.h"
#include "dScMgCard_c.h"

/* The constructors run on raw bytes of the new scene, so they are called by
 * their mangled names. Dealer_Construct (the mTable constructor) is still
 * unnamed in symbols.txt. */
extern "C" {
void *_ZN11dScMgBase_cC2Ev(void *scene);
extern "C" void _ZN8Particle10SysTrackerC1Ev(char* p);
extern "C" void __cxa_vec_ctor(void* p, int a, int b, void* d, void* e);
extern "C" void *_ZN5ModelC1Ev(char* p);
extern int _ZTV19dScMgSingle3DBase_c[];
extern int _ZTV15dScMgRoulette_c[];
extern "C" void Roulette_ConstructElement(void);
}

// @symbol Roulette_ConstructElement
/* Element constructor for mArray; the entries need no setup. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void Roulette_ConstructElement(void)
{
}
}

// @symbol dScMgRoulette_c_classInit
/* The name is reconstructed; the ROM proves the class, not the spelling.
 * Still raw: the header keeps both Models as raw bytes, addressed off the
 * mSlider member. Naming them directly costs bytes, so the offsets stay. */
extern "C" void* dScMgRoulette_c_classInit(void){
  dScMgRoulette_c *scene = (dScMgRoulette_c *)_ZN7fBase_cnwEj(sizeof(dScMgRoulette_c));
  if (scene) {
    char *raw = (char *)scene;
    _ZN11dScMgBase_cC2Ev(scene);
    *(int*)raw = (int)_ZTV19dScMgSingle3DBase_c;
    _ZN8Particle10SysTrackerC1Ev((char *)&scene->mSysTracker);
    *(int*)raw = (int)_ZTV15dScMgRoulette_c;
    Dealer_Construct((char *)scene->mTable);
    __cxa_vec_ctor(scene->mArray, 5, 0x34, Roulette_ConstructElement, Roulette_Nothing);
    char *models = (char *)scene->mSlider;
    _ZN5ModelC1Ev(models + 0x10);   /* mModel1 */
    _ZN5ModelC1Ev(models + 0x60);   /* mModel2 */
  }
  return scene;
}
