/* BlendModelAnim::~BlendModelAnim (deleting / D0) at 0x02016644
 *
 * BlendModelAnim : ModelAnim : Model, with an Animation member subobject
 * at +0x50 and an owned pointer at +0x6c.
 *   [this+0]    = _ZTV14BlendModelAnim                  (0x0208e94c)
 *   [this+0x50] = VTable_Animation_BlendModelAnimThunk  (0x0208e970)
 *   if (this->unk6c) operator delete(this->unk6c)  (bl 0x0203cbc0 -> func_0203cbc0)
 *   bl 0x0201689c = ModelAnim::~ModelAnim(this)    (D2/base)
 *   bl 0x0203cbcc = Memory::operator_delete2(this)
 *   return this;
 */

struct BlendModelAnim {
    void **vtable;       /* 0x00 */
    char pad0[0x50 - 4];
    void **animVtable;   /* 0x50: Animation member subobject */
    char pad1[0x6c - 0x54];
    void *unk6c;         /* 0x6c: owned pointer */
};

extern void *_ZTV14BlendModelAnim[];
extern void *VTable_Animation_BlendModelAnimThunk[];

extern void func_0203cbc0(void *ptr);                   /* 0x0203cbc0 */
extern void *_ZN9ModelAnimD2Ev(struct BlendModelAnim *self); /* 0x0201689c */
extern void _ZN6Memory16operator_delete2EPv(void *p);   /* 0x0203cbcc */

struct BlendModelAnim *_ZN14BlendModelAnimD0Ev(struct BlendModelAnim *self)
{
    self->vtable = (void **)_ZTV14BlendModelAnim;
    self->animVtable = (void **)VTable_Animation_BlendModelAnimThunk;
    if (self->unk6c)
        func_0203cbc0(self->unk6c);
    _ZN9ModelAnimD2Ev(self);
    _ZN6Memory16operator_delete2EPv(self);
    return self;
}
