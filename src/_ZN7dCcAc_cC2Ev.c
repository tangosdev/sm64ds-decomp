/* _ZN7dCcAc_cC2Ev at 0x020149f4
 * dCcAc_c C2 (base subobject) constructor:
 *   same code as C1 for single-inheritance with no virtual bases.
 */
struct dCcAc_c {
    void *vtable;       /* 0x00 */
    char pad[0x30 - 4];
    void *owner;        /* 0x30 */
};

extern void *_ZTV7dCcAc_c[];
extern void _ZN5dCc_cC2Ev(struct dCcAc_c *thiz); /* 0x020150cc */

struct dCcAc_c *_ZN7dCcAc_cC2Ev(struct dCcAc_c *thiz)
{
    _ZN5dCc_cC2Ev(thiz);
    thiz->vtable = (void *)_ZTV7dCcAc_c;
    thiz->owner = (void *)0;
    return thiz;
}
