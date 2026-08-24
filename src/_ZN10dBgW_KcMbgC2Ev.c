/* _ZN10dBgW_KcMbgC2Ev - dBgW_KcMbg's base-subobject constructor at
 * 0x0203a4b8, renamed from func_0203a4b8 (2026-08-24). Its only ROM
 * caller is the derived base step inside dBgW_KcMbgSclY's ctor pair,
 * so the variant label is C2 -- section 4 of notes/ctor-migration.md.
 * (The old banner's address and dCc_c guess were stale.)
 */
struct Obj { void *vtable; };
extern void *_ZTV10dBgW_KcMbg[];
extern void _ZN7dBgW_KcC2Ev(struct Obj *thiz); /* 0x020398c8 */

struct Obj *_ZN10dBgW_KcMbgC2Ev(struct Obj *thiz)
{
    _ZN7dBgW_KcC2Ev(thiz);
    thiz->vtable = (void *)_ZTV10dBgW_KcMbg;
    return thiz;
}
