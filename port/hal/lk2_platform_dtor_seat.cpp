// LINKAGE SEAT: Platform's own ROM destructor pair, seated into the base
// table for the walk_window family.
//
// The ROM's Platform base table (ov002 0x0210ae38) carries exactly three of
// the class's own bodies: slot 16 the complete-object destructor (config
// spells it _ZN8PlatformD2Ev, the same D1/D2 spelling note the
// MeshColliderBase seat carries), slot 17 the deleting destructor
// _ZN8PlatformD0Ev, and slot 31 Platform::Kill, which is already linked.
// Both destructor TUs are clean flat-C matched src (src/_ZN8PlatformD2Ev.c,
// src/_ZN8PlatformD0Ev.c); until this seat they were in no slice, so the two
// bodies the ROM table names were the only part of it the port did not carry.
//
// STORAGE vs SEAT, the meshcolliderbase_vtable.cpp reading. The table is
// hosted TWICE under the two names destructor TUs spell for the same ROM
// object: _ZTV10dBgActor_c (hal/actor_classes.cpp) and _ZTV8Platform
// (hal/actor_vtables.cpp). hal_fill_platform_vtable gives both the shared
// half and traps the rest, and its own comment records why slots 16/17 could
// stay trapped: the base table is only installed BETWEEN two member teardowns
// of a derived destructor chain and nothing dispatches through it while it is
// there. That reading holds; this seat does not change it. Seating the real
// pair is the reference edge that pulls the two matched TUs into the link,
// and it makes the hosted table say what the ROM's says.
//
// WHY A SEPARATE FILE, walk_window family only. The fill that owns the
// storage (hal/actor_classes.cpp) already rides only smoke_player,
// walk_window and walk_window_hires, and the pair rides slice_gate16 (the
// same three targets), so naming the bodies here resolves everywhere this TU
// links. The gate-8/9 smoke targets link neither the storage nor the fill and
// are untouched.
//
// ABI. Both flat-C bodies are `int *f(int *this)` (cdecl, this on the stack).
// The slots are dispatched __thiscall (this in ecx), so each takes the
// ecx->arg adapter, the meshcollider_dtor_seat.cpp convention. D2 (the
// complete form) tears down the MovingMeshCollider at +0x124, the Model at
// +0xd4 and Actor's base; D0 is the same chain plus Memory::Deallocate on the
// game heap (the TU's G0, aliased to data_020a0eac in hal/cxx_aliases.cpp,
// whose comment has named the Platform D0 pair as that alias's reader since
// before this seat existed).

extern "C" {

extern void *_ZTV10dBgActor_c[32];   /* storage: hal/actor_classes.cpp */
extern void *_ZTV8Platform[32];      /* storage: hal/actor_vtables.cpp */

int *_ZN8PlatformD2Ev(int *self);    /* slot 16 body, complete (ov002 0x020ee42c) */
int *_ZN8PlatformD0Ev(int *self);    /* slot 17 body, deleting (ov002 0x020ee464) */

void hal_fill_platform_vtable(void); /* idempotent; run it first so the seats
                                        land after the shared half, not under
                                        a later trap prefill */
}

static int __fastcall plat_d1(void *s, void *)
{ return (int)(__int64)_ZN8PlatformD2Ev((int *)s); }
static int __fastcall plat_d0(void *s, void *)
{ return (int)(__int64)_ZN8PlatformD0Ev((int *)s); }

/* Called from level_boot's boot fills. Naming the pair here is the reference
   edge; both host arrays stand in for the ONE ROM table and get the same
   contents, the hal_fill_platform_vtable contract. */
extern "C" void hal_seat_platform_dtors(void)
{
    hal_fill_platform_vtable();
    void **tabs[2] = { _ZTV10dBgActor_c, _ZTV8Platform };
    for (int k = 0; k < 2; ++k) {
        tabs[k][16] = (void *)plat_d1;
        tabs[k][17] = (void *)plat_d0;
    }
}
