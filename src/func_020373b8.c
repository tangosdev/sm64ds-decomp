struct dBgCh_Actr { void **vtable; };
extern void *_ZTV10dBgCh_Actr[];
extern void _ZN9dBgCh_LinD1Ev(void *thiz);
extern void _ZN12dBgCh_SphCrrD1Ev(void *thiz);
extern void func_020354d0(struct dBgCh_Actr *thiz);   /* base D2 @0x020354d0 */
extern void _ZN6Memory16operator_delete2EPv(void *ptr);

struct dBgCh_Actr *func_020373b8(struct dBgCh_Actr *thiz)
{
    thiz->vtable = (void **)_ZTV10dBgCh_Actr;
    _ZN9dBgCh_LinD1Ev((char *)thiz + 0x134);
    _ZN12dBgCh_SphCrrD1Ev((char *)thiz + 0x20);
    func_020354d0(thiz);
    _ZN6Memory16operator_delete2EPv(thiz);
    return thiz;
}
