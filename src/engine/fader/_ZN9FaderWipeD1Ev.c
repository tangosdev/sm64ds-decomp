/* FaderWipe::~FaderWipe (D1/complete) at 0x02017450
 *
 *   [this+0] = _ZTV9FaderWipe (0x0208ea9c)
 *   bl 0x02016d20 = Model::~Model(this+0x10)   (Model member subobject)
 *   bl 0x02017574 = Color::~Color(this)        (immediate base dtor)
 *   return this;
 */

struct FaderWipe {
    void **vtable;       /* 0x00 */
    char pad[0x10 - 4];
    void **modelVtable;  /* 0x10: Model member subobject */
};

extern void *_ZTV9FaderWipe[];
extern void _ZN5ModelD1Ev(void *model);          /* 0x02016d20 */
extern void _ZN5ColorD1Ev(struct FaderWipe *t);  /* 0x02017574 */

struct FaderWipe *_ZN9FaderWipeD1Ev(struct FaderWipe *thiz)
{
    thiz->vtable = (void **)_ZTV9FaderWipe;
    _ZN5ModelD1Ev((char *)thiz + 0x10);
    _ZN5ColorD1Ev(thiz);
    return thiz;
}
