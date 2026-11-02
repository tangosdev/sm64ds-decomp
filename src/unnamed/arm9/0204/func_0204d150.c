#include "types.h"
#include "private/spl_particle_views.h"
#pragma opt_strength_reduction off /* required: without it the match fails */

/* The particle definition's texture animation (SPL's SPLTexAnim): up to eight
 * texture indices, each shown for `step` life-rate units. */
struct TexAnim {
    u8 textures[8]; /* 0x0 */
    u8 frameCount;  /* 0x8 */
    u8 step;        /* 0x9 */
};

/* The texture animation callback (pret's SPL: SPLAnim_Texture): picks the
 * frame whose span holds lifeRate. */
void func_0204d150(ParticleElement *ptcl, PtclDef *def, int lifeRate) {
    struct TexAnim *anim = def->texAnim;
    int i = 0;
    int end;
    int count = anim->frameCount;
    u8 step;
    if (count <= 0) {
        return;
    }
    step = anim->step;
    end = 0;
    do {
        if (lifeRate < (end + step)) {
            ptcl->texture = anim->textures[i];
            return;
        }
        i++;
        end += step;
    } while (i < anim->frameCount);
}
