#include "types.h"

/* The particle's 0x40 halfword (SPL's visibility): the base alpha in bits 0-4
 * (func_0204c584 seeds it), the animated alpha in bits 5-9. This
 * callback rewrites the halfword whole, so it keeps this view rather than
 * spl_particle_views.h's ParticleElement, which spells it as bitfields. */
struct ParticleAlphaView {
    char pad0[0x40];
    u16 visibility; /* 0x40 */
};

/* The child particles' alpha callback (pret's SPL: SPLAnim_ChildAlpha): the
 * animated alpha fades from 31 at birth to 0 at lifeRate 255. */
void func_0204d0b8(struct ParticleAlphaView *ptcl, void *def, int lifeRate) {
    u16 *visibility = &ptcl->visibility;
    u16 alpha = ((0xff - lifeRate) * 0x1f) / 0xff;
    *visibility = (*visibility & ~0x3e0) | ((alpha & 0x1f) << 5);
}
