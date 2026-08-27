/* View::~View (deleting / D0) at 0x0202fcc8
 *
 * vtable chain View -> dBase_c:
 *   0x02092720 = _ZTV4View
 *   0x0208e4b8 = data_0208e4b8
 *   bl 0x02043d48 = fBase_c::~fBase_c
 *   bl 0x0203c1e8 = Memory::Deallocate(this, *gameHeapPtr)
 *   return this;
 */

struct View { void **vtable; /* 0x0 */ };
struct Heap;

extern void *_ZTV4View[];
extern void *data_0208e4b8[];

extern void _ZN7fBase_cD2Ev(struct View *self);              /* 0x02043d48 */
extern void _ZN6Memory10DeallocateEPvP4Heap(void *ptr, struct Heap *heap); /* 0x0203c1e8 */
extern struct Heap *data_020a0eac;                  /* 0x020a0eac */

struct View *_ZN4ViewD0Ev(struct View *self)
{
    self->vtable = (void **)_ZTV4View;
    self->vtable = (void **)data_0208e4b8;
    _ZN7fBase_cD2Ev(self);
    _ZN6Memory10DeallocateEPvP4Heap(self, data_020a0eac);
    return self;
}
