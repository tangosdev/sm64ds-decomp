// Lane DTOR-PAIRS-C: the three ROOT actor tables' own destructor pairs, seated
// where the port's host arrays carried zeros.
//
// ActorBase (arm9 0x02099edc), ActorDerived (0x0208e4b8) and View (0x02092720)
// are the transient ctor/dtor installs every actor passes through, and the
// port hosts all three as storage only (hal/actor_vtables.cpp,
// hal/camera_bridges.cpp): nothing dispatches through them, so no fill ever
// wrote their dtor words. The ROM's words 16/17 of each are the class's own
// flat-C D1/D0 by relocation:
//
//   data_02099edc[16] <- _ZN9ActorBaseD1Ev      from:0x02099f1c -> 0x02043dbc
//   data_02099edc[17] <- _ZN9ActorBaseD0Ev      from:0x02099f20 -> 0x02043d78
//   data_0208e4b8[16] <- _ZN12ActorDerivedD1Ev  from:0x0208e4f8 -> 0x02013e80
//   data_0208e4b8[17] <- _ZN12ActorDerivedD0Ev  from:0x0208e4fc -> 0x02013ea4
//   _ZTV4View[16]     <- _ZN4ViewD1Ev           from:0x02092760 -> 0x0202fc98
//   _ZTV4View[17]     <- _ZN4ViewD0Ev           from:0x02092764 -> 0x0202fcc8
//
// Every body byte-matches under 2004/b56 and spells only hosted names
// (_ZTV9ActorBase / _ZTV12ActorDerived are aliases onto the two arrays,
// hal/cxx_aliases.cpp; data_020a0eac is the game heap). Their callee closure
// (ActorBase D2, the ProcessingListNode dtor func_020440e8, Memory::Deallocate)
// is linked already. Each slot takes the matched body behind the ecx->arg
// adapter, the lk2_platform_dtor_seat.cpp shape. Called from the tail of
// port_stage_a2_seat beside hal_seat_model_family_dtors; walk_window family
// only (rides the same CMake lists as hal/model_dtor_seat.cpp).

extern "C" {

extern int data_02099edc[];      /* _ZTV9ActorBase, hal/actor_vtables.cpp */
extern int data_0208e4b8[];      /* _ZTV12ActorDerived, hal/actor_vtables.cpp */
extern void *_ZTV4View[];        /* hal/camera_bridges.cpp */

int *_ZN9ActorBaseD1Ev(void *self);
void *_ZN9ActorBaseD0Ev(void *self);
void *_ZN12ActorDerivedD1Ev(void *self);
void *_ZN12ActorDerivedD0Ev(void *self);
void *_ZN4ViewD1Ev(void *self);
void *_ZN4ViewD0Ev(void *self);

}

static int __fastcall ab_d1(void *s, void *) { return (int)(size_t)_ZN9ActorBaseD1Ev(s); }
static int __fastcall ab_d0(void *s, void *) { return (int)(size_t)_ZN9ActorBaseD0Ev(s); }
static int __fastcall ad_d1(void *s, void *) { return (int)(size_t)_ZN12ActorDerivedD1Ev(s); }
static int __fastcall ad_d0(void *s, void *) { return (int)(size_t)_ZN12ActorDerivedD0Ev(s); }
static int __fastcall vw_d1(void *s, void *) { return (int)(size_t)_ZN4ViewD1Ev(s); }
static int __fastcall vw_d0(void *s, void *) { return (int)(size_t)_ZN4ViewD0Ev(s); }

extern "C" void hal_seat_base_actor_dtors(void)
{
    data_02099edc[16] = (int)(size_t)ab_d1;
    data_02099edc[17] = (int)(size_t)ab_d0;
    data_0208e4b8[16] = (int)(size_t)ad_d1;
    data_0208e4b8[17] = (int)(size_t)ad_d0;
    _ZTV4View[16]     = (void *)vw_d1;
    _ZTV4View[17]     = (void *)vw_d0;
}
