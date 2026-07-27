/* MultiBootScene::~MultiBootScene (D1/complete) at 0x02034a78
 *
 * Chain MultiBootScene -> Scene -> ActorDerived, with a FaderColor member
 * subobject at +0x50.  CW emits: most-derived vptr write, member dtor,
 * then the base-subobject vptr writes, then the lowest base dtor.
 *   0x020943c4 = _ZTV14MultiBootScene
 *   bl 0x020175c4 = FaderColor::~FaderColor(this+0x50)
 *   0x02092680 = _ZTV5Scene
 *   0x0208e4b8 = _ZTV12ActorDerived
 *   bl 0x02043d48 = ActorBase::~ActorBase(this)
 *   return this;
 */

struct MultiBootScene {
    void **vtable;       /* 0x00 */
    char pad[0x50 - 4];
    void **faderVtable;  /* 0x50: FaderColor member subobject */
};

extern void *_ZTV14MultiBootScene[];
extern void *_ZTV5Scene[];
extern void *_ZTV12ActorDerived[];

extern void _ZN10FaderColorD1Ev(void *fader);          /* 0x020175c4 */
extern void _ZN9ActorBaseD2Ev(struct MultiBootScene *self); /* 0x02043d48 */

struct MultiBootScene *func_02034a78(struct MultiBootScene *self)
{
    self->vtable = (void **)_ZTV14MultiBootScene;
    _ZN10FaderColorD1Ev((char *)self + 0x50);
    self->vtable = (void **)_ZTV5Scene;
    self->vtable = (void **)_ZTV12ActorDerived;
    _ZN9ActorBaseD2Ev(self);
    return self;
}
