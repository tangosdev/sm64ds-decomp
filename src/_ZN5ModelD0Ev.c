/* Model::~Model (deleting / D0) at 0x02016ce0
 *
 * Model : ModelBase, with an owned pointer at +0x4c.
 *   [this+0] = _ZTV5Model (0x0208e90c)
 *   if (this->unk4c) operator delete(this->unk4c)  (bl 0x0203cbc0 -> func_0203cbc0)
 *   bl 0x020170b8 = ModelBase::~ModelBase(this)    (D2/base)
 *   bl 0x0203cbcc = Memory::operator_delete2(this)
 *   return this;
 */

struct Model {
    void **vtable;   /* 0x00 */
    char pad[0x4c - 4];
    void *unk4c;     /* 0x4c: owned pointer */
};

extern void *_ZTV5Model[];
extern void func_0203cbc0(void *ptr);                          /* 0x0203cbc0 */
extern void *_ZN9ModelBaseD2Ev(struct Model *self);     /* 0x020170b8 */
extern void _ZN6Memory16operator_delete2EPv(void *p);   /* 0x0203cbcc */

struct Model *_ZN5ModelD0Ev(struct Model *self)
{
    self->vtable = (void **)_ZTV5Model;
    if (self->unk4c)
        func_0203cbc0(self->unk4c);
    _ZN9ModelBaseD2Ev(self);
    _ZN6Memory16operator_delete2EPv(self);
    return self;
}
