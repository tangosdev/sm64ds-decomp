//cpp
// @symbol _ZN7dWipe_c20IsBetweenStartAndEndEv
/* recovered: renamed to Class_Method */
/* dWipe_c::IsBetweenStartAndEnd() -- vtable slot 7. extern "C" carries the
 * literal mangled name unmangled -- see include/dWipe_c.h. */
struct FaderBrightness {
    virtual void m00();
    virtual void m04();
    virtual void m08();
    virtual void m0c();
    virtual void m10();
    virtual int  m14();      /* vtable 0x14 */
    virtual int  m18();      /* vtable 0x18 */
    int pad[4];
    int type;            /* 0x14 */
};

extern "C" int _ZN15FaderBrightness20IsBetweenStartAndEndEv(struct FaderBrightness *thiz);

extern "C" int _ZN7dWipe_c20IsBetweenStartAndEndEv(struct FaderBrightness *thiz)
{
    if (thiz->type == 1)
        return _ZN15FaderBrightness20IsBetweenStartAndEndEv(thiz);
    if (!thiz->m14()) {
        if (thiz->m18() == 0)
            return 1;
    }
    return 0;
}
