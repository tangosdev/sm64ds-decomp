struct FaderWipe { void **vtable; };
extern void *_ZTV9FaderWipe[];
extern void _ZN5ModelD1Ev(void *thiz);
extern void _ZN10FaderColorD2Ev(struct FaderWipe *thiz);   /* base subobject dtor @0x02017574 */
extern void _ZN6Memory16operator_delete2EPv(void *ptr);

struct FaderWipe *_ZN9FaderWipeD0Ev(struct FaderWipe *thiz)
{
    thiz->vtable = (void **)_ZTV9FaderWipe;
    _ZN5ModelD1Ev((char *)thiz + 0x10);
    _ZN10FaderColorD2Ev(thiz);
    _ZN6Memory16operator_delete2EPv(thiz);
    return thiz;
}
