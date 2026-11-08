#include "types.h"
#include "private/spl_particle_views.h"
extern void func_0204af3c(void *p);
extern void SPLUtil_SetTexture_Stub(void *p);
extern void func_0204b244(void *self, void *node);
extern void func_0204be40(void *self, void *node);
extern void SPLDraw_DirectionalBillboard(void *self, void *node);
extern void func_0204b028(void *self, void *node);
extern void func_0204bbd8(void *self, void *node);
extern void SPLDraw_Child_DirectionalBillboard(void *self, void *node);

/* Draw the current system's particles, then its child particles: bind the
 * definition's texture, pick the draw routine by draw type (billboard,
 * directional billboard, polygon), and when the definition animates its
 * texture rebind each particle's own before drawing it
 * (SPLUtil_SetTexture_Stub is the no-op). pret's SPL splits this into SPLManager_DrawParticles and
 * SPLManager_DrawChildParticles. */
void func_0204a5c8(char *self) {
    ManagerView *mgr = (ManagerView *)self;
    ParticleSystem *system = mgr->mCurrentSystem;
    PtclDef *def = system->mDefinition;
    ParticleElement *n;
    PtclData *data = def->data;
    void (*draw)(void *, void *) = 0;
    void (*setTexture)(void *);

    func_0204af3c(&mgr->mTextures[data->texture]);

    switch (data->drawType) {
    case 0:
        draw = func_0204be40;
        break;
    case 1:
        draw = SPLDraw_DirectionalBillboard;
        break;
    case 2:
        draw = func_0204b244;
        break;
    }

    setTexture = data->hasTexAnim ? func_0204af3c : SPLUtil_SetTexture_Stub;

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

    func_0204af3c(&mgr->mTextures[def->child->texture]);

    switch (def->child->flags.drawType) {
    case 0:
        draw = func_0204bbd8;
        break;
    case 1:
        draw = SPLDraw_Child_DirectionalBillboard;
        break;
    case 2:
        draw = func_0204b028;
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
