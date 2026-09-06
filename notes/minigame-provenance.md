# Minigame scene classes -- provenance

Where the recovered layout of the minigame headers came from. This file exists
so the headers themselves can read like source rather than like a lab notebook.
Nothing here is load-bearing for the byte match; the constraints that ARE
load-bearing stay as short comments in the headers.

## dScMiniGm_c (include/dScMiniGm_c.h)

One of dScene_c's ten direct children (census in dScene_c.h) -- the
minigame-select scene, per the ROM's own RTTI name. It had a flat
auto-generated header before the class pass, with no base clause; seven
resolvable vtable slots were blocked on it.

**Base.** dScene_c, confirmed two independent ways:

* RTTI -- dScMiniGm_c's `__si_class_type_info` (ov005:0x020c2448, name
  "dScMiniGm_c" at 0x020c2454) names its single base dScene_c (build/rtti.json).
* Vtable -- `_ZTV11dScMiniGm_c` (ov005:0x020c2490) is 18 slots, matching
  dScene_c's own 18 (`tools/rtti_vtables.py --own dScMiniGm_c`). It overrides
  exactly seven: 0, 3, 6, 9, 12, 16, 17. Every other slot is still whatever
  dScene_c's table holds. dScMiniGm_c adds no new virtual, and the destructor
  pair is slots 16/17, checked directly against the `_ZN8dScene_cD1Ev` /
  `_ZN8dScene_cD0Ev` anchors.

**Size.** `dScMiniGm_c_classInit` (historical alias `func_ov005_020c21ec`) is the factory; it opens with
`_ZN7fBase_cnwEj(0xb0)` -- fBase_c::operator new(0xb0). 0xb0 therefore comes
straight off the allocator call. The factory writes only the vtable chain
(fBase_c -> dScene_c inlined -> data_ov005_020c2490) and two spawn-flag bits at
fBase_c's own 0x13; it constructs no nested sub-object, matching the
plain-scalar field layout.

**Members.** The old flat header put every field it found at or above 0x54,
consistent with dScene_c adding no fields of its own, so nothing needed
re-attribution the way dScEntry_c's did. It was still incomplete: reading the
six slot functions directly turned up eight more fields (0x050, 0x058, 0x05c,
0x060, 0x064, 0x0a0, 0x0a4, 0x0a8) on top of the six it had.

Field names and the matched body that settles each:

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x050 | `mSubBgScrollX` | InitResources sets it to 0xb0 or 0 from `data_0209b304` (the page index), then feeds it to `SetSubBg0Offset` / `SetSubBg2Offset` / `SetSubBg3Offset` and to `*(u32*)0x400101c = mSubBgScrollX & 0x1ff`. |
| 0x054 | `mPageFlipped` | u8. Zeroed by InitResources; Behavior sets it to 1 on each of the two page-timer expiries. Render draws the left/right page arrows only while it is 0. |
| 0x058 | `mGroupBase` | InitResources seeds it from `data_0208a170`, the base index into the minigame table `data_ov005_020c24d8[]`. func_ov005_020c0878 copies it back into `data_0208a170` once `mScrollDelay` drains, i.e. it is the *target* group base and the global is the committed one. |
| 0x05c | `unk_05c` | Zeroed by InitResources; func_ov005_020c0378 copies it into `(&data_0209b308)[0x30]`. Role not settled -- left `unk_`. |
| 0x060 | `unk_060` | Zeroed by InitResources, no other matched access. |
| 0x064 | `unk_064` | Zeroed by InitResources, no other matched access. |
| 0x08c | `mArrowBobPhase` | Behavior free-runs it 0..0x3f; Render uses it as the horizontal bob of the two page arrows (`0x50 - t/2` and `t/2 + 0xE0`). |
| 0x090 | `mPrevPageTimer` | Behavior counts it down; expiry sets `data_0209b304 = 0` (page 0) and `mPageFlipped = 1`. Render drives the left arrow's scale pop from it. |
| 0x094 | `mNextPageTimer` | Same shape; expiry sets `data_0209b304 = 1`. Drives the right arrow. |
| 0x098 | `mExitTimer` | Behavior counts it down to 1, then `dScene_c::SetAndStopColorFader()`, `ExitMinigameMenu()`, stops the music and sets `mExiting`. |
| 0x09c | `mIconBlinkPhase` | Behavior free-runs it 0..0x3f; Render picks between two arrow frames on `>= 0x20`. |
| 0x0a0 | `mScrollDelay` | func_ov005_020c0878 decrements it, reloads it to 0x1e after committing a group change, and gates cursor input on `<= 0`. |
| 0x0a4 | `unk_0a4` | Zeroed by InitResources, no other matched access. |
| 0x0a8 | `unk_0a8` | Zeroed by InitResources, no other matched access. |
| 0x0ac | `mExiting` | u8. Behavior sets it on the exit branch; func_ov005_020c0878 / _020c0378 / _020c06cc all early-out while it is set. |

0x064..0x08c and 0x0ad..0xb0 are padding: no slot function touches them, and
`mExiting` plus three bytes of tail padding closes exactly on the 0xb0
allocation.

## dScMgBase_c (include/dScMgBase_c.h)

The banner credited the file to a `deepen_rtti.py` under `tools/`. That tool has
never existed in this repo (`git log --all --diff-filter=A -- '*deepen_rtti*'`
is empty; see notes/runbook-type-reconstruction.md section 2), so every field
that does not carry its own evidence note is an unverified placeholder rather
than machine-checked evidence.

**Base.** dScene_c, confirmed by `tools/rtti_extract.py`: dScMgBase_c's
`__si_class_type_info` at ov004:0x020bbf6c points at dScene_c arm9:0x020914d4,
offset 0 -- the same edge dScene_c.h's own census documents. dScMgBase_c is also
a second hierarchy root: 15 direct RTTI children, 32 transitive descendants (the
minigame family -- notes/dscene-c-siblings-census.md section 2). Its own fields
start at ROM offset 0x50 == sizeof(dScene_c), like every other dScene_c child.

**Destructor.** The cascade goes one more level than dScene_c's own fix.
dScMgBase_c has 32 descendants, so its D2/D1 would have to be defined inline for
them to inline it the way Stage inlines dScene_c's -- except this body has real
work in it (a global write, a function call) and mwcc does *not* inline it:
dScMgD3DBase_c's own D1 in the ROM (0x38 bytes) calls `_ZN11dScMgBase_cD2Ev` as
a real `bl`. Compiling dScMgD3DBase_c's destructor against an INLINE-defined
dScMgBase_c dtor produced exactly 0x84 bytes and `999 word(s) differ`. So the
destructor is declared here and defined for real in src/_ZN11dScMgBase_cD1Ev.cpp
and .../_D0Ev.cpp. The class's own `operator delete` copy is a separate matter
and must stay: mwcc only inlines a D0 route through the class itself or its
immediate base, and for dScMgBase_c's children that immediate base is
dScMgBase_c, not dScene_c.

**0x0f4 is hand-verified.** `_ZN11dScMgBase_cD2Ev` calls
`func_ov004_020b929c(this + 0xf4)`, which is
`__destroy_arr(p, count=8, elem_size=0x24, func_ov004_020b9280)`
(0x0207328c is `__destroy_arr` / `__cxa_vec_cleanup`,
config/arm9/symbols.txt:3050-3051). The per-element destructor writes two
vtables into `[r0+0]` back to back with no further calls -- 0x020bca7c then
0x020ad494 -- which build/rtti.json identifies as
`_ZTVN10dMgPsOpt_c11TouchIcon_cE` (ov004:0x020bca68, "dMgPsOpt_c::TouchIcon_c")
and `_ZTV9dThIcon_c` (ov001:0x020ad478, "dThIcon_c", a root class). So
TouchIcon_c : dThIcon_c, single inheritance at offset 0, both destructors
trivial enough to fully inline into vtable writes, and 0x0f4 is 8 contiguous
0x24-byte TouchIcon_c spanning 0x0f4..0x214. Neither class has a header yet, so
the array stays raw bytes and the destructor calls func_ov004_020b929c on it
explicitly, which is exactly what the ROM's D2 does. 0x214..0x21c has no matched
access and stays padding.

**data_ov004_020beb68** is a global singleton pointer to the active
dScMgBase_c; 60+ files across ov004/ov006 read it, each having invented its own
local type (Base*, Obj*, G*, char*, void*...). Retyping it tree-wide is its own
slice. The D2's `*(int*)data_ov004_020beb68 = 0` writes 0 into the *global's own
storage* -- that file declared it `extern int data_ov004_020beb68[]`, and arrays
decay to their own address -- i.e. a plain global pointer being nulled, not a
target zeroed through it.

**Slots 18-35** are eighteen further virtuals new at this class, the same shape
as dActor_c's 13 new slots over fBase_c. All 18 targets are already matched
source (func_ov004_* under arm9/ov004, notes/dscene-c-siblings-census.md
section 2), but their signatures are not reconstructed, so they stay undeclared
rather than guessed.

**The blink prompt (0x0c0 / 0x0c3 / 0x0c4).** func_ov004_020b0de0, called from
`dScMgBase_c::BeforeRender`, is the whole story: nothing draws unless
`mPromptEnabled` (0x0c3) is set; while `mPromptBlinkCount` (0x0c4) is below 4 the
16-bit `mPromptBlinkTimer` (0x0c0) free-runs 0..0x2f, bumping the count each
wrap, and the per-language prompt sprite
(`data_ov004_020bbfa8[GetGameLanguage()] + 0x28`) is drawn at (0xc0, 0xb0) only
during the first 0x18 frames of each cycle. Once four cycles have elapsed it is
drawn every frame. The width and signedness of 0x0c0 come from that function's
unsigned compares against 0x30 and 0x18; 0x0c4's from its `< 4U`.

Roughly 25 pre-existing ov006 files spell these three by raw offset:
dScMgFlower_c's, dScMgSnowball_c's and dScMgMCarlo_c's Behaviors all carry the
identical `if (0xc4 == 0) { 0xc3 = 1; 0xc4 = 1; *(s16*)0xc0 = 0; }` idiom, i.e.
"start the prompt blinking from the top". Naming them in the header is what lets
those files stop spelling raw offsets.

**The size assert is a floor, not a claim the object ends there.**
dScMgD3DBase_c needs a number to start its own fields at; 0x465c is the last
field any matched function has observed, and 0x4660 is its 4-byte-aligned round
up. If it were short, dScMgD3DBase_c's fields would land on the wrong bytes and
build_pin would catch it -- the safety net check_header_offsets.py's own
DATA_SIZE comment describes. Same reasoning for every other floor assert in the
family.

## dScMgSingle3DBase_c (include/dScMgSingle3DBase_c.h)

dScMgSingle3DBase_c : dScMgBase_c, confirmed by build/rtti.json (its
`__si_class_type_info` points at dScMgBase_c, offset 0). It is itself a
hierarchy root: 13 direct RTTI children -- the "single 3D minigame" family
(card, cup, memory x2, mahjong-carlo x2, roulette, slot3, sound, BSC, snowball,
flower, plus dScMg3DEsp_c). Its own fields start at ROM offset 0x4660 ==
sizeof(dScMgBase_c).

**Own vtable slots** (`tools/rtti_vtables.py --own dScMgSingle3DBase_c`): 2, 5,
7, 10 re-override slots dScMgBase_c already gave a body (AfterInitResources,
AfterCleanupResources, BeforeBehavior, BeforeRender); 16/17 are its own D1/D0;
26 and 33 are new overrides (an OnHitByCannonBlastedChar-shaped routine and a
VRAM/graphics-bank setup routine). The eight source files still carry an
auto-generated `recovered name: dScMgFlower_c_*` comment -- the same off-by-one
"recovered from vtable slot identity" mislabelling documented for dScMgBase_c's
siblings, where an arbitrary concrete descendant's name is borrowed for what is
really the base's own method. The vtable dump is the authority, not the comment.

**mSysTracker at 0x471c is hand-verified**, four independent witnesses agreeing
on the offset: this class's D1 and D0 both destroy it
(`_ZN8Particle10SysTrackerD1Ev((char*)c + 0x471c)`), AfterInitResources
initialises it, BeforeBehavior updates it conditionally. Particle::SysTracker is
declared locally rather than shared, for the reason Stage.h's own note gives
(two independent gen_header.py shadows, union gives 0x81c, no file here includes
either shadow header). This is a third local copy of the identical type;
consolidating all three is a separate change with its own blast radius.

**0x4700..0x4718** (seven fields) were split out of the former `pad_4660[0xbc]`:
dScMgRoulette_c's Render (_ZN15dScMgRoulette_c6RenderEv, in src/actors/dScMgRoulette_c.cpp) and dScMg3DEsp_c's Render
(src/_ZN12dScMg3DEsp_c6RenderEv.cpp) both write those exact offsets, so they belong to
this class, not either leaf. 0x4718..0x471b has no matched access and stays
padding.

**Their comments deliberately avoid the usual `/* 0xNN */` style.**
tools/check_header_offsets.py's DATA_SIZE precompute walks a struct's commented
fields by regex to find where a derived class's fields start, and that regex
cannot parse the namespaced `Particle::SysTracker mSysTracker` a few lines down
-- it silently stops at the last field it CAN parse. Before these seven fields
existed nothing in the struct matched the regex at all, so every dependent
derived header correctly fell back to the asserted `sizeof`. Giving the seven
the usual hex-comment style made the regex succeed partway and stop there,
undercounting every derived class's field offsets by nearly the whole SysTracker
member -- measured on dScMgRoulette_c.h, 22/22 fields "mismatched" by exactly
that delta. **Do not restyle those seven comments.**

**The destructor must stay defined inline.** Same fix and reason as
dScene_c.h's note: 13 direct children each inline this destructor's vptr store
plus mSysTracker destruction plus the chain to `~dScMgBase_c()`. Measured on
dScMgMemory_c: a merely declared `virtual ~dScMgSingle3DBase_c();` compiles a
derived destructor referencing `_ZN19dScMgSingle3DBase_cD2Ev` as an undefined
external, and no such symbol exists anywhere in the ROM. The raw pre-migration
destructors confirm it -- func_ov006_020f3834 (dScMgMemory_c's D1) destroys
`_ZN8Particle10SysTrackerD1Ev(c + 0x471c)` inline and then calls
`_ZN11dScMgBase_cD2Ev(c)` directly, with no dScMgSingle3DBase_c-specific
destructor call at all. The out-of-line definition that landed in #1421 was a
latent bug; #1421 never tested a real descendant. No separate `operator delete`
copy is needed here -- dScMgBase_c, the immediate base, already provides one,
and mwcc's inline-D0 route only needs to reach the immediate base.

`Particle::SysTracker::Initialise` / `::Update` and `Particle::RenderAll` are
declared in the header only so the calls can be spelled normally; they are
non-virtual and add neither a field nor a vtable slot. Before that they were
reached through `extern "C"` declarations of the mangled symbols at the call
sites, which is the same call the compiler emits from the declaration.

## cMgSmartball_ball_c (include/cMgSmartball_ball_c.h)

One of eleven direct children of cMgSmartball_object_c -- see that header for
the family's shape (a root, three slots, no virtual destructor). Everything
below 0x34 is reached through inherited members; this class's four functions
never touch the base's 0x31-0x33 region, so no raw cast is needed anywhere.
Field-by-field provenance (RTTI/vtable addresses, size, every offset's name or
pad determination, and the two out-of-scope helpers that lend several of the
names) is tabulated in full in `notes/smartball-provenance.md` -- this file no
longer keeps a separate copy.

## dScMgAmida_c (include/dScMgAmida_c.h)

dScMgAmida_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py (zero
RTTI edges name dScMgAmida_c as a base).

Own vtable slots (python tools/rtti_vtables.py --own dScMgAmida_c): 0
(InitResources), 5 (AfterCleanupResources -- the recovered source locally
declared the base override as returning void*, which is WRONG; the real
dScMgBase_c.h override returns void, so this now calls the base method as
a plain statement instead of returning it, same fix dScMgLuigi_c's own
slot 5 needed), 6 (Behavior), 9 (Render), 16 (D1), 17 (D0), 18
(dScMgBase_c::OnYoshiTryEat, declared on the base and overridden here;
the body is src/_ZN12dScMgAmida_c13OnYoshiTryEatEi.cpp, still a raw
extern "C" helper rather than a member definition, same precedent as
every other dScMgBase_c leaf's slot 18; it no longer includes this
header at all -- its one
inherited-field access at 0xbc is dScMgBase_c's own pad_0bc, not a named
field there either, so it now reaches it via a raw char* offset, the same
idiom dScMgPachinko_c's own slot 18 helper already uses), 31 (now
`Virtual7C`, src/_ZN12dScMgAmida_c9Virtual7CEv.cpp -- takes no parameters
and never touches `this` at all, pure hardware-register/global reset; it
was left as a raw helper by THIS migration and picked up later, when the
slot-31 keystone commit named the base slot), 34 (now `Virtual88`,
src/_ZN12dScMgAmida_c9Virtual88Eiiii.cpp -- the slot's signature is
`void(int, int, int, int)`, measured from the seven call sites in
ov004:0x020ae5c4, and this body reads only three of the four because the
fourth arrives on the stack and it supplies its own size instead; it is
the collision half of the ghost-leg rule, probing the 0x158-stride
occupancy grid at +0x4710 and then delegating the drawing to
dScMgBase_c's brush, which it is the only override in the family to
call.  The guess recorded here -- "draws a HUD digit/glyph" -- and the
matching one in include/dScMgTeresa_c.h were both pointing at
dScMgBase_c's slot 34 being a multi-argument virtual; it is declared and
reconstructed tree-wide now.  Left as a raw helper by THIS migration and
picked up later, when the slot-34 keystone commit named the base slot --
it never includes this header either).

rtti_vtables.py --own ALSO reports slot 35, now `Virtual8C`
(src/_ZN12dScMgAmida_c9Virtual8CEv.cpp, a one-line
`((*(int*)((char*)c+8))&0xff)==1` check, same shape as slot 36 below but
for a different constant).  The observation recorded here -- that nothing
in InitResources/AfterCleanupResources/Behavior/Render/D1/D0 calls it, and
that there is no cross-reference anywhere in src/ besides its own file --
is correct and is why it was left alone through this migration.  It is
also why "nothing calls it" was the wrong conclusion to draw: the slot is
dispatched thirteen times in ov006, virtually, so no call site names the
symbol.  All thirteen are in FOUR OTHER leaf classes (dScMgCoin_c,
dScMgPanel_c, dScMgSound_c, dScMgSnowball_c), each asking the question of
itself.  The word being tested is fBase_c::param1 at +0x08; the base
answers `!= 0` and this class narrows it to `== 1`.  Declared and renamed
by the slot-35 keystone commit, which closed the 18-35 range.

SLOT 36 IS DIFFERENT FROM EVERY SIBLING'S SLOT 18+: it is a brand-new
own slot (dScMgBase_c's own vtable is 36 slots, 0-35; Amida's is 37,
0-36) that IS genuinely called, virtually, from three of this class's
own real methods (InitResources, Behavior, Render), each through its own
hand-rolled vtable-shim struct (`Obj::v90()` / `VtObj::m36()` /
`Base::m_90()` -- the shim's exact shape varies per pre-migration file,
all landing on the same byte offset, vtable_ptr+0x90 = 36*4). Declared
here as a real virtual method (`Unk36`, a placeholder name -- its true
ROM identity is not tree-wide reconstructed) -- but MEASURED, not
assumed, that calling it as a normal `this->Unk36()` does NOT reproduce
the ROM: it compiled Render 0xc bytes larger (0x2ac vs 0x2a0), because
mwcc's own compiled slot for Unk36 (appended after dScMgBase_c's
compiler-visible virtuals, which stop at dScene_c/fBase_c's own count --
dScMgBase_c.h leaves slots 18-35 undeclared) does NOT land on true ROM
slot 36 the way it accidentally does for a class's first-and-only new
slot when that slot is 18 (dScMgCoin_c's OnYoshiTryEat, etc.) -- Amida's
slot 36 is 18 slots further out, past all of dScMgBase_c's undeclared
ones, so the coincidence that makes slot 18 "just work" does not extend
here. That single size delta cascaded through the rest of the module
(dsd does not hard-fail a declared-vs-compiled size mismatch, ~1400
unrelated-looking functions afterward all mismatched) -- caught by
rombuild, traced via final_link.o.xMAP. So all three call sites keep the
pre-migration source's exact vtable-shim dispatch (see each method's own
file), even though Unk36 is declared as a real virtual method here --
unlike dScMgSlot1_c's OnHitByMegaChar/OnHitFromUnderneath, whose
compiler-computed slot also doesn't match their true ROM position 27/28
but is harmless there because neither is called from within Slot1's own
migrated methods.

THE DESTRUCTOR IS NON-TRIVIAL: unlike most siblings, this class explicitly
destroys FOUR arrays via __destroy_arr, in this exact order, in BOTH D1
and D0 (src/_ZN12dScMgAmida_cD1Ev.cpp and .../_D0Ev.cpp carry an identical
body, same shape dScMgHanachan_c's own D1/D0 pair uses): the 0x80x0x18
dScMgAmida_c_Piece array at 0x4768 (own per-element dtor
func_ov006_020d116c, a no-op -- the element type needs no real cleanup),
then the three NullDestructor_0203d47c-based 4x8-byte arrays at 0x4744,
0x4724, and 0x4660 in that order (their own per-element dtor is also a
no-op). The base-D2 call and own-vtable-write are compiler generated;
D0's extra Memory::Deallocate comes for free from dScMgBase_c's own
operator delete (its immediate base), no per-class copy needed. The
tree-wide OnYoshiTryEat mislabel lands on THIS class's D0 too (its source
comment said "recovered name: dScMgAmida_c_OnYoshiTryEat") -- same
textbook D0 shape (vtable write, four array destroys, base D2,
Deallocate) as every other mislabeled D0 in the family, not gameplay
logic.

arr4724/arr4744 (0x4724, 0x4744): two more of the three
NullDestructor_0203d47c-based 4x8-byte arrays the destructor tears down.
Unlike 0x4660 (never touched individually -- see below), BOTH are
genuinely field-accessed, not just passed to a helper: Behavior reads and
writes both elements' two 4-byte sub-fields directly (indexed 0..3, not
just element 0), and Render reads arr4724's same two sub-fields as
fixed-point (>>12) screen coordinates -- so both get a real {x,y}-shaped
layout (a 2-D int array reproduces the true interleaved 8-byte stride
exactly) instead of raw bytes. arr4744's own two fields are added into
arr4724's and one of them is decremented by a fixed step every Behavior
tick, consistent with a decaying delta/velocity feeding a position --
named accordingly, though the exact game-side meaning (this is the
Amida/ghost-leg-lottery minigame) is not otherwise confirmed. 0x4660,
the third such array, is NEVER touched individually by any of the six
real methods (only its base address is threaded through the generic
construct/destroy helpers), so it stays raw bytes, matching the
guidance that only genuinely field-accessed ranges get a granular
layout.

unk_470c/unk_4710: two 0x15800-byte buffers, confirmed in both
directions -- Memory::Allocate'd in InitResources, Memory::Deallocate'd
in AfterCleanupResources (only when vfSuccess==2; the pre-migration
source's early `if (r1!=2) return c` means the base-class call is ALSO
skipped in that case, preserved exactly). Typed as pointers, not s32,
because slot 34's own raw Ctx-shaped view of this class (not migrated,
see above) indexes through both of them as byte buffers
(`ctx->unk470c + y*0x158`), and AfterCleanupResources itself reads them
through a `*(void**)` cast.

Fields below dScMgBase_c's own last-observed field (mSceneKind at 0x465e,
DATA_SIZE 0x4660 per tools/check_header_offsets.py's own convention -- see
dScMgBase_c.h) are genuinely THIS class's own, drawn only from what
InitResources/AfterCleanupResources/Behavior/Render/D1/D0 directly touch.
Fields touched only by the raw slot 18/31/34 helpers (0x46d8..0x4700's
digit-drawing state, 0x4704..0x470b's flag bytes, and dScMgBase_c's own
inherited 0xb4/0xbc slot 18 touches) are NOT represented here -- they
stay inside a pad, same discipline as every prior sibling. The class's
true allocation size, 0x53fc, is independently confirmed (not just the
last-named-field's rounded end) by the untouched constructor's own
`_ZN7fBase_cnwEj(0x53fc)` call (src/d_s_mg_amida.cpp), which
also independently corroborates all four destroyed-array offsets/sizes
via its own construction-side func_020733a8 calls.

### The 0x4768 piece array

dScMgAmida_c's own 0x80-element array at 0x4768 (destroyed via
__destroy_arr in D1/D0, see the class banner below). Each 0x18-byte
element is walked with individual field accesses in Behavior AND
Render, not passed opaquely to a helper, so it gets a real named shape
instead of raw bytes: position (+0x00/+0x04) accumulates velocity
(+0x08/+0x0c) once per Behavior tick; Vec2_Len/func_0203d434/
func_0203d630 read and rescale the velocity; a per-element timer
(+0x10) counts up and gates the u8 active flag (+0x14) off past a
threshold; Render reads position/timer/active directly for fixed-point
(>>12) screen-space drawing. The trailing 3 bytes (+0x15..+0x17) are
plain struct tail padding to reach the observed 0x18 stride -- nothing
ever reads or writes them, so they are not modelled as a field.

### The __destroy_arr declarations

__destroy_arr / func_ov006_020d116c / NullDestructor_0203d47c: the same
__destroy_arr(p, count, elemSize, dtor) idiom dScMgBase_c's own D1/D0 use
for touchIcon_0f4 (see dScMgBase_c.h's file banner and
src/_ZN11dScMgBase_cD1Ev.cpp) -- declared here, not per-destructor-file,
matching dScMgHanachan_c.h's own placement.

---

# Round-3 readability pass: field names

The `dScMg*` family's `unk_NNN` fields, named from what the matched bodies do
with them. One table per class; every row cites the file that settles it. Where
no matched body does anything characteristic with a field it keeps its `unk_`
name -- a wrong name is a claim the next reader will trust.

## dScMgAmida_c field names

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x46d0 | `mState` | The subject of `Behavior`'s own `switch` (src/_ZN12dScMgAmida_c8BehaviorEv.cpp): 0 sets the board up and falls into 1, 1 runs the lottery, 2 waits out the result, 3 is the finale. src/func_ov006_020d3ba0.cpp leaves it at 1. |
| 0x46d4 | `mFinished` | u8. src/func_ov006_020d3ba0.cpp zeroes it; Behavior sets it only on the branch that fires when `mRoundCount` reaches 5, and every later read takes the celebration path (`func_ov004_020b0a54(0)` instead of `0x12`, and Render's confetti pass). |
| 0x4700 | `mLineEndY` | InitResources stores 0x78 or 0x98 here; src/func_ov006_020d3ba0.cpp passes it as the y2 argument of four `func_ov004_020ae5c4` line draws whose other three arguments are literal screen coordinates (x = 0x20/0x60/0xa0/0xe0, y1 = -0xb4 or -0xd4). |
| 0x4724 | `mLanePos[4][2]` | src/func_ov006_020d3ba0.cpp seeds `{ (0x20 + 0x40*i) << 12, 0xb0 << 12 }`; Behavior adds `mLaneVel` into it; Render draws the lane sprite at `>> 12`. |
| 0x4744 | `mLaneVel[4][2]` | Added into `mLanePos` once a tick, and its y component loses a fixed 0x100 every tick -- a velocity under gravity. Zeroed by the same reset. |
| 0x4768 | `mPieces[0x80]` | Renamed from `arr4768`; the element layout is unchanged (see the section above). |
| 0x5368 | `mScrollSpeed` | src/func_ov006_020d3ba0.cpp computes it from the pattern table `data_ov006_0212e1b0 + mPatternIndex * 0x1c`, biases it by the inherited 0xbc, and clamps it to 0x64. |
| 0x536c | `mScrollAccum` | Behavior adds `mScrollSpeed` into it, keeps the low four bits (`&= 0xf`) and runs `func_ov006_020d27dc` once per 16 accumulated -- a fixed-point step accumulator. |
| 0x5374 | `mRoundCount` | Zeroed by the reset; Behavior replays the board while it is below 5 and finishes at 5, and scales the fast-forward speed by `n * 5 + 0x20`. |
| 0x539c | `mLaneAnimTimer[4]` | Render bumps entry `i` each frame and wraps it on the per-lane period it copies out of `data_ov006_0213b880`. |
| 0x53ac | `mLaneAnimFrame[4]` | Bumped when the timer above wraps, cycles 0..0xd, and indexes the sprite table `data_ov006_0213a458`. |
| 0x53bc | `mBgScrollPhase` | u16. Render adds 0xc0 a frame and feeds `>> 4` into the shared sine table `data_02082214` to get the sub-screen BG2 offset. The 16-bit width comes from the reset's own `*(s16*)` store. |
| 0x53c0 | `mResultWaitTimer` | Loaded with 0x3c on entry to state 2 and counted down there; at 0 the scene clears `mPromptEnabled` and moves to state 3. |
| 0x53c4 | `mStartBannerTimer` | Reset to 0x3c right after `func_ov004_020b0cac(0xd, 0x80, 0x60, ...)` puts banner 0xd on screen; Behavior counts it down and calls `FreeGfxSlotsById(0xd)` on expiry. |
| 0x53d0 | `mEndDelayTimer` | Set to 0xb4 when state 3 begins; Render keeps drawing the play field until it and `mState == 3` agree, then switches to the finale. |
| 0x53d4 | `mPatternIndex` | src/func_ov006_020d3ba0.cpp picks it (clamped, or randomised for the harder variant) and then uses it as the row index into five different 0x1c-stride tables in ov006. |
| 0x53e0 | `mRoundTimer` | Behavior counts it down inside state 1; reaching 0 is what ends the round and chooses between another board and the finale. |
| 0x53e8 | `mScore` | InitResources seeds it from the inherited 0xbc times 5; src/func_ov006_020d3ba0.cpp clamps it to 0x270f (9999); Behavior pushes it to the HUD counter `func_ov004_020adb1c` every tick. |

Left `unk_`: 0x46d5 (a second reset flag, only ever zeroed and compared against
1), 0x470c/0x4710 (two 0x100 x 0x158 byte buffers -- the shape is now in the
header comment, the role is not settled), 0x4714 (a per-lane gate read by both
Behavior and Render, but nothing in scope writes it), 0x4764, 0x53dc/0x53dd
(two speed-modifier flags), 0x53e4 (a constant 2 handed to every line draw).

## dScMgSnowball_c field names

The slalom's own state lives in two clusters: a movement block at 0xab38 that
the previous header held as four pads, and a run/HUD block at 0xb9d8.

| Offset | Name | Evidence |
| --- | --- | --- |
| 0xab38 | `mPosX` / `mPosY` (0xab3c) | src/func_ov006_021279b0.cpp seeds `mPosX = 0x80000` and `mPosY = mStartY << 12`; Behavior adds the velocity into both; Render's progress bar reads `mPosY >> 12`. |
| 0xab40 | `mPrevPosX` / `mPrevPosY` (0xab44) | Behavior's first act each tick is to copy 0xab38/0xab3c here, and the climb term is `mPosY - mPrevPosY`. |
| 0xab48 | `mDrawPosX` / `mDrawPosY` (0xab4c) | Render draws the ball sprite `data_ov006_02139c38` at `(n - mScrollX) >> 12`, the same transform every world object gets. |
| 0xab50 | `mSoundPosX` / `mSoundPosY` (0xab54) | Behavior fires a rolling sound whenever the position has moved 0x30000 from these two, then copies the position in. |
| 0xab60 | `mVelX` / `mVelY` (0xab64) | `Vec2_Len` of the pair is the speed, `atan2` of it is the heading, and it is added into `mPos` each tick. Capped at 0x8000. |
| 0xab68 | `mScrollX` | Subtracted from every world X before drawing; src/func_ov006_021279b0.cpp zeroes it. |
| 0xab6c | `mScrollY` | `mPosY - 0x190000`, clamped to `[0, mScrollLimit]`; drives all four `SetBg*Offset` calls and the four hardware scroll registers in src/_ZN15dScMgSnowball_c8OnKickedEv.cpp. |
| 0xab70 | `mTouchX` / `mTouchY` (0xab74) | Behavior stores the raw touch sample (`data_020a0dea` / `data_020a0deb`) here and steers off the difference from the previous one. |
| 0xab78 | `mRollAngle` | u16. `+= speed * 0x2710 / mBallSize` -- an angle that advances faster the smaller the ball. |
| 0xab7c | `mHeadingAngle` | u16. `atan2(mVelX, mVelY)`, approached linearly while rolling and set outright while crashing. |
| 0xab7e | `mPrevRollAngle` / `mPrevHeadingAngle` (0xab82) | Behavior's prologue copies 0xab78..0xab7c into 0xab7e..0xab82 verbatim. |
| 0xab84 | `mSpinAxis[3]` | src/func_ov006_021279b0.cpp seeds it from a `(0, 0x1000, 0)` Vector3 and passes it to `Quaternion_FromVector3`. |
| 0xab90 | `mSpinQuat[4]` | The destination of that same `Quaternion_FromVector3`, then `Quaternion_Normalize`. Four words. |
| 0xaba0 | `mBallSize` | Seeded 0x4000; grows by the uphill distance, capped at 0x37000; Render scales mModel by `n/2 + n*4`; the melt state subtracts 0x1000 a tick until it reaches 0. |
| 0xac58 | `mArray1Active[0x80]` | Render skips an mArray1 slot unless this byte is 1. |
| 0xb0d8 | `mArray1Kind[0x80]` | 1 picks the eight-frame animated sprite table, anything else the single static sprite. |
| 0xb2d8 | `mArray1Hit[0x80]` | src/func_ov006_02125bbc.c sets it to 1 on contact; Render then adds 8 to the sprite frame. |
| 0xb358 | `mArray2Active[0x80]` | The same gate for mArray2. |
| 0xb3d8 | `mArray2Kind[0x80]` | Render's `switch`: 0..2 draw one sprite, 3 picks between two by X. |
| 0xb9d8 | `mAnimCounter` | Render bumps it and wraps it at 0x20; the obstacle frame is `(n / 4) & 7`. |
| 0xb9dc | `mTimeLeft` | Frames. Seeded 0x960 or 0x4b0 by variant; Behavior counts it down and plays a tick sound at 60/30/15-frame intervals as it shortens; Render formats it as seconds and centiseconds; 0 ends the run. |
| 0xb9e0 | `mScore` | Zeroed by the reset, +1 a tick while rolling, handed to the HUD counter `func_ov004_020adb1c` at the crash -- the same sink dScMgAmida_c's score uses. |
| 0xb9f4 | `mState` | Behavior's `switch`: 0 count-in, 1 rolling, 2/3 crash, 4 melt, 5 over. |
| 0xb9f8 | `mScreensSwapped` | u8. Behavior sets it from `mPosY >= 0xe8000`; src/_ZN15dScMgSnowball_c8OnKickedEv.cpp uses it to flip the POWCNT1 display-swap bit at 0x4000304 and exchange the main/sub BG offsets. |
| 0xb9fc | `mCountdownTimer` | Seeded 0xf1; state 0 counts it down, plays a beep at 0xf0/0xb4/0x78 and starts the run at 0x3c; Render draws the 3-2-1 banner from `n / 60`. |
| 0xba00 | `mStartY` | 0x2dc0 or 0x1740 by variant; `mPosY` starts at `mStartY << 12` and the progress bar uses it as one end. |
| 0xba04 | `mGoalY` | The other end of that bar, and the line `mPosY - mBallSize` must cross to end the run. |
| 0xba08 | `mScrollLimit` | `mScrollY` is clamped to `(n << 16) - 0x1d0000`. |
| 0xba0c | `mCrashTimer` | Counts up through states 2 and 4 and gates each of their phase changes (0x3c, 0x5a). |
| 0xba10 | `mEndDelayTimer` | 0xb4 frames of state 3, cut short by a touch. |

Left `unk_`: 0xab58 (the climb/descent debt term -- its arithmetic is clear, its
name is not), 0xab7a / 0xab80 (the middle angle of the three-angle block, never
read in scope), 0xabf4 (the loaded BMD file pointer, already documented),
0xb9e4/0xb9e5/0xb9e6 and 0xb9f0 (four flags inside `pad_b9e4`, only one of which
any matched body reads).

## dScMgSlot3_c field names

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x4fe4 | `mReelPos[3]` | InitResources seeds each entry with `(random % mStripLength) * 0x50000`; Render divides `n >> 12` by 0x50 to get the top stop and the pixel remainder, i.e. 0x50 screen pixels per symbol. |
| 0x4ff4 | `mReelWinPos[3]` | Read exactly the way `mReelPos` is, but only in the `mState == 6` win-line pass. Split out of the former `pad_4ff4[0xc]`, which was three words wide. |
| 0x5000 | `mState` | Behavior's whole body is `(self->*data_ov006_02142bdc[n])()` -- it is the index into that pointer-to-member table. Render tests it against 3, 4, 6 and 7 to pick which pass to draw. |
| 0x500c | `mReelDrawY` | While positive the two marker rows are drawn at `n + 0x10` and `n + 0x60`; at 0 or below a single row is drawn at 0x60. |
| 0x5010 | `mWinColumn` | Used as `n * 0x50 + 0x20/0x30/0x40` for the payout caption's x, against the same 0x50 column pitch the reels use; a negative value selects the "no win" caption instead. |
| 0x5018 | `mLamp1Angle` / `mLamp2Angle` (0x501a) | u16 each. Behavior subtracts 0x200 and 0x400 a tick while `mState == 1`; Render hands each to `func_ov004_020afb20` in its rotation argument. InitResources zeroes both. |
| 0x501c | `mReelStrip[3][5]` | Render walks it as `*(u8*)(p + row + 0x501c)` with `p` advancing 5 a reel and `row` taken modulo `mStripLength` -- three reels of five stops. |
| 0x502e | `mLineActive[3]` | Three bytes gating both the payout-marker pass and the win chime. |
| 0x5031 | `mResultSymbols[3][3]` | The same walk with `p` advancing 3 a reel, indexed 0..2 -- the 3x3 window the reels stopped on, compared against `mWinSymbol`. |
| 0x503a | `mStripLength` | The modulus of that `row` walk, i.e. the number of stops per reel. |
| 0x503b | `mWinSymbol` | Drawn as the marker row (`data_ov006_0213e9a4[n * 3 + i]`) and compared against `mResultSymbols` to decide a win. InitResources seeds it from 0x503c. |
| 0x503f | `mFrameCounter` | u8. Behavior increments it unconditionally; Render gates the marker pass on `n & 0x20` and fires the win chime exactly once per cycle on `(n & 0x3f) == 0x20`. |

Left `unk_`: 0x4ff0, 0x5004 (copied out of dScMgBase_c's own 0xbc, but nothing
in scope reads it back), 0x5014, 0x503c (the second marker row's symbol),
0x503d, 0x503e, 0x5040..0x5042.

## dScMgFlower_c field names

The minigame is petal-plucking: `mArray` at 0x4f38 is the 0x16-entry petal
table (its element type is still not reconstructed, so Behavior keeps reaching
into it by raw offset).

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x5fb8 | `mCursorX` / `mCursorY` (0x5fbc) | Behavior loads the touch sample `data_020a0dea` / `data_020a0deb` and shifts it left 12; the nearest-petal test is `Vec2_Len` of the difference against this pair. |
| 0x5fc0 | `mPrevCursorX` / `mPrevCursorY` (0x5fc4) | Copied from the pair above at the top of state 0; the drag applied to the held petal is the difference between the two. |
| 0x5fc8 | `mHeldPetal` | The `mArray` index the search loops store on a hit and read back to move that element; -1 means nothing is held, and the drop path restores it to -1. |
| 0x5fcc | `mPetalToggle` | u8, flipped on every completed pull. One value plays sound 0x103 with banner 0x10, the other 0x104 with banner 0x13 -- the "loves me / loves me not" alternation. |
| 0x5fd0 | `mHintTimer` | Held at 0x3c for as long as a petal is held, counted down otherwise; draining it while petals remain sets the face sprite. |
| 0x5fd4 | `mResultTimer` | Loaded with 0x3c after each outcome and counted down before the next pull is accepted; reaching 0 with no petals left moves to state 1. |
| 0x5fd8 | `mPetalsLeft` | Decremented once per pull, gates every "still playing" branch on `>= 1`, and is the bound of the loop that finishes off the remaining petals when the round times out. |
| 0x5fdc | `mWinStreak` | Incremented on the `mPetalToggle == 1` outcome and zeroed by the other; at 3 it swaps in banner 0x12 and adds 3 to `mScore` instead of 1. |
| 0x5fe0 | `mLoseStreak` | The mirror counter on the other outcome; at 3 it swaps in banner 0x11. It never touches the score. |
| 0x5fe4 | `mHoldTimer` | src/_ZN13dScMgFlower_c13OnYoshiTryEatEi.cpp increments it while `<= 0x14` and otherwise resets it to 0; Behavior treats `> 0x14` as "held long enough". InitResources zeroes it. |
| 0x5fe8 | `mState` | Behavior's `switch`: 0 plays, 1 is over (it stops the prompt and only ticks the 0x51f8 object). |
| 0x5fec | `mFaceSprite` | Render's only use is `data_ov006_0213ab94[n]` drawn at the screen centre; Behavior sets it to 0..4 on each outcome. |
| 0x5ff0 | `mScore` | Incremented by 1 or 3 per winning pull and clamped to 0x270f -- the same 9999 cap dScMgAmida_c and dScMgSnowball_c use. |
| 0x5ff4 | `mBgScrollPhase` | u16. Render adds 0xc0 a frame and feeds `>> 4` into `data_02082214` for both background layers -- the same idiom as dScMgAmida_c's own. |

Left `unk_`: 0x5fcd, a second gate on the between-rounds branch that nothing in
scope ever writes.

## dScMgTrampoline_c field names

Almost everything this class does with its own tail happens in the ov006
helper functions its pointer-to-member state machine dispatches to, not in the
four vtable methods; the citations below name those functions. All of them,
helpers and vtable methods alike, now live in the class's own translation unit,
src/minigames/d_s_mg_trampoline.cpp.

The three state-entry helpers are now real `dScMgTrampoline_c` members too:
`BeginIntro`, `BeginPlay`, and `BeginResults`. Each initializes the fields for
one phase and finishes by copying that phase's TU-owned PMF descriptor into
`mState`. Those English spellings are behavioral coinage; the ROM symbols were
address-only, so the exact original source names remain unknown.

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x5d94 | `mScrollY` | `dScMgTrampoline_c::UpdateScroll` approaches it toward `mScrollTargetY` by 2 a tick; `OnKicked` and Render both use `mScrollY + mScrollOffsetY` as the BG2 offset and as the hardware scroll register value. InitResources seeds it to 0x20. |
| 0x5d98 | `mScrollTargetY` | The other argument of that `ApproachLinear`; recomputed as `(q << 3) + 0x20` once the scroll has caught up. InitResources seeds it from `mScrollY`. |
| 0x5d9c | `mScrollHoldTimer` | Loaded with 0x78 and run down to 0 by `ApproachLinear(..., 0, 1)`; the target may not move again until it reaches 0. |
| 0x5da0 | `mScrollOffsetY` | Added to `mScrollY` at every one of its uses, and zeroed once the scroll settles. |
| 0x5da4 | `mArrow1X` / `mArrow2X` (0x5da8) | `dScMgTrampoline_c::StatePlay` drives the pair in opposition (`ApproachLinear` one toward 0 while the other goes toward 0x20); Render draws sprite `data_ov006_02134f08` at `n + 0xf0` for each. |
| 0x5dac | `mDragSoundHandle` | `UpdateTouchInput` passes the previous word to `func_02012468`, stores its returned handle, and zeroes it when a new drag begins; the intro interpolation uses the same positional-sound update path. |
| 0x5db0 | `mTouchX` / `mTouchY` (0x5db2) | `dScMgTrampoline_c::UpdateTouchInput` refreshes them from the touch sample `data_020a0dea` / `data_020a0deb` every tick a drag is live, and draws the drag segment from them. |
| 0x5db4 | `mTouchStartX` / `mTouchStartY` (0x5db6) | Copied from the pair above on the press edge and then left alone; `OnAttacked2` measures the swipe as start-to-current and only accepts it if the two ends sit on opposite sides of the screen. |
| 0x5db8 | `mInputEnabled` | s16. `UpdateTouchInput` clears `mTouching` and returns immediately while it is 0. |
| 0x5dc4 | `mTouching` | u8, set on the press edge and cleared when input is disabled; the drag body runs only while it is 1. |
| 0x5dc5 | `mTouchReleased` | u8, set on the release edge by the same file; `OnAttacked2` is the only reader and clears it after scoring the swipe. |

Left `unk_`: 0x5dba (an s16 with its own getter/setter pair,
`func_ov006_02121750` and `func_ov006_02121768`, but no reader that says what it
means), 0x5dbc..0x5dc2 (four s16 counters inside `dScMgTrampoline_c::StatePlay`'s
banner-blink logic).

## The minigame camera, and the base fields it explains

`src/Camera_UpdateMatrices.c` is the ov006 routine every 3D minigame scene
calls once a frame, and the local struct it already carries is the whole story:

```
struct Camera {           /* 0xbc */
    Matrix4x3 viewMat;    /* 0x00 */
    char      pad30[0x30];
    Matrix4x3 projMat;    /* 0x60 */
    char      pad90[0x10];
    Vector3   eye;        /* 0xa0 */
    Vector3   target;     /* 0xac */
    short     angle;      /* 0xb8 */
};
```

It computes the view direction as `eye - target`, so which vector is which is
settled rather than assumed. (This is NOT `include/Camera.h`'s `Camera`, the
0x1a8-byte gameplay camera whose `lookAt`/`pos` sit at 0x80/0x8c. Two different
types, one English word.)

**dScMgSingle3DBase_c** embeds one at 0x4660. Three readings agree:
`dScMgRoulette_c::Render` and `dScMg3DEsp_c::Render` both call
`Camera_UpdateMatrices(this + 0x4660)` immediately after writing 0x4700,
0x470c and 0x4718, which are +0xa0, +0xac and +0xb8 of that object; and
`0x4660 + 0xbc = 0x471c`, exactly where `mSysTracker` begins. So the seven
fields the previous round split out of `pad_4660` are that camera's `eye`,
`target` and `angle`, and they are named `mCameraEyeX/Y/Z`,
`mCameraTargetX/Y/Z` and `mCameraAngle`.

**dScMgD3DBase_c** embeds two, at 0x466c and 0x4728. Its header had already
established the 0xbc stride from all four factories' construction loops and
noticed that 0x470c..0x4724 and 0x47c8..0x47e0 are "the SAME seven offsets
within each element"; `dScMgJump_c::InitResources` names the two element bases
outright by calling `Camera_UpdateMatrices(this + 0x466c)` and
`Camera_UpdateMatrices(this + 0x4728)`. The fourteen fields become
`mCamera0EyeX/Y/Z`, `mCamera0TargetX/Y/Z`, `mCamera0Angle` and the same six
plus one for camera 1.

Both blocks stay flat scalars rather than becoming a real `Vector3` or a real
`Camera[2]`, for the reason dScMgD3DBase_c.h already gives: eight files spell
them this way, and typing them is its own change with its own blast radius.

## dScMgBase_c field names

Only the fields several descendants corroborate are named here; this class has
32 descendants and a wrong name in it is a wrong name in all of them.

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x0b4 | `mHudScore` | `dScMgBase_c::BeforeInitResources` zeroes it. `func_ov004_020adb1c` -- the routine that writes the HUD counter word at scene+0x464c -- is handed it directly by `func_ov006_02125364` (src/minigames/d_s_mg_bsc.cpp) and src/func_ov006_020ea3d0.c; dScMgMemory_c and dScMgSound_c seed it in their own InitResources; dScMgCard_c::Render keeps its own high-water mark of it; dScMgAmida_c::Behavior copies its round score into it. Deliberately NOT called `mScore`: five leaves already have a field of their own by that name, and naming the base's the same would silently shadow every one of them (see the round-2 `mPrevPosX` incident). |
| 0x21c | `mSavedMainBgBits` | src/_ZN11dScMgBase_c16OnAimedAtWithEggEv.cpp (slot 29) stores `data_0209d45c` here; src/_ZN11dScMgBase_c25OnAimedAtWithEggReturnVecEv.cpp (slot 30) restores it from here. |
| 0x220 | `mSavedSubBgBits` | The same save/restore pair for `data_0209d454`. |
| 0x224 | `mSavedScreenSwap` | Saved as `(POWCNT1 & 0x8000) >> 15` and restored as `n << 15` by that same pair. |

Deliberately left `unk_`: 0x0a8/0x0ac (a pair every leaf seeds together and
`func_ov004_020ad79c` checks against `func_ov004_020ad8b8`, but nothing in the
tree says what it counts -- dScMgRoulette_c reads it through the singleton as a
bonus added to its payout), 0x0bc (clamped to 0x270e, taken modulo 5, and used
to scale difficulty in four different leaves -- a progression counter of some
kind, but nothing in scope increments it), 0x0b8, 0x0c8, 0x0f0, 0x05c,
0x462c/0x4630, 0x465c.

## dScMgRoulette_c field names

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x53d6 | `mSelectedTile` | Render draws the cursor sprite at `data_ov006_02142ab4[n]` / `_02142ab8[n]` (the tile coordinate tables, stride 8); Behavior hands the same value to `func_ov006_02108b90` once per racer to score the board. |
| 0x53e4 | `mCameraPreset` | Render copies row `n` of `data_ov006_0213e34c` and `_0213e370` (stride 0xc) into the base's `mCameraTarget` and `mCameraEye`, takes the angle from `data_ov006_0213e2e0 + n*2`, and then calls `Camera_UpdateMatrices`. 0 leaves the camera alone. Behavior sets it to 1 as the deal starts and clears it at the end. |
| 0x53e6 | `mPhase` | Behavior's `switch`: 1 deals the racers out one at a time, 2 runs the countdown and reads the board, 3 pays out per landed tile, 4 announces win/lose/draw. _ZN15dScMgRoulette_c13OnYoshiTryEatEi (in src/actors/dScMgRoulette_c.cpp) starts it at 1. |
| 0x53e8 | `mPhaseTimer` | Counted down in every phase and reloaded on each transition (8, 0x258, 1, 0x5a); Render also renders the countdown digits from it while `mPhase == 2`. |
| 0x53f2 | `mScore` | Phase 3 sums each racer's payout into it and phase 4 compares it against `mTargetScore` to pick the win, lose or draw banner. Zeroed by the reset. |
| 0x53f6 | `mTargetScore` | One per racer that did NOT land on the winning tile; the bar `mScore` has to beat. |
| 0x53f8 | `mDealIndex` | How many racers have been dealt; the deal advances one per 8 frames until it reaches `mRacerCount`, then reuses the field as a loop cursor. |
| 0x53fc | `mRacerCount` | The bound of every per-racer loop in Behavior and Render; the reset seeds it from the party size. |

Left `unk_`: 0x53c4 (an "idle" gate Render's own local variable already calls
that, but nothing in scope writes it), 0x53e0, 0x53ea, 0x53ec..0x53f0 (five
bytes the reset zeroes and nothing reads).

## dScMgCard_c field names

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x5388 | `mState` | `func_ov006_020dac34` in src/minigames/d_s_mg_card.cpp is one long `switch` on it that mostly `++`s it; `func_ov006_020db720` in the same file switches on the same field; the reset in `func_ov006_020db9dc`, also there, starts it at 1. |
| 0x538a | `mStateTimer` | Reloaded with 0x10, 0x14, 0x1e, 0x3c or 0x5a on each step and run down to 0 (by `--` or `ApproachLinear2`) before `mState` advances. |
| 0x5396 | `mFrameCounter` | `dScMgCard_c::Behavior`'s only own statement is `+= 1`; Render blinks the highlighted cards on bit 3. |
| 0x5398 | `mScore` | Render keeps it as a high-water mark of the base's `mHudScore` and pushes it back out through `func_ov004_020adb1c` every frame. |

Left `unk_`: 0x538c, and the two highlight pairs 0x538e/0x5390 and
0x5392/0x5394. Their mechanics are now in the header (Render blinks the card
whose config byte matches either member of a pair, 6 means none, and the second
member is cleared back to 6 when the two banks disagree) but which of the two
is "picked" and which is "still lit" is not settled by anything in the tree.
