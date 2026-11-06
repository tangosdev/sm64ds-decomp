#ifndef PARTICLE_IDS_H
#define PARTICLE_IDS_H

/* Effect IDs: indices into the SPA particle resource that arm9 embeds
 * (data_02075f14, 321 definitions).  An ID is only named here once ROM
 * evidence ties it to what it draws; the rest stay bare hex at their call
 * sites.  tools/spa_dump.py prints a definition and
 * tools/particle_id_sites.py lists every literal that spawns it.
 *
 * "Emitter" below is the table's spawn shape (where particles start), not how
 * the effect looks; the poofs in particular read as soft clouds on screen.
 *
 * Evidence for the six below: all share texture 40 and are billboards.
 *  - kPtclLandingDust: emitter on a circle rim, radius 2.0, 25 frames.  dActor_c::HugeLandingDustAt
 *    spawns it, and several actors spawn it directly.
 *  - kPtclGroundPuff: emitter on a circle rim, radius 1.0, 18 frames.  func_0200fc44 spawns it 25.0
 *    above a (raycast) ground point.  Seen at runtime on Mario's landing
 *    (daPly_c::St_Land_Init); the hurt, thrown and punch/kick states reach it
 *    through the same helper, all skipped in shallow water.
 *  - kPtclFlameOutPuff: emitter on a circle rim, radius 0.5, 35 frames, base colour dark grey on
 *    the same puff texture 40.  Spawned where a flame ends: daKpaFire_c's
 *    variants 6/7 and daObjFire_c when the flame is being eaten.  The name
 *    states the context; "smoke" is an inference from the colour.
 *  - kPtclTriplePoof*: dActor_c::TriplePoofDustAt spawns the three in order.
 *    Their letter suffixes only record that order. */
enum ParticleId {
    kPtclFlameOutPuff = 0x9f,
    kPtclLandingDust = 0xb2,
    kPtclGroundPuff = 0xb3,
    kPtclTriplePoofA = 0x124,
    kPtclTriplePoofB = 0x125,
    kPtclTriplePoofC = 0x126
};

#endif
