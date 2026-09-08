//cpp
/* Production translation unit for ov043/daObjKm1_Kurumajiku_c, hand-curated.
 * 5 function(s), .text 0x021113fc..0x02111518.
 *
 * ENROLLED AND CANONICAL. config/arm9/overlays/ov043/delinks.txt licenses that
 * whole run to this one path, so the ROM build compiles this file in place of
 * the four legacy class-method sources and the adjacent factory it replaces.
 *
 * The Bob-omb Battlefield rickshaw axle -- the spindle that daObjKm1_Kuruma_c,
 * the same overlay's cart, rides on. It adds no state of its own to
 * daObjKurumajiku_c (see include/daObjKm1_Kurumajiku_c.h) and overrides only
 * the two vtable slots the abstract base leaves pure, both of which hand the
 * actor and this level's descriptor to the shared ov002 helpers.
 *
 * THE .text RUN, DERIVED BY ADDRESS. delinks.txt bounds it on both sides with
 * a different class: src/game/actors/d_a_obj_km1_ukishima.cpp ends at
 * 0x021113fc and src/game/actors/d_a_obj_km1_kuruma.cpp begins at 0x02111518,
 * and the five members in between tile it with no gap -- 0x021113fc + 0x50 +
 * 0x64 + 0x14 + 0x18 + 0x3c = 0x02111518 exactly.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder:
 * tools/rombuild.py refuses the object outright when the emitted order and the
 * ROM's disagree.
 *
 * It absorbs five legacy one-function sources, at ROM 0x021113fc (D1),
 * 0x0211144c (D0), 0x021114b0 (CleanupResources), 0x021114c4 (InitResources)
 * and 0x021114dc (the factory). Their paths are recorded per function in
 * config/tu_manifest.d/ov043/daObjKm1_Kurumajiku_c.json, which is where a
 * reference to a deleted file belongs.
 *
 * THE FIFTH IS THE FACTORY, and build/tu_map.json does not report it. Its
 * factory rule matches names ending `_Spawn`, which labels zero functions in
 * this image, so every `_classInit` is orphaned from its class and the map's
 * count for this run is a floor of 4, not the 5 the addresses give.
 */

#include "daObjKm1_Kurumajiku_c.h"

/* The three-word block at ov043 0x02112344 that ov002's shared setup and
   teardown helpers read: this level's model and collision file handles.
   Retail places it between the class typeinfo and type-name records. Nothing
   in this TU dereferences it, so it stays an opaque descriptor rather than a
   guess at its members, and it is DECLARED rather than defined -- this entry
   claims .text only. */
struct ResourceDescriptor {
    void *entries[3];
};

extern "C" {
/* ov002's shared rickshaw setup and teardown, still under placeholder names.
   Read off the literal pools at 0x021114d4 and 0x021114bc rather than taken
   from relocs.txt, because tools/match.py wildcards every relocated word and
   cannot decide the identity of a call target. */
int func_ov002_020b6c54(daObjKm1_Kurumajiku_c *self,
                        ResourceDescriptor *descriptor, unsigned actorID);
int func_ov002_020b6ac8(daObjKm1_Kurumajiku_c *self,
                        ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov043_02112344;
}

struct Km1KurumajikuProfile {
    daObjKm1_Kurumajiku_c *(*classInit)();
    s16 profileID;
    s16 groupFlags;
    u32 actorFlags;
    Fix12i cullRadiusX;
    Fix12i cullRadiusY;
    u32 executeOrder;
    u32 drawOrder;
};

typedef char Km1KurumajikuProfile_size_must_be_0x1c[
    sizeof(Km1KurumajikuProfile) == 0x1c ? 1 : -1];

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- daObjKm1_Kurumajiku_c_classInit, 0x021114dc, size 0x3c    */
/* -------------------------------------------------------------------------- */
// @symbol daObjKm1_Kurumajiku_c_classInit
/* Reconstructed source-style name: SM64DS proves the RTTI class, the
   KM1_KURUMAJIKU registry ID, the descriptor/factory relationship and the
   object shape; later EAD lineage supplies the spelling prior. Exact original
   SM64DS symbols are not preserved. Historical project aliases:
   RickshawBdw_Spawn and RickshawBdw_SpawnInfo.

   Every instruction the cartridge has here falls out of the one `new`.
   0x021114e0 loads 0x330 -- the class's own size -- into the header's inline
   operator new; 0x021114f0 calls dBgActor_c's C2 and the two stores at
   0x021114fc and 0x02111500 lay down daObjKurumajiku_c's vptr and then this
   class's, both to offset 0, because both intervening constructors are
   implicit and inline. The null check is the one `new` itself emits. */
extern "C" daObjKm1_Kurumajiku_c *daObjKm1_Kurumajiku_c_classInit()
{
    return new daObjKm1_Kurumajiku_c();
}

/* The profile ID and descriptor relationship survive in the cartridge; the
   local descriptor type name is reconstructed from that runtime role. Every
   word below is read out of ov043 0x02112368: the factory pointer, the packed
   0x00b20089 halves, and the 0x00300000 cull radius. */
extern "C" Km1KurumajikuProfile g_profile_KM1_KURUMAJIKU = {
    daObjKm1_Kurumajiku_c_classInit,
    0x0089,
    0x00b2,
    2,
    0,
    0x00300000,
    0x01000000,
    0
};

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN21daObjKm1_Kurumajiku_c13InitResourcesEv, 0x021114c4, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN21daObjKm1_Kurumajiku_c13InitResourcesEv
/* Vtable slot 0, one of the two the abstract base leaves pure. Delegates to
   the shared ov002 setup helper with this level's descriptor and the actor ID
   of the cart the axle drives: 0x021114cc is `mov r2, #0x88`, and 0x88 is the
   profile ID of KM1_KURUMA at 0x02112438, one below this class's own 0x89.
   The ov047 twin passes 0x97 to its own 0x98 the same way. */
int daObjKm1_Kurumajiku_c::InitResources()
{
    return func_ov002_020b6c54(this, &data_ov043_02112344, 0x88);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN21daObjKm1_Kurumajiku_c16CleanupResourcesEv, 0x021114b0, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN21daObjKm1_Kurumajiku_c16CleanupResourcesEv
/* Vtable slot 3, the teardown half of the InitResources delegation, over the
   same ov043 descriptor. */
int daObjKm1_Kurumajiku_c::CleanupResources()
{
    return func_ov002_020b6ac8(this, &data_ov043_02112344);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN21daObjKm1_Kurumajiku_cD0Ev, 0x0211144c, size 0x64     */
/* ROM ordinal 0 -- _ZN21daObjKm1_Kurumajiku_cD1Ev, 0x021113fc, size 0x50     */
/* -------------------------------------------------------------------------- */
// @symbol _ZN21daObjKm1_Kurumajiku_cD1Ev
// @symbol _ZN21daObjKm1_Kurumajiku_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjKm1_Kurumajiku_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no leaf D2. Writing the
   body out of line here instead flips them to D0-before-D1 and the isolation
   step rejects the object.

   Their bodies are three vptr stores and the member destructions, every one a
   consequence of `daObjKm1_Kurumajiku_c : daObjKurumajiku_c : dBgActor_c`:
   this class's vptr, then daObjKurumajiku_c's and dBgActor_c's -- both
   inlined, because both destructors are defined in their class bodies -- then
   dBgActor_c's dBgW_KcMbg at +0x124 and Model at +0xd4, then dActor_c. This
   class adds no member with a destructor of its own, and D0's trailing
   deallocation is the inline `operator delete` it inherits, which is why
   nothing here names a heap. */
