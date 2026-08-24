/* ModelBase::~ModelBase (D0/deleting) at 0x020170e8
 *
 *   [this+0]  = _ZTV9ModelBase (0x0208e87c)
 *   ptr = this->unk4; if (ptr) Deallocate(ptr);
 *   Memory::operator_delete2(this);
 *   return this;
 */

struct ModelBase {
    void **vtable;   /* 0x00 */
    void *unk4;      /* 0x04 */
};

extern void *_ZTV9ModelBase[];
extern void Deallocate(void *ptr);                  /* 0x02018144 */
extern void _ZN6Memory16operator_delete2EPv(void *p); /* 0x0203cbcc */

struct ModelBase *_ZN9ModelBaseD0Ev(struct ModelBase *thiz)
{
    thiz->vtable = (void **)_ZTV9ModelBase;
    if (thiz->unk4)
        Deallocate(thiz->unk4);
    _ZN6Memory16operator_delete2EPv(thiz);
    return thiz;
}
