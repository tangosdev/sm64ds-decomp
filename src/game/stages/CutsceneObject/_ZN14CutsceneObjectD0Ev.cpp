//cpp
// @symbol _ZN14CutsceneObjectD0Ev
/* D0, the DELETING destructor. Unlike the D1/D2 pair these are NOT the
 * same code -- D0 runs the destructor and then hands the object to
 * operator delete, so it is longer. What is shared is the SOURCE: one
 * `Class::~Class()` makes mwcc emit D0, D1 and D2 together, and
 * objisolate keeps the one this file is bound to by config/.../delinks.txt.
 * That is why this file carries the same definition as
 * src/_ZN14CutsceneObjectD1Ev.cpp -- it is not duplication, it is how
 * one-symbol-per-file enrolment meets a compiler that emits three.
 *
 * The `operator delete` on the immediate base is what makes the length come
 * out right: CW inlines it into D0 only when it finds one there, and without
 * it D0 calls the global _ZdlPv and lands three words short. */

extern "C" void _ZN6Memory10DeallocateEPvP4Heap(void *ptr, void *heap);
extern "C" void *data_020a0eac;


struct dActor_c {
    virtual ~dActor_c();
    /* INLINE and on the IMMEDIATE base -- both load-bearing. CW inlines
       operator delete into D0 only when it is found on the class or its
       immediate base; without it D0 calls the global _ZdlPv, which this image
       does not contain, and comes out three words short. Mirrors
       include/dActor_c.h. */
    void operator delete(void *ptr) { _ZN6Memory10DeallocateEPvP4Heap(ptr, data_020a0eac); }
};


struct CutsceneObject : dActor_c {
    virtual ~CutsceneObject();
};

CutsceneObject::~CutsceneObject()
{
}
