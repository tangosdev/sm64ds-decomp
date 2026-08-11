# dScene_c's eight unnamed children: vtable maps, so nobody re-derives them

**Status:** mapped, not migrated. Nothing here renames or types anything.
**Scope:** the eight of `dScene_c`'s ten direct children that have never had a
single function named in this tree.
**Provoked by:** migrating `Scene`/`Stage`/`BootScene` to real C++ (2026-08-11,
branch `cpp/stage-slices`) and wanting to know what's left in the family before
starting the next slice.

**Update (2026-08-11, branch `cpp/dscmgbase-family`, PR #1396):** `dScMgBase_c`
itself is now real -- `: Scene` base, D0/D1/D2 verified byte-exact (D2 stays the
hand-written free function it already was; D0/D1 are real, inline-cascading,
forcing-TU-emitted), and its 8 overridden slots (1, 2, 5, 6, 7, 9, 10, 12) are
real methods. Its own D0 no longer carries the OnYoshiTryEat mislabel from §3
below -- the file was rewritten from scratch, not patched. Its 15 direct / 32
transitive descendants are NOT yet named; that's still open, see §2's own note
on scope. `tools/check_header_offsets.py` gained a real fix in the same PR:
it had never seen a header that declares its destructor/overrides BEFORE its
fields (Scene.h's own convention) and silently reported "0 commented fields"
for exactly that shape -- dScMgBase_c.h's ~30 real fields were invisible to it
until fixed.

---

## 0. The ten, and where each one stands

`dScene_c` has exactly ten direct RTTI-confirmed children (`tools/rtti_extract.py`,
cross-checked against `include/Scene.h`'s own census comment):

| class | vtable addr | module | status |
|---|---|---|---|
| `dScBoot_c` | 0x02091528 | arm9 | **done** — `include/BootScene.h`, D0/D1 real |
| `dScStage_c` | 0x020921c0 | arm9 | **done** — `include/Stage.h`, D0/D1/methods real |
| `dScMB_c` | 0x020943c4 | arm9 | unnamed, 0 attributed functions |
| `dScTitle_c` | 0x020b1650 | ov003 | unnamed, 0 attributed functions |
| `dScStarSel_c` | 0x020b1704 | ov003 | unnamed, 0 attributed functions |
| `dScGameOver_c` | 0x020b179c | ov003 | unnamed, 0 attributed functions |
| `dScMgBase_c` | 0x020bc0c0 | ov004 | **class itself done** (PR #1396) — `: Scene`, real D0/D1/D2, 8/28 own-slots real. 32 descendants unnamed |
| `dScMiniGm_c` | 0x020c2490 | ov005 | unnamed, 0 attributed functions (has a generated header, no named methods) |
| `dScDSMT_c` | 0x021032e8 | ov007 | unnamed, 0 attributed functions |
| `dScEntry_c` | 0x0211d304 | ov075 | unnamed, 0 attributed functions (has a generated header, no named methods) |

"0 attributed functions" means literally no symbol anywhere in `config/**/symbols.txt`
mangles under that class's English or ROM name — confirmed by grepping every
mangled prefix (`7dScMB_c`, `10dScTitle_c`, `12dScStarSel_c`, `13dScGameOver_c`,
`11dScMiniGm_c`, `9dScDSMT_c`, `10dScEntry_c`) tree-wide, zero hits each.

## 1. Vtable slots, all eight

All eight are already-MATCHED source (`build_pin` passes, byte-exact) sitting
under `func_<mod>_<addr>` names — the blocker is naming/attribution, not
matching. Every one shares Scene's 18-slot shape; the slots below are the ones
that differ from Scene's own implementation (a real override, not an inherited
pointer). Full detail (per-slot addresses beyond what's listed, plus the raw
`rtti_vtables.json` this was read from) lived in a now-torn-down worktree's
`build/rtti_vtables.json` — regenerate with
`python tools/rtti_vtables.py --class dScMB_c dScTitle_c dScStarSel_c
dScGameOver_c dScMiniGm_c dScDSMT_c dScEntry_c dScMgBase_c` (or equivalent; check
the tool's current flags) rather than trusting this table's addresses blindly
for anything but orientation.

| class | module | overridden slots | example target |
|---|---|---|---|
| `dScMB_c` | arm9 | 0 (InitResources), 3, 6, 9, 16 (D1), 17 (D0) | `func_0203506c` (InitResources) |
| `dScTitle_c` | ov003 | 0, 3, 6, 9, 12, 16, 17 | `func_ov003_020ada9c` |
| `dScStarSel_c` | ov003 | 0, 3, 6, 9, 12, 16, 17 | `func_ov003_020af8a0` |
| `dScGameOver_c` | ov003 | 0, 3, 6, 9, 12, 16, 17 | `func_ov003_020b0b3c` |
| `dScMiniGm_c` | ov005 | 0, 3, 6, 9, 12, 16, 17 | `func_ov005_020c1a20` |
| `dScDSMT_c` | ov007 | 0, 3, 6, 9, 12, 16, 17 | `func_ov007_020cc4c0` |
| `dScEntry_c` | ov075 | 0, 1, 3, 6, 9, 12, 16, 17 | `func_ov075_0211a410` |
| `dScMgBase_c` | ov004 | 1,2,5,6,7,9,10,12,16,17 **+ 18 NEW slots (18-35)** | see §2 |

Slot numbers match Scene's own table (0=InitResources, 1=BeforeInitResources,
3=CleanupResources, 6=Behavior, 9=Render, 12=OnPendingDestroy, 16=D1, 17=D0) —
same convention `include/Scene.h`/`include/Stage.h` document.

## 2. dScMgBase_c is not a plain sibling -- it's a second hierarchy root

Its vtable is **36 slots**, not 18: it overrides 8 of Scene's own plus D1/D0,
then adds **18 brand-new slots (18-35)** beyond what Scene/ActorBase declare --
the same shape `include/Actor.h` documents for Actor's own 13 new slots
(`OnYoshiTryEat`, `OnTurnIntoEgg`, etc.). All 28 override/new-slot targets are
already matched source. ~24 of them carry `// recovered name: dScMgBase_c_X`
comments from an earlier, untooled vtable-identity pass -- **trust the D1 (slot
16) ones, not the D0 (slot 17) ones, see §3.**

`dScMgBase_c` also has **32 further RTTI descendants** -- a whole minigame
family, 24 of which already have generated headers waiting in `include/`:
15 direct children (`dScMgAmida_c`, `dScMgCoin_c`, `dScMgCurling_c`/`_2`,
`dScMgHanachan_c`, `dScMgLuigi_c`, `dScMgPachinko_c`/`_2`, `dScMgPanel_c`,
`dScMgSlot1_c`, `dScMgSmartball_c`, `dScMgTeresa_c`, `dScMgBomroom_c`, plus two
abstract intermediates `dScMgD3DBase_c`/`dScMgSingle3DBase_c`) with 17 further
grandchildren under those two. One direct child already has a COINED name from
an earlier, separate pass -- `MgBounceAndPounce` (`include/MgBounceAndPounce.h`,
still Rung 0/flat, calls dScMgBase_c's base methods as plain functions the way
the ROM's own `bl` sequence does) -- so "32 unnamed" is the RTTI count, not
literally every one being untouched.

**Naming the class itself (PR #1396) is done; naming the 32 descendants is
still the next slice**, and the groundwork PR #1396 laid down is exactly what
unlocks it: `dScMgBase_c` now has its own inline D2/D1 and its own
`operator delete`, which per the same cascading rule Scene's fix documents is
what a descendant's IMMEDIATE base must carry for the descendant's own D0/D1
to inline. Before this PR, no child of dScMgBase_c could get a real
destructor; now they can, the same way Stage/BootScene could only get theirs
once Scene's was fixed. `include/dScMgBase_c.h` still leaves 18 of its own 28
override/new-slots undeclared (slots 18-35, ~18 new virtuals beyond
Scene/ActorBase) -- their targets are matched source but their signatures
aren't reconstructed; three of the migrated methods reach them through the
same local by-vtable-position stand-in the recovered sources always used.
Reconstructing those 18 signatures is likely needed before any descendant's
OWN overrides of the same slots can become real methods too.

## 3. A tree-wide comment defect, found here, not yet fixed anywhere

**Every slot-17 (D0, the deleting destructor) "recovered name" comment across
all eight classes above is wrong** -- mislabeled `<Class>_OnYoshiTryEat`, even
though the function body is unmistakably a deleting destructor (writes the
class's own vtable, then Scene's, then ActorDerived's, calls
`ActorBase::~ActorBase`, calls `Memory::Deallocate`). Confirmed by reading the
actual disassembled bodies directly, not by trusting the comments, in
`dScMB_c`, `dScTitle_c`, `dScStarSel_c`, `dScGameOver_c`, `dScMgBase_c`,
`dScMiniGm_c`, `dScDSMT_c`, `dScEntry_c` -- eight for eight. Slot-16 (D1)
comments on the same files are accurate.

`dScMgBase_c`'s own instance of this is now moot, incidentally rather than by
design: PR #1396 rewrote its D0 file from scratch as a forcing-TU stub (see
`include/Scene.h`'s own D0/D1 files for the pattern), which carries no
"recovered name" comment at all. The other seven still have the mislabel.

This is almost certainly not scoped to just these eight -- the "recovered
name" comment layer covers roughly 700 files tree-wide (untooled, unattributed
per `notes/runbook-type-reconstruction.md` section 2's warning about the
`tools/deepen_rtti.py` banner fraud). Nobody has checked whether the D0-labeled-
as-OnYoshiTryEat pattern is systematic (e.g. every class's D0 happens to sit at
the same relative vtable-scan position slot-17 shares with something in
whatever tool or process generated these comments) or coincidental to this
family. **Whoever names these eight next will be reading these comments --
don't trust a slot-17 "recovered name" without checking the body first**, and
consider fixing the mislabeling mechanically before relying on the comment
layer for anything else in the tree. `tools/dtor_variant_audit.py` already
separates D0/D1/D2 by ROM structure (never by comment) for exactly this
reason; it doesn't yet report on the comment layer's own accuracy -- extending
it to do so would settle the "how far does this spread" question in one run
instead of by hand.

Related: `notes/plan-cpp-language-mode.md`, `notes/dtor-variant-audit.md`,
`notes/runbook-type-reconstruction.md` section 2.
