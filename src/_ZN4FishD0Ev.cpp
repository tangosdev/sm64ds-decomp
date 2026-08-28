//cpp
// @symbol _ZN4FishD0Ev
/* D0, the DELETING destructor. Unlike the D1/D2 pair these are NOT the
 * same code -- D0 runs the destructor and then hands the object to
 * operator delete, so it is longer. What is shared is the SOURCE: one
 * `Fish::~Fish()` makes mwcc emit D0, D1 and D2 together, and objisolate
 * keeps the one this file is bound to by its delinks entry. That is why
 * this file carries the same definition as src/_ZN4FishD1Ev.cpp.
 *
 * The `operator delete` on the immediate base is what makes the length come
 * out right: CW inlines it into D0 only when it finds one there, and without
 * it D0 calls the global _ZdlPv and lands three words short.
 *
 * (Verified in #1851 and left out of that PR by mistake -- it was the case
 * the whole operator-delete finding came from.) */

extern "C" void _ZN6Memory10DeallocateEPvP4Heap(void *ptr, void *heap);
extern "C" void *data_020a0eac;

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
    void operator delete(void *ptr) { _ZN6Memory10DeallocateEPvP4Heap(ptr, data_020a0eac); }
};

struct ModelAnim { char pad[0x4]; ~ModelAnim(); };

struct Fish : dActor_c {
    ModelAnim m0;   /* 0xd4 */
    virtual ~Fish();
};

Fish::~Fish()
{
}
