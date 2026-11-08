#include "private/spl_particle_views.h"

/* The child particles' scale callback: the scale runs from 1.0 at birth to
 * child->endScale at lifeRate 255. The name is pret's SPL coinage, not a ROM
 * or SDK symbol. */
void SPLAnim_ChildScale(ParticleElement *ptcl, PtclDef *def, int lifeRate) {
    int endScale;
    int delta;
    endScale = def->child->endScale;
    delta = (endScale - 0x1000) * (lifeRate - 0xff) / 255;
    ptcl->animScale = endScale + delta;
}
