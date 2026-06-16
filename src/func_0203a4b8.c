/* func_0203a4b8 at 0x02014878
 * CylinderClsnWithPos C1 (complete object) constructor:
 *   call base CylinderClsn::CylinderClsn() (C2), then set own vtable.
 */
struct Obj { void *vtable; };
extern void *_ZTV19CylinderClsnWithPos[];
extern void _ZN12CylinderClsnC2Ev(struct Obj *thiz); /* 0x020150cc */

struct Obj *func_0203a4b8(struct Obj *thiz)
{
    _ZN12CylinderClsnC2Ev(thiz);
    thiz->vtable = (void *)_ZTV19CylinderClsnWithPos;
    return thiz;
}
