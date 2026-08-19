/* _ZN18MovingCylinderClsnC1Ev at 0x020149c8
 * MovingCylinderClsn C1 (complete object) constructor:
 *   call base CylinderClsn::CylinderClsn() (C2), set own vtable, zero owner ptr.
 */
struct MovingCylinderClsn {
    void *vtable;       /* 0x00 */
    char pad[0x30 - 4];
    void *owner;        /* 0x30 */
};

extern void *_ZTV18MovingCylinderClsn[];
extern void _ZN12CylinderClsnC2Ev(struct MovingCylinderClsn *thiz); /* 0x020150cc */

struct MovingCylinderClsn *_ZN18MovingCylinderClsnC1Ev(struct MovingCylinderClsn *thiz)
{
    _ZN12CylinderClsnC2Ev(thiz);
    thiz->vtable = (void *)_ZTV18MovingCylinderClsn;
    thiz->owner = (void *)0;
    return thiz;
}
