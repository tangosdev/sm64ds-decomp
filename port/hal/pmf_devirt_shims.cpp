// GCC-only: reunite devirtualized placeholder-virtual calls with the object's
// real vtable.
//
// Many byte-locked Render() TUs model a real embedded vtable-bearing subobject
// with a LOCAL placeholder class -- e.g. `struct Foo { virtual void M0..M5; }`
// (BulletBill), `struct Base { virtual ... v4; virtual void M(void*); }`
// (FlameChomp), `struct Base { ...; virtual int vfunc(int); }` (FortressWall) --
// and call the 6th virtual (slot 5) on a subobject held BY VALUE:
//   ((Container*)this)->obj.M5(0);
// MSVC keeps that an indirect vtable call. GCC sees `obj` as a concrete value of
// the complete local type and DEVIRTUALIZES it into a direct call to the
// placeholder method (`Foo::M5`, `Base::M`, `Base::vfunc`) -- which has no body,
// so the link is left with an undefined reference. -fno-devirtualize does not
// help (the type really is complete here).
//
// The faithful behaviour is the indirect call the ROM makes: dispatch through
// the object's own vtable at slot 5. These weak definitions do exactly that --
// read the vptr the caller passed as `this` and tail-call slot 5 with the same
// argument. Weak so any real definition (there is none in the byte-locked tree)
// would win, and so this file is inert on any target that does not reference
// them. Linux/GCC only; src/ is untouched and MSVC never compiles this.
#if defined(__GNUC__) && !defined(_MSC_VER)

// slot 5 (6th virtual) is the called one in every placeholder here.
static inline void *vslot5(void *self)
{
    void **vt = *reinterpret_cast<void ***>(self);
    return vt[5];
}

// Foo::M5(int)  -- BulletBill/StarSwitch/... : `void M5(int)`
extern "C" __attribute__((weak)) void _ZN3Foo2M5Ei(void *self, int x)
{
    reinterpret_cast<void (*)(void *, int)>(vslot5(self))(self, x);
}

// Base::M(void*) -- FlameChomp/... : `void M(void*)`
extern "C" __attribute__((weak)) void _ZN4Base1MEPv(void *self, void *arg)
{
    reinterpret_cast<void (*)(void *, void *)>(vslot5(self))(self, arg);
}

// Base::vfunc(int) -- FortressWall/PeachPainting/... : `int vfunc(int)`
extern "C" __attribute__((weak)) int _ZN4Base5vfuncEi(void *self, int a)
{
    return reinterpret_cast<int (*)(void *, int)>(vslot5(self))(self, a);
}

// ---- FaderBrightness direct-call entry points -----------------------------
// ProcessKuppaScript and a few others call `fader->SetToEnd()` etc. directly on
// a FaderBrightness* that at runtime points at the host HalFaderWipe instances
// (port/hal/fader_wipes.cpp, hal_wipes[]). MSVC compiles each as the class's
// vtable slot; the host HalFaderWipe is a real vtable-bearing object, so the
// faithful behaviour is the same indirect call. HalFaderWipe is an
// anonymous-namespace type (internal linkage) so it cannot be aliased to
// cross-TU; instead dispatch through the object's own vtable at the slot the
// class layout puts each method (byte offsets 0x0c/0x10/0x20/0x24 -> slots
// 3/4/8/9, see fader_wipes.cpp's slot map). Weak; Linux/GCC only.
static inline void *vslot(void *self, int n)
{
    return (*reinterpret_cast<void ***>(self))[n];
}
extern "C" __attribute__((weak)) void _ZN15FaderBrightness8SetToEndEv(void *self)
{   reinterpret_cast<void (*)(void *)>(vslot(self, 8))(self); }
extern "C" __attribute__((weak)) void _ZN15FaderBrightness10SetToStartEv(void *self)
{   reinterpret_cast<void (*)(void *)>(vslot(self, 9))(self); }
extern "C" __attribute__((weak)) int _ZN15FaderBrightness14SetForwardTimeEjj(void *self, unsigned a, unsigned b)
{   return reinterpret_cast<int (*)(void *, unsigned, unsigned)>(vslot(self, 4))(self, a, b); }
extern "C" __attribute__((weak)) int _ZN15FaderBrightness15SetBackwardTimeEjj(void *self, unsigned a, unsigned b)
{   return reinterpret_cast<int (*)(void *, unsigned, unsigned)>(vslot(self, 3))(self, a, b); }

#endif
