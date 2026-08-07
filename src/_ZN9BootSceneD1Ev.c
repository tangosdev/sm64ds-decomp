/* BootScene::~BootScene() at 0x02023598 -- complete-object destructor (D1).
 *
 * This was named _ZN5SceneD2Ev, which was wrong twice over: it is a D1, not a
 * D2, and it belongs to BootScene, not Scene. It occupies slot 16 of
 * dScBoot_c's vtable (0x02091528) -- the slot right beside _ZN9BootSceneD0Ev --
 * and a D2 is never in a vtable at all. Scene's own D1/D0 live at 0x0202e140
 * and 0x0202e170, some 43KB away; every other class in the tree emits its
 * destructor variants contiguously.
 *
 * The three vptr writes are the chain with the intermediate destructors inlined:
 * dScBoot_c -> dScene_c -> dBase_c, then a real call to fBase_c's D2.
 * See notes/dtor-variant-audit.md.
 */

extern void *data_02091528;  /* dScBoot_c's vtable -- only BootScene writes this */
extern void *_ZTV5Scene;
extern void *data_0208e4b8;  /* == _ZTV12ActorDerived, the ROM's dBase_c */
extern void *_ZN9ActorBaseD2Ev(void *self);

struct BootScene {
    void *vtable;
};

void *_ZN9BootSceneD1Ev(struct BootScene *self)
{
    self->vtable = &data_02091528;
    self->vtable = &_ZTV5Scene;
    self->vtable = &data_0208e4b8;
    _ZN9ActorBaseD2Ev(self);
    return self;
}
