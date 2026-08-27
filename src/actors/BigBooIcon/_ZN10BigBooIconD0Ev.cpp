//cpp
// @symbol _ZN10BigBooIconD0Ev
/* D0, the DELETING destructor: run the destructor body, then hand the object
 * to operator delete. Longer than the D1/D2 pair, but built from the same one
 * `BigBooIcon::~BigBooIcon()` definition -- so this file carries the same definition as its
 * D1 sibling, and objisolate keeps the variant the delinks entry names. */

extern "C" void _ZN6Memory10DeallocateEPvP4Heap(void *ptr, void *heap);
extern "C" void *data_020a0eac;


struct dActor_c {
    virtual ~dActor_c();
    /* CW inlines operator delete into D0 only when it finds one on the class
       or its immediate base; without it D0 calls the global _ZdlPv, which this
       image does not contain, and lands three words short. Mirrors
       include/dActor_c.h. */
    void operator delete(void *ptr) { _ZN6Memory10DeallocateEPvP4Heap(ptr, data_020a0eac); }
};


struct BigBooIcon : dActor_c {
    virtual ~BigBooIcon();
};

BigBooIcon::~BigBooIcon()
{
}
