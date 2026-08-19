/* _ZN18MovingCylinderClsnC2Ev at 0x020149f4
 * MovingCylinderClsn C2 (base subobject) constructor:
 *   same code as C1 for single-inheritance with no virtual bases.
 */
struct MovingCylinderClsn {
    void *vtable;       /* 0x00 */
    char pad[0x30 - 4];
    void *owner;        /* 0x30 */
};

extern void *_ZTV18MovingCylinderClsn[];
extern void _ZN12CylinderClsnC2Ev(struct MovingCylinderClsn *thiz); /* 0x020150cc */

struct MovingCylinderClsn *_ZN18MovingCylinderClsnC2Ev(struct MovingCylinderClsn *thiz)
{
    _ZN12CylinderClsnC2Ev(thiz);
    thiz->vtable = (void *)_ZTV18MovingCylinderClsn;
    thiz->owner = (void *)0;
    return thiz;
}
