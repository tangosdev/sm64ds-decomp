//cpp
/* Mushroom Roulette scene factory (MG_ROULETTE), plus the empty array
 * element constructor. The scene's methods live in dScMgRoulette_c.cpp.
 *
 * The base C2 receives the allocated scene in r0. Allocation and the
 * vptr stores remain written out by hand; reconstructing a native scene
 * constructor and a `new` expression remains separate matching work.
 */

#include "dScMgRoulette_c.h"
#include "dScMgCard_c.h"

/* The constructors run on raw bytes of the new scene, so they are called by
 * their mangled names. func_ov006_020c1d80 (the mTable constructor) is still
 * unnamed in symbols.txt. */
extern "C" {
extern "C" void *_ZN11dScMgBase_cC2Ev(void *scene);
extern "C" void _ZN8Particle10SysTrackerC1Ev(char* p);
extern "C" void __cxa_vec_ctor(void* p, int a, int b, void* d, void* e);
extern "C" void *_ZN5ModelC1Ev(char* p);
extern int _ZTV19dScMgSingle3DBase_c[];
extern int _ZTV15dScMgRoulette_c[];
extern "C" void func_ov006_0210a4ac(void);
}

// @symbol func_ov006_0210a4ac
/* Element constructor for mArray; the entries need no setup. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210a4ac(void)
{
}
}

// @symbol dScMgRoulette_c_classInit
/* The name is reconstructed; the ROM proves the class, not the spelling.
 * Still raw: 0x471c is a Particle::SysTracker with no header field, and the
 * header keeps both Models as raw bytes. Addressing them as members costs
 * bytes, so they stay offsets from a base at 0x530c. */
extern "C" void* dScMgRoulette_c_classInit(void){
  dScMgRoulette_c *scene = (dScMgRoulette_c *)_ZN7fBase_cnwEj(0x5400);
  if (scene) {
    char *raw = (char *)scene;
    _ZN11dScMgBase_cC2Ev(scene);
    *(int*)raw = (int)_ZTV19dScMgSingle3DBase_c;
    _ZN8Particle10SysTrackerC1Ev(raw + 0x471c);
    *(int*)raw = (int)_ZTV15dScMgRoulette_c;
    func_ov006_020c1d80((char *)scene->mTable);
    __cxa_vec_ctor(scene->mArray, 5, 0x34, func_ov006_0210a4ac, func_ov006_021079c8);
    char *models = raw + 0x530c;
    _ZN5ModelC1Ev(models + 0x10);   /* mModel1 */
    _ZN5ModelC1Ev(models + 0x60);   /* mModel2 */
  }
  return scene;
}
