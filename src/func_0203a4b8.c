/* func_0203a4b8 at 0x02014878
 * dBgW_KcMbg C1 (complete object) constructor:
 *   call base dCc_c::dCc_c() (C2), then set own vtable.
 */
struct Obj { void *vtable; };
extern void *_ZTV10dBgW_KcMbg[];
extern void func_020398c8(struct Obj *thiz); /* 0x020150cc */

struct Obj *func_0203a4b8(struct Obj *thiz)
{
    func_020398c8(thiz);
    thiz->vtable = (void *)_ZTV10dBgW_KcMbg;
    return thiz;
}
