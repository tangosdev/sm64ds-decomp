#include "types.h"
#include "private/spl_particle_views.h"
extern void SPLUtil_SetTexture(void *p);
extern void SPLUtil_SetTexture_Stub(void *p);
extern void SPLDraw_Polygon(void *self, void *node);
extern void SPLDraw_Billboard(void *self, void *node);
extern void SPLDraw_DirectionalBillboard(void *self, void *node);
extern void SPLDraw_Child_Polygon(void *self, void *node);
extern void SPLDraw_Child_Billboard(void *self, void *node);
extern void SPLDraw_Child_DirectionalBillboard(void *self, void *node);

/* Draw the current system's particles, then its child particles: bind the
 * definition's texture, pick the draw routine by draw type (billboard,
 * directional billboard, polygon), and when the definition animates its
 * texture rebind each particle's own before drawing it
 * (SPLUtil_SetTexture_Stub is the no-op). pret's SPL splits this into SPLManager_DrawParticles and
 * SPLManager_DrawChildParticles. */
void SPLManager_DoDraw(char *self) {
    ManagerView *mgr = (ManagerView *)self;
    ParticleSystem *system = mgr->mCurrentSystem;
    PtclDef *def = system->mDefinition;
    ParticleElement *n;
    PtclData *data = def->data;
    void (*draw)(void *, void *) = 0;
    void (*setTexture)(void *);

    SPLUtil_SetTexture(&mgr->mTextures[data->texture]);

    switch (data->drawType) {
    case 0:
        draw = SPLDraw_Billboard;
        break;
    case 1:
        draw = SPLDraw_DirectionalBillboard;
        break;
    case 2:
        draw = SPLDraw_Polygon;
        break;
    }

    setTexture = data->hasTexAnim ? SPLUtil_SetTexture : SPLUtil_SetTexture_Stub;

    n = system->particles.head;
    if (n != 0) {
        do {
            setTexture(&mgr->mTextures[n->texture]);
            draw(self, n);
            n = n->next;
        } while (n != 0);
    }

    if (!data->hasChildResource)
        return;

    SPLUtil_SetTexture(&mgr->mTextures[def->child->texture]);

    switch (def->child->flags.drawType) {
    case 0:
        draw = SPLDraw_Child_Billboard;
        break;
    case 1:
        draw = SPLDraw_Child_DirectionalBillboard;
        break;
    case 2:
        draw = SPLDraw_Child_Polygon;
        break;
    }

    n = system->particles2.head;
    if (n == 0)
        return;
    do {
        draw(self, n);
        n = n->next;
    } while (n != 0);
}
