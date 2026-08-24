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

**Size.** func_ov005_020c21ec is the factory; it opens with
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
dScMgRoulette_c's Render (src/_ZN15dScMgRoulette_c6RenderEv.cpp) and dScMg3DEsp_c's Render
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

Real ROM name confirmed by `tools/rtti_extract.py` (build/rtti.json). Own vtable
ov006:0x0213ec98, RTTI ov006:0x0213ebec (`_ZTI19cMgSmartball_ball_c`),
`_ZTS19cMgSmartball_ball_c` at ov006:0x0213edc0. One of eleven direct children
of cMgSmartball_object_c -- see that header for the family's shape (a root,
three slots, no virtual destructor).

Size 0x12c, from `_Znwj(0x12c)` in func_ov006_02115b0c. The base ends at 0x34,
so this class adds 0xf8 bytes -- the densest of the eleven children. Everything
below 0x34 is reached through inherited members; this class's four functions
never touch the base's 0x31-0x33 region, so no raw cast is needed anywhere.

Most of the 0x34+ fields are declared and zeroed by this class's own
RestoreInitial, which is exhaustive -- every array length and every scalar width
below comes from that function's loop bounds and store widths. SaveSnapshot and
Update corroborate roughly half of the same offsets.

**Several names are borrowed, not invented.** func_ov006_02112ad8.c and
func_ov006_021128fc.c -- two out-of-scope helpers SaveSnapshot calls with `this`
-- each reinterpret the pointer through their own local Obj-style struct cast
and name a number of these exact offsets (hit/hitA/hitB/hitC, anyHit,
specialHit, nearby, targetIndex, soundTimer, soundPlayed, state3a, state3b).
Every one of those offsets is also independently touched by RestoreInitial, so
the width and existence of each field is evidenced in-scope; only the spelling
is borrowed. Anything without that corroboration keeps an `unk_` name.

0x44-0x4b are hitX/hitZ in func_ov006_02112ad8.c's naming, but none of this
class's own four functions touches them, so per the wing_c precedent they stay
an explicit pad -- unmodelled, not unread. pad_0e7[0x11] (0xe7-0xf7) is a
genuine gap: RestoreInitial's exhaustive zero pass skips straight over it
(nearby[] ends at 0xe6, targetIndex starts at 0xf8) and func_ov006_02112ad8.c's
Obj cast also treats it as padding. pad_101 / pad_111 / pad_122 / pad_12a are
pure alignment gaps between adjacent int fields (house style: explicit pads over
implicit compiler-inserted ones).

Constructed by func_ov006_02114548, left a free function per the recipe. It
calls the base constructor and writes only this vtable and the base's
`unk_028 = 0x8000`; it touches nothing at or past 0x34, so it adds no evidence
to the field list.

## dScMgAmida_c (include/dScMgAmida_c.h)

dScMgAmida_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py (zero
RTTI edges name dScMgAmida_c as a base).

Own vtable slots (python tools/rtti_vtables.py --own dScMgAmida_c): 0
(InitResources), 5 (AfterCleanupResources -- the recovered source locally
declared the base override as returning void*, which is WRONG; the real
dScMgBase_c.h override returns void, so this now calls the base method as
a plain statement instead of returning it, same fix dScMgLuigi_c's own
slot 5 needed), 6 (Behavior), 9 (Render), 16 (D1), 17 (D0), 18 (own new
slot, not yet named -- stays a raw extern "C" helper,
src/func_ov006_020d52f0.c, same precedent as every other dScMgBase_c
leaf's slot 18; it no longer includes this header at all -- its one
inherited-field access at 0xbc is dScMgBase_c's own pad_0bc, not a named
field there either, so it now reaches it via a raw char* offset, the same
idiom dScMgPachinko_c's own slot 18 helper already uses), 31 ("Kill",
src/func_ov006_020d11a0.cpp -- takes no parameters and never touches
`this` at all, pure hardware-register/global reset; left as a raw helper,
untouched by this migration), 34 (src/func_ov006_020d14c0.c -- takes
three extra parameters (y, x, arg3) beyond `this` and draws a HUD
digit/glyph into its own private Ctx-shaped state; same multi-argument
shape dScMgTeresa_c's own slot 34 documents, left as a raw helper,
untouched by this migration -- it never includes this header either).

rtti_vtables.py --own ALSO reports slot 35 (func_ov006_020d1170, a
one-line `((*(int*)((char*)c+8))&0xff)==1` check, same shape as slot 36
below but for a different constant) -- unlike slot 36, nothing in
InitResources/AfterCleanupResources/Behavior/Render/D1/D0 calls it (no
cross-reference anywhere in src/ besides its own file), so it is left
completely alone: not declared, not renamed, not even touched.

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

Fields below dScMgBase_c's own last-observed field (0x465c, DATA_SIZE
0x465d per tools/check_header_offsets.py's own convention -- see
dScMgBase_c.h) are genuinely THIS class's own, drawn only from what
InitResources/AfterCleanupResources/Behavior/Render/D1/D0 directly touch.
Fields touched only by the raw slot 18/31/34 helpers (0x46d8..0x4700's
digit-drawing state, 0x4704..0x470b's flag bytes, and dScMgBase_c's own
inherited 0xb4/0xbc slot 18 touches) are NOT represented here -- they
stay inside a pad, same discipline as every prior sibling. The class's
true allocation size, 0x53fc, is independently confirmed (not just the
last-named-field's rounded end) by the untouched constructor's own
`_ZN7fBase_cnwEj(0x53fc)` call (src/func_ov006_020d5974.cpp), which
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
