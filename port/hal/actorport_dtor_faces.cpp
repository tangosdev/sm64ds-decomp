// ACTORPORT -- out-of-line destructor faces for the nine INLINE-destructor classes.
//
// Companion to hal/actorport_dtor_bridge.cpp; read that file's header first.
//
// WHY THESE NINE ARE DIFFERENT.  The bridge aliases the old folded name
// ??1X@@UAE@XZ and the new slot name ?Destructor1@X@@UAEXXZ onto the class's own
// complete-object destructor ??1X@@QAE@XZ.  That works for every class whose
// destructor has an out-of-line body somewhere in the link.  These nine spell
// theirs INLINE in the header (`~daStarGate_c() {}`), and an inline NON-VIRTUAL
// destructor is emitted only where a TU odr-uses it.  While the same destructor
// was VIRTUAL it was emitted unconditionally, because the compiler vtable
// referenced it -- that is the whole difference, and it is why the nine only
// surface once the host arm stops saying `virtual`.
//
// So the face below is the odr-use.  Each one is a real out-of-line function
// whose body IS the destructor -- the inline body plus the base and member
// destructor chain the compiler synthesises -- and the bridge points both names
// at it.  __fastcall(void *, void *) is the tree's dead-edx face shape: a
// __thiscall caller passes the receiver in ecx and neither side puts anything on
// the stack, so the alias is ABI-exact (the same reasoning as hal's other faces
// and the arity audit in sm64ds-port-fastcall-face-arity).
//
// daChRoom_c and daObjKumo_c are here for a second reason as well: hal/int4_rows.cpp
// aliases ??1CameraTag@@UAE@XZ and ??1Cloud@@UAE@XZ -- shadow names that
// hal/actor_classes_ov_link100.cpp references -- onto those two classes BY ADDRESS,
// and those right-hand sides were the virtual mangles that no longer exist.

#include "daBakubaku_c.h"
#include "daBgSnmBdy_c.h"
#include "daBgSnwmn_c.h"
#include "daChRoom_c.h"
#include "daObjFl_Gura_c.h"
#include "daObjKi_Ita_c.h"
#include "daObjKm1_Kuruma_c.h"
#include "daObjKm1_Kurumajiku_c.h"
#include "daObjKm2_Ukishima_c.h"
#include "daObjKumo_c.h"
#include "daStarGate_c.h"

#define ACTORPORT_D1_FACE(Class)                                              \
    extern "C" void __fastcall actorport_d1_##Class(void *s, void *unused)    \
    {                                                                         \
        (void)unused;                                                         \
        ((Class *)s)->~Class();                                               \
    }

ACTORPORT_D1_FACE(daBakubaku_c)
ACTORPORT_D1_FACE(daBgSnmBdy_c)
ACTORPORT_D1_FACE(daBgSnwmn_c)
ACTORPORT_D1_FACE(daChRoom_c)
ACTORPORT_D1_FACE(daObjFl_Gura_c)
ACTORPORT_D1_FACE(daObjKi_Ita_c)
ACTORPORT_D1_FACE(daObjKm1_Kuruma_c)
ACTORPORT_D1_FACE(daObjKm1_Kurumajiku_c)
ACTORPORT_D1_FACE(daObjKm2_Ukishima_c)
ACTORPORT_D1_FACE(daObjKumo_c)
ACTORPORT_D1_FACE(daStarGate_c)
