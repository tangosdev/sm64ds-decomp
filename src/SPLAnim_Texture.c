#include "types.h"
#include "private/spl_particle_views.h"

/* The particle definition's texture animation (SPL's SPLTexAnim): up to eight
 * texture indices, each shown for `step` life-rate units. */
struct TexAnim {
    u8 textures[8]; /* 0x0 */
    u8 frameCount;  /* 0x8 */
    u8 step;        /* 0x9 */
};

/* The texture animation callback: picks the frame whose span holds
 * lifeRate. The name is pret's SPL coinage, not a ROM or SDK symbol. */
void SPLAnim_Texture(ParticleElement *ptcl, PtclDef *def, int lifeRate) {
    struct TexAnim *anim = def->texAnim;
    int i;
    for (i = 0; i < anim->frameCount; i++) {
        if (lifeRate < anim->step * (i + 1)) {
            ptcl->texture = anim->textures[i];
            return;
        }
    }
}
