//cpp
/* Mushroom Roulette scene factory (MG_ROULETTE), plus the racer's empty
 * constructor. The scene's methods live in d_s_mg_roulette_methods.cpp.
 *
 * ROM: .text 0x0210a400..0x0210a4b0 -- dScMgRoulette_c_classInit (the
 * registry factory) and RouletteRacer::RouletteRacer (the constructor the
 * factory hands __cxa_vec_ctor for mRacers).
 */

#include "dScMgRoulette_c.h"

// @symbol _ZN13RouletteRacerC1Ev
/* The racers need no setup; the constructor is out of line only because
   __cxa_vec_ctor takes its address. */
RouletteRacer::RouletteRacer() {}

// @symbol dScMgRoulette_c_classInit
/* The name is reconstructed; the ROM proves the class, not the spelling.
 * `new` inlines the implicit constructor: the base chain, this class's
 * vptr, then mDealer, mRacers and mBoard's two models in declaration order.
 * Historical alias: MgMushroomRoulette_Spawn. */
extern "C" void *dScMgRoulette_c_classInit(void) {
    return new dScMgRoulette_c;
}
