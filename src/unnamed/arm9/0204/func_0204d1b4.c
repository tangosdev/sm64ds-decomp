#include "types.h"
#include "private/spl_particle_views.h"
extern int __aeabi_idiv(int a, int b);
extern u32 data_020a4d30;

/* The particle's 0x40 halfword (SPL's visibility): the base alpha in bits 0-4,
 * the animated alpha in bits 5-9. This
 * callback rewrites the halfword whole, so it keeps this view rather than
 * spl_particle_views.h's ParticleElement, which spells it as bitfields. */
struct ParticleAlphaView {
    char pad0[0x40];
    u16 visibility; /* 0x40 */
};

/* Three 5-bit alpha stops in one halfword. */
struct AlphaStops {
    u16 start : 5;
    u16 mid : 5;
    u16 end : 5;
    u16 pad : 1;
};

/* The particle definition's alpha animation (SPL's SPLAlphaAnim): the alpha
 * ramps start -> mid over life rates [0, in), holds mid until out, then ramps
 * mid -> end, and each step is scaled down by up to randomRange / 256. */
struct AlphaAnim {
    struct AlphaStops alpha; /* 0x0 */
    u8 randomRange;          /* 0x2 */
    u8 flags;                /* 0x3 */
    u8 in;                   /* 0x4 */
    u8 out;                  /* 0x5 */
};

/* The alpha animation callback (pret's SPL: SPLAnim_Alpha). data_020a4d30 is
 * the particle code's random seed (the same LCG as func_0204c584). */
void func_0204d1b4(struct ParticleAlphaView *ptcl, PtclDef *def, int lifeRate) {
    struct AlphaAnim *anim = def->alphaAnim;
    int in = anim->in;
    int out = anim->out;
    int value;
    if (lifeRate < in) {
        int start = anim->alpha.start;
        int mid = anim->alpha.mid;
        int diff = mid - start;
        int step = __aeabi_idiv(lifeRate * diff, in);
        value = step + start;
    } else if (lifeRate < out) {
        value = anim->alpha.mid;
    } else {
        int mid = anim->alpha.mid;
        int end = anim->alpha.end;
        int diff = end - mid;
        int step = __aeabi_idiv((lifeRate - 0xff) * diff, 0xff - out);
        value = step + end;
    }
    u16 *visibility = &ptcl->visibility;
    u32 s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
    data_020a4d30 = s;
    int range = anim->randomRange;
    u16 old = *visibility;
    *visibility =
        (u16)((old & ~0x3e0) |
              ((((u32)((value * (0xff - ((range * (int)(s >> 0x18)) >> 8))) << 8) >> 0x10) & 0x1f)
               << 5));
}
