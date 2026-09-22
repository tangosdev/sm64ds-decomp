/* HOST COPIES of two ov096 TORNADO (actor 308, daTor_c) sites MSVC cannot
 * deliver from the matched source, run rel0215 wave 2 lane cast-lvl16pair.
 * Both matched TUs stay byte-locked in src/ and are simply not referenced from
 * port/slice_ov096.txt. Pokey_HostSites.cpp header carries the long form of
 * the destructor derivation; only what is DIFFERENT about Tornado is written
 * out here.
 *
 * (1) src/_ZN7TornadoD1Ev.cpp -- THE C++-MANGLING WALL, the same shape as
 *     Pokey. The ROM says TornadoD1 makes exactly TornadoD0 calls in
 *     TornadoD0 order, minus the final Memory::Deallocate and its
 *     data_020a0eac argument (config/arm9/overlays/ov096/relocs.txt):
 *
 *       TornadoD1 0x02136db0..0x02136df8   TornadoD0 0x02136df8..0x02136e54
 *         0x02136dc4 -> 0x0201592c           0x02136e0c -> 0x0201592c
 *         0x02136dcc -> 0x0201691c           0x02136e14 -> 0x0201691c
 *         0x02136dd4 -> 0x020373f8           0x02136e1c -> 0x020373f8
 *         0x02136ddc -> 0x020149a4           0x02136e24 -> 0x020149a4
 *         0x02136de4 -> 0x020112c8           0x02136e2c -> 0x020112c8
 *         0x02136df4 (load) 0x02137a90       0x02136e3c -> 0x0203c1e8  <- only
 *                                            0x02136e4c (load) 0x02137a90
 *                                            0x02136e50 (load) 0x020a0eac
 *
 *     Resolved against src/_ZN7TornadoD0Ev.cpp own member list:
 *     TextureTransformer +0x328 (0x0201592c), ModelAnim +0x2c4 (0x0201691c),
 *     WithMeshClsn +0x108 (0x020373f8), MovingCylinderClsn +0xd4 (0x020149a4),
 *     then _ZN8dActor_cD2Ev (0x020112c8). THE LAST THREE ARE THE SAME THREE arm9
 *     ADDRESSES POKEY DESTRUCTOR USES for the same three member types, from a
 *     separately recovered destructor -- the cross-check that makes this a
 *     reading rather than a guess.
 *
 *     The load at 0x02136df4 reads 0x02137a90, the one Tornado table. src
 *     spells it two ways -- daTor_c_classInit says _ZTV7Tornado and D0 says
 *     _ZTV7daTor_c -- and port/ov096_syms.txt proves they are one table by
 *     reading its own vtable[-1] typeinfo through to the string "7daTor_c".
 *     This copy stores the host array _ZTV7Tornado; the /alternatename bridge
 *     in hal/actor_classes_ov096.cpp carries the other spelling to it.
 *
 * (2) src/_ZN7Tornado6RenderEv.cpp -- THE ModelAnim SLOT-5 COLLISION.
 *     hal/cxxname_bridge.cpp fills _ZTV9ModelAnim in MSVC numbering, where
 *     slot 5 is Virtual18 (two arguments) and NOT Render, and it cannot be
 *     dual-filled because something real occupies the slot. The matched body
 *     is a TextureTransformer::Update over the model components at +0x2cc
 *     followed by `((Sub *)(c + 0x2c4))->g5(c + 0x80)` through a six-virtual
 *     local shadow that counts in the ROM numbering. +0x2c4 is a ModelAnim,
 *     named as one by src/_ZN7TornadoD1Ev.cpp own member list and by
 *     Tornado::InitResources, which calls ModelAnim::SetAnim on it. The
 *     Whomp/Fish/Butterfly/HootTheOwl/ov077 case.
 *
 *     POKEY RENDER IS THE OPPOSITE CASE AND IS NOT REFUSED. It is the same
 *     six-virtual shadow reaching index 5, but over a plain Model at +0xd4,
 *     which hal/cxxname_bridge.cpp:511 dual-fills for exactly this shape. Two
 *     Renders in one overlay, one refused and one not, entirely on which
 *     member they dispatch. See port/slice_ov096.txt.
 *
 *     The matched TU is already `extern "C"` and defines the flat name itself,
 *     so this copy needs no face beside it -- unlike the seven real C++
 *     methods that stay in the slice.
 */
#include "ModelAnim.h"
#include "Tornado.h"

extern "C" {

/* ---- (1) the destructor ------------------------------------------------ */
extern int _ZTV7Tornado[];
void _ZN18TextureTransformerD1Ev(void *);    /* 0x0201592c, member +0x328 */
void _ZN9ModelAnimD1Ev(void *);              /* 0x0201691c, member +0x2c4 */
void _ZN10dBgCh_ActrD1Ev(void *);          /* 0x020373f8, member +0x108 */
void _ZN7dCcAc_cD1Ev(void *);    /* 0x020149a4, member +0xd4  */
void _ZN8dActor_cD2Ev(void *);                  /* 0x020112c8               */

/* PORT_HOST_ABI: mwcc Itanium D1 that MSVC emits no flat name for. */
int *_ZN7TornadoD1Ev(int *t)
{
    t[0] = (int)(size_t)_ZTV7Tornado;
    _ZN18TextureTransformerD1Ev((char *)t + 0x328);
    _ZN9ModelAnimD1Ev((char *)t + 0x2c4);
    _ZN10dBgCh_ActrD1Ev((char *)t + 0x108);
    _ZN7dCcAc_cD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    return t;
}

/* ---- (2) Render -------------------------------------------------------- */
int _ZN18TextureTransformer6UpdateER15ModelComponents(void *, void *);

/* _ZN7Tornado6RenderEv RETIRED (run link100, lane EXCEPT). Its stated reason -- the
   ROM-order model slot-5 dispatch -- died with lane SLOT5F's
   respelling of include/ModelBase.h: hal/cxxname_bridge.cpp:522/578
   put Render back on index 5 of _ZTV5Model and _ZTV9ModelAnim, so the
   matched source's local six-virtual shadow reaches the body it means.
   The C name is defined in hal/except_faces.cpp onto the matched
   __thiscall method; the ROM vtable word and the kind:function record
   are in port/slice_except2.txt. */


/* ---- (3) Tornado::Behavior -- HOST BODY RETIRED, run link100 wave 15 -----
 * SEATED: src/_ZN7Tornado8BehaviorEv.cpp is on port/slice_l15ov7.txt and the
 * ROM body runs.
 *
 * WHY THE OLD REFUSAL NO LONGER HOLDS. The banner this replaces stood on the
 * ROM carrying this method's receiver through r0 into three zero-argument
 * state calls (Tornado::Behavior's own prologue at 0x02137448 never reloads
 * r0 across the three `bl`s at 0x02137474/7c/84, so each callee read `this`
 * straight out of the entry register), while the matched TU called
 * State0/State1/State2 with no arguments and each body wanted a receiver.
 * BOTH HALVES ARE GONE: src/_ZN7Tornado8BehaviorEv.cpp is now
 * `int Tornado::Behavior()`, calling all three as ordinary members at
 * :14..:16, so no receiver rides r0 any more. The three callees were always
 * correct and stay in port/slice_ov096.txt; only this caller was ever wrong,
 * and it is the caller the sync fixed.
 *
 * THE BRIDGE IS WHAT STAYS, and it lives in the ledger, not here. mwccarm
 * mangles the member Itanium (_ZN7Tornado8BehaviorEv); MSVC mangles it
 * ?Behavior@Tornado@@UAEHXZ; and hal/actor_classes_ov096.cpp's tor_behavior
 * thunk (the host vtable fill for _ZTV7Tornado slot 6) calls the FLAT name
 * directly. Seating the TU therefore needed a face onto the flat name, not a
 * deletion of the row that named it: port/faces_sync.txt:3231 carried
 *     __ZN7Tornado8BehaviorEv   ?Behavior@Tornado@@UAEHXZ   0x02137448 F
 * a FORWARD row, which made facegen define the DECORATED name as a wrapper
 * onto this host body -- and the seated TU defines that same decorated name
 * itself, being now a real C++ member, so the two collided:
 *     faces_sync_gen.cpp.obj : error LNK2005: "public: virtual int __thiscall
 *     Tornado::Behavior(void)" (?Behavior@Tornado@@UAEHXZ) already defined in
 *     _ZN7Tornado8BehaviorEv.cpp.obj
 * Same shape as SHADOWS3's Flamethrower::Behavior seat (git 62fe1fd00): the
 * row flips F -> R, so facegen defines the FLAT name as
 * ((Tornado *)self)->Tornado::Behavior() instead, which is what tor_behavior
 * calls, and the matched TU alone defines the decorated name. Row now reads
 *     __ZN7Tornado8BehaviorEv   ?Behavior@Tornado@@UAEHXZ   0x02137448 R
 * and nothing else in the link defines the decorated spelling, so it cannot
 * duplicate.
 */

/* ---- (4) Tornado::State1 -- HOST BODY RETIRED, run link100 wave 15 -------
 * SEATED: src/_ZN7Tornado6State1Ev.cpp is on port/slice_l15ov7.txt and the ROM
 * body runs. Only the C-name bridge below is left here.
 *
 * WHY THE OLD REFUSAL NO LONGER HOLDS, re-read against the tree at 8ddff3187.
 * The banner this replaces said the TU could not compile at all: it declared
 * its own body `void _ZN7Tornado6State1Ev(char *c);` inside an `extern "C"`
 * block while include/decl_common.h declared the same C-linkage name
 * `extern void _ZN7Tornado6State1Ev(void);` at its line 2928, which is MSVC
 * error C2733. BOTH HALVES OF THAT ARE GONE. include/decl_common.h carries
 * exactly one Tornado line today -- `extern int _ZTV7Tornado[];` at :712 --
 * and no _ZN7Tornado6State1Ev declaration anywhere, so the collision has no
 * second declaration to collide with; and the matched TU is now
 * `void Tornado::State1()`, a real C++ member that declares
 * func_ov002_020de328 itself at :7 and calls UpdateSpin as a member at :80,
 * which closes the second reason the old banner named ("this body needs
 * func_ov002_020de328 and _ZN7Tornado10UpdateSpinEi FROM it and declares
 * neither itself").
 *
 * THE BRIDGE IS WHAT STAYS. mwccarm mangles the member Itanium
 * (_ZN7Tornado6State1Ev); MSVC mangles it ?State1@Tornado@@QAEXXZ; and the
 * host Tornado::Behavior above still dispatches its three states under the
 * flat names, because that row is NOT seated in this lane -- see the note on
 * it below. One face carries the flat name into the ROM body, the same
 * ((Cls *)self)->Cls::meth() shape port/faces_sync.txt already generates for
 * ?State0@Tornado@@QAEXXZ and ?State2@Tornado@@QAEXXZ at :967-968, and nothing
 * else in the link defines the decorated spelling, so it cannot duplicate.
 *
 * TORNADO::BEHAVIOR WAS TESTED IN THE SAME LANE AND IS NOT SEATED, although
 * its refusal is equally stale (src/_ZN7Tornado8BehaviorEv.cpp is
 * `int Tornado::Behavior()` and calls State0/State1/State2 as members at
 * :14..:16, so no receiver rides r0 any more). Seating it is a compile
 * failure THAT THIS FILE CANNOT FIX: port/faces_sync.txt:3231 carries the
 * forward row
 *     __ZN7Tornado8BehaviorEv   ?Behavior@Tornado@@UAEHXZ   0x02137448 F
 * so facegen emits a definition of ?Behavior@Tornado@@UAEHXZ, and the seated
 * TU defines it too:
 *     faces_sync_gen.cpp.obj : error LNK2005: "public: virtual int __thiscall
 *     Tornado::Behavior(void)" (?Behavior@Tornado@@UAEHXZ) already defined in
 *     _ZN7Tornado8BehaviorEv.cpp.obj
 * That F row exists only because nothing else defined the decorated name --
 * the ledger says so in its own words at :3219-3229 ("nothing references the
 * decorated spellings today ... they make Tornado D0 and D1 forwardable").
 * Deleting that ONE LINE retires the host Behavior body in section (3) above
 * and returns that TU too. It is
 * outside this lane's owned files, so the row stops here as a written finding.
 */
void _ZN7Tornado6State1Ev(void *cv)
{
    ((Tornado *)cv)->Tornado::State1();
}

/* ---- (5) THE WRONG-NAMED LEVEL-DATA REFERENCE -------------------------
 * src/_ZN7Tornado13InitResourcesEv.cpp hands its texture animation descriptor
 * to TextureTransformer twice, spelled `func_02112968`:
 *     TextureTransformer::Prepare(data_ov096_02137ba8[1], func_02112968);
 *     TextureTransformer::SetFile(this + 0x328, func_02112968, 0, 0x1000, 0);
 * and include/decl_common.h:1380 declares it `extern int func_02112968[];`.
 * THERE IS NO func_02112968 ANYWHERE IN config/ -- the name is a recovery
 * artifact. 0x02112968 is LEVEL-OVERLAY data, and ov096's own relocation says
 * so and says how badly:
 *     from:0x021376b0 kind:load to:0x02112968
 *         module:overlays(10,18,22,24,25,26,32,35,45,56)
 * TEN level overlays have a symbol at that address, every one of them marked
 * `ambiguous`, because on the DS the tornado reads its descriptor out of
 * WHICHEVER LEVEL OVERLAY IS RESIDENT. The ov017 ShipWater trap
 * (data_ov055_02111a94 / data_ov056_02111a60) at ten-way scale.
 *
 * THE BYTES SAY IT IS A FILE DESCRIPTOR, and the identification is by SHAPE
 * against a known one rather than by the name. ov024's image at 0x02112968:
 *     00000028 021118b4 021118ac 02111948 00000002 021118bc
 * and ov017's descriptor at 0x02111a60, which the ov017 lane identified as
 * "the six-word file descriptor":
 *     0000012c 021114bc 021114b8 021114dc 00000001 021114c0
 * Same six-word shape, same [count][ptr][ptr][ptr][small][ptr] layout, every
 * pointer into its own overlay's .rodata.
 *
 * WHY port_ov024_at AND NOT A PER-SYMBOL MOUNT. The descriptor carries FOUR
 * relocated pointers into ov024's .rodata, none of which port/ov024_syms.txt
 * mounts (that list is a hand-picked seven), so adding data_ov024_02112968 to
 * it would host the record with four raw DS addresses in it -- the T2 shape,
 * and TextureTransformer would read a file header out of unmapped memory.
 * ov024 is ALSO whole-mounted in PORT_LEVEL_OVERLAYS (port/CMakeLists.txt:1436)
 * and `--whole` rebases the image's internal pointers, so the correctly-fixed
 * copy already exists in this build; port_ov024_at(ds) is the accessor
 * hal/level_boot.cpp already uses for exactly this. Nothing is added to
 * another lane's mount list and nothing new is hosted.
 *
 * THE LIMITATION, STATED RATHER THAN HIDDEN. Binding to ov024 is correct for
 * LEVEL 16, the only level this tree's census places TORNADO on. On the DS the
 * address would resolve to whatever level is loaded; here it always resolves
 * to Shifting Sand Land's. If a later lane finds actor 308 placed on any of
 * the other nine levels (2, 10, 14, 17, 18, 24, 27, 37, 48), this call needs a
 * level-keyed dispatch over the port_ovNNN_at family instead of one binding,
 * and this comment is the record of why.
 *
 * Every other statement is transcribed line for line from the matched body.
 */
void *port_ov024_at(unsigned ds);
int _ZN5Model8LoadFileER13SharedFilePtr(void *sfp);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, void *f, int a, int b);
int _ZN9Animation8LoadFileER13SharedFilePtr(void *sfp);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, void *f, int a,
                                                 int fix, unsigned u);
void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(void *a, void *b);
void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void *thiz, void *f,
                                                           int a, int fix,
                                                           unsigned u);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *thiz, void *actor,
                                                        int a, int b,
                                                        unsigned c, unsigned d);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *thiz, void *actor, int a, int b, void *v, int e);
void func_02016aac(void *self, unsigned value, int index);
extern void *data_ov096_02137ba8[];
extern void *data_ov096_02137bb0[];

/* PORT_HOST_ABI: a recovery-artifact name for level-overlay data, reached
   through the whole mount's rebased image instead. */
int _ZN7Tornado13InitResourcesEv(void *selfv)
{
    char *c = (char *)selfv;
    void *bta = port_ov024_at(0x02112968);

    _ZN9ModelBase7SetFileEP8BMD_Fileii(
        c + 0x2c4,
        (void *)(size_t)_ZN5Model8LoadFileER13SharedFilePtr(data_ov096_02137ba8),
        1, 0x15);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov096_02137bb0);
    func_02016aac(c + 0x2c4, 0x16, 1);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x2c4,
                                                data_ov096_02137bb0[1],
                                                0, 0x1000, 0);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(
        data_ov096_02137ba8[1], bta);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(c + 0x328, bta,
                                                          0, 0x1000, 0);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(c + 0xd4, c, 0, 0,
                                                       0x200002, 0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        c + 0x108, c, 0x50000, 0x50000, 0, 0);

    *(int *)(c + 0x340) = *(int *)(c + 0x5c);   /* unk_340 = mPosX */
    *(int *)(c + 0x344) = *(int *)(c + 0x60);   /* unk_344 = mPosY */
    *(int *)(c + 0x348) = *(int *)(c + 0x64);   /* unk_348 = mPosZ */
    *(short *)(c + 0x35a) = 0;
    *(int *)(c + 0x35c) = 0;
    *(unsigned char *)(c + 0x360) = 0;
    *(int *)(c + 0x09c) = -0x1000;
    *(int *)(c + 0x0a0) = -0x1e000;
    {
        unsigned int t = (unsigned int)(*(int *)(c + 8)) & 0xff;   /* mParam */
        *(int *)(c + 0x34c) = (t == 0xff) ? 0x5dc000 : (int)(t * 0x64000);
    }
    *(unsigned *)(c + 0x364) = 0;
    *(unsigned *)(c + 0x368) = 0;
    return 1;
}

}  /* extern "C" */
