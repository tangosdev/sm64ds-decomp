/* HOST COPY of src/_ZN13RollingLogTtm8BehaviorEv.cpp -- the Ukiki's Behavior,
 * ROM slot 6 of the daMky_c table at 0x02115bfc. Run rel0215, lane cast-ov030.
 *
 * THE NAME IS THE CONFIG'S AND IT IS WRONG. 0x02114278 is slot 6 of the table
 * whose own type_info reads "7daMky_c", so this is the Ukiki's Behavior, not
 * RollingLogTtm's. The symbol keeps the config spelling because that is what
 * the ROM slot and every other TU reference; port/ov030_syms.txt's header
 * carries the evidence. Nothing is renamed by this lane.
 *
 * WHY A HOST COPY: THE SHADOW-CLASS SLOT DEFECT, stack-unbalancing half.
 * The matched src models the object at this+0xd4 with a LOCAL FOUR-VIRTUAL
 * shadow and dispatches the fourth word:
 *
 *     struct VObj { virtual void v0(); virtual void v1();
 *                   virtual void v2(); virtual void v3(); };
 *     ((VObj *)(c + 0xd4))->v3();
 *
 * That is the ROM's own numbering -- byte +0xc of an Itanium model vtable is
 * the fourth word, and on the model classes that word is UpdateVerts
 * (0 D1, 1 D0, 2 DoSetFile, 3 UpdateVerts, 4 Virtual10, 5 Render).
 *
 * On the host that byte is the FOURTH MSVC word, and MSVC folds D1 and D0 into
 * one slot, so the host's fourth word is Virtual10(Matrix4x3 &) -- one slot
 * along. TWO things then go wrong at once, and the second is the one that
 * surfaces two frames away from the cause:
 *   1. the wrong body runs, with a `Matrix4x3 &` that was never passed;
 *   2. Virtual10 is __thiscall with ONE stack parameter, so it returns with
 *      `ret 4` and eats four bytes the caller never pushed.
 * That is the exact pair of failures port/unmatched/MgMemory2_ShadowSlot_
 * 020c06dc.cpp measured, and the same slot.
 *
 * WHICH CLASS THE OBJECT IS, established from the CONSTRUCTOR rather than
 * guessed from the call: UkikiStar_Spawn and UkikiThief_Spawn (both factories
 * for this one class) each run `_ZN9ModelAnimC1Ev((char *)p + 0xd4)`, so +0xd4
 * is a ModelAnim, whose UpdateVerts is MSVC slot 2 under include/ModelAnim.h.
 * hal/cxxname_bridge.cpp:518 states in its own words that ModelAnim gets NO
 * dual-fill, so unlike the two plain-Model shadows in this slice this one
 * cannot be left alone.
 *
 * THE DISPATCH STAYS VIRTUAL rather than qualified, for
 * MgFlower_ModelRender_020c3bf4.cpp's reason: the ROM's own sequence is
 * (ldr vptr / ldr [vptr,#0xc] / blx) and a qualified call would silently
 * devirtualise a subclass override.
 *
 * AND A SECOND DELTA: A SHORT-1 CALL, WITH THE ROM'S OWN ARGUMENT RESTORED.
 * The matched src declares `extern void func_ov030_021141a8(Actor *c);` and
 * calls it with ONE argument. The real body takes TWO --
 * `void func_ov030_021141a8(char *self, int idx)` -- and the ROM passes the
 * second in r1:
 *
 *   02114324  mov  r1, #1
 *   02114328  strb r1, [r4, #0x3c8]
 *   0211432c  bl   #0x21141a8        ; r1 is still 1
 *
 * mwcc emitted `mov r1,#1` for the strb one instruction earlier and the call
 * reuses it, so `func_ov030_021141a8(c, 1)` compiles to the SAME bytes as the
 * one-argument spelling and the decompiler had no way to tell them apart. On
 * the host, cdecl, the callee would read whatever the stack happened to hold
 * and index data_ov030_02115e0c with it -- an out-of-range cell pointer written
 * into the object at +0x3a4, dispatched through on the very next enter.
 *
 * THIS IS THE ONLY DROPPED-ARGUMENT SITE IN THE OVERLAY, and that is a census
 * rather than an impression: func_ov030_021141a8 has 31 arm_call relocations
 * naming it, 30 of the src call sites pass two arguments (values 0..9 and
 * *(int *)(c + 0x3b8)), and this one is the single exception.
 *
 * EVERYTHING ELSE IS TRANSCRIBED STATEMENT FOR STATEMENT. Both DELTAs are
 * marked inline below.
 *
 * THE SYMBOL IS DELIBERATELY NOT _ZN13RollingLogTtm8BehaviorEv, and that is a
 * DEPARTURE from the usual host-copy convention with two reasons, the second of
 * them a live landmine for another lane.
 *
 * 1. THE RATCHET. src/__sinit_ov029_02112c10.c:5 declares
 *    `extern int _ZN13RollingLogTtm8BehaviorEv();` -- no parameters -- and a
 *    host definition under that exact name makes aritycheck's RECEIVER RATCHET
 *    fire a new row. That ratchet may only shrink, and the declaring file is in
 *    src/, which this lane does not edit. Measured against the base tree
 *    f77f01169: run_checks already fails there on one pre-existing receiver row
 *    (_ZN5Model6RenderEPK7Vector3, 166 baselined / 167 live / 1 NEW) and one
 *    vtspan row (port_scene_fill_vs), and this seat added EXACTLY ONE row on
 *    top. Renaming leaves the ratchet exactly where the lane found it.
 *
 * 2. THE LANDMINE, which is the better reason. ov029 and ov030 SHARE the level
 *    overlay load window at base 0x021111a0, and dsd resolved a word in ov029's
 *    own .init to the ov030 symbol that happens to sit at that address. The
 *    ov029 sinit does not CALL it -- it hands the address to func_020731dc as a
 *    SharedFilePtr destructor callback for data_ov029_02114270. Nothing builds
 *    that TU today (ov029 is unseated; it is wave 1 lane W1-C's target). The
 *    moment it IS built, a host definition under the Itanium name would bind
 *    ov029's destructor callback to THE UKIKI'S BEHAVIOR -- the Coffin /
 *    Spindrift wrong-object shape, which fails at no link and shows in no map.
 *    Not defining the name keeps that binding impossible and leaves the ov029
 *    lane a loud unresolved external instead of a silent wrong call.
 *
 * hal/actor_classes_ov030.cpp's slot-6 thunk calls this name directly, so the
 * vtable seat is unaffected. Nothing else in the tree references the Itanium
 * spelling except the ov029 sinit above and the matched src file itself.
 */
#include "types.h"
#include "ModelAnim.h"

struct Vector3;
struct Vector3_16;

extern "C" {
int _ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(void *thiz, int d);
void *_ZN5Actor13ClosestPlayerEv(void *thiz);
void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 a, u32 b, const Vector3 *c,
                                                   const Vector3_16 *d, int e, int f);
int _ZN8SaveData16HasPlayerLostCapEv(void);
void func_ov030_021141a8(void *c, int idx);
void func_ov030_02114134(void *c);
void func_ov030_02112094(void *c);
void func_ov030_02111734(char *c);
int port_ov030_ukiki_behavior(void *self);
}

/* PORT_HOST_ABI: mwcc virtual-shadow dispatch (ROM vtable numbering vs MSVC's)
   on a ModelAnim sub-object; the ROM's slot 3 is UpdateVerts and the host's
   slot 3 is Virtual10, which over-pops four bytes. */
extern "C" int port_ov030_ukiki_behavior(void *self)
{
    char *c = (char *)self;
    if (_ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(self, 0x5dc000) != 0 &&
        *(int *)(c + 0x3b4) != 8) {
        int b = (*(unsigned short *)(c + 0xc) == 0x10b);
        if (b != 0 && *(unsigned char *)(c + 0x3c8) == 0 &&
            _ZN8SaveData16HasPlayerLostCapEv() != 0) {
            void *pl = _ZN5Actor13ClosestPlayerEv(self);
            unsigned cp = *(unsigned *)((char *)pl + 8);
            if (cp < 3) {
                void *spawned;
                *(int *)(c + 0x3b0) = (int)cp;
                spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                    0x10d,
                    ((unsigned)*(int *)(c + 0x3b0) << 8) | 2,
                    (const Vector3 *)(c + 0x5c),
                    (const Vector3_16 *)0,
                    (int)*(signed char *)(c + 0xcc),
                    -1);
                *(int *)(c + 0x3ac) = *(int *)((char *)spawned + 4);
                *(unsigned char *)(c + 0x3c8) = 1;
                func_ov030_021141a8(self, 1);   /* DELTA: SHORT-1, see header */
            }
        }
        func_ov030_02111734(c);
    } else {
        func_ov030_02114134(self);
        ((ModelAnim *)(c + 0xd4))->UpdateVerts();   /* DELTA: was ->v3() */
        func_ov030_02112094(self);
    }
    return 1;
}
