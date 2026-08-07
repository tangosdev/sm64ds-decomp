# The D2 backlog is 41% mislabelled

**Status:** answered, and reproducible — `python tools/dtor_variant_audit.py`.
**Scope:** which destructor variant a symbol actually is. Nothing here renames anything.
**Provoked by:** picking `Scene` as the Phase-2 pilot per `notes/plan-cpp-language-mode.md`,
and finding its `D2` is not a D2 and not Scene's.

---

## 0. The rule is not new; mechanising it is

`include/MeshColliderBase.h` already states it, calls it **the #774 rule** — *"a
class vtable carries [D1, D0] and never D2; D2 is reached by direct call alone"* —
applies it correctly to that one class, concludes *"the symbol names are on the
wrong bodies"*, and deliberately defers the fix: *"Correcting that pair is a
symbols.txt rename, deliberately left to its own change so this one stays
byte-neutral."*

So the reasoning below was already right, in the tree, for one class. What is new
is running it over all 538 destructor symbols instead of the one somebody happened
to be reading, which is what turned 1 known case into 8. The header's deferred
rename is the change this note accompanies.

## 1. The rule, and why it decides

The Itanium ABI gives a polymorphic class up to three destructors:

| variant | in the vtable? | reached how |
|---|---|---|
| `D0` deleting | **yes** | virtual dispatch; also calls `operator delete` |
| `D1` complete | **yes** | virtual dispatch |
| `D2` base-object | **never** | a direct `bl` from a derived class's destructor |

That last row is structural, not a heuristic. A D2 exists to destroy a base
*subobject*; it is never dispatched, so its address is never taken.

And "is this function's address taken" is already in the tree, in the authority
`notes/runbook-reference-repair.md` §2 names — dsd's analysis of the real ROM:

    kind:load     the value is data      -> the address was taken
    kind:arm_call the value is a call    -> it was not

So for any symbol named `...D2Ev`, a single `kind:load` reloc pointing at it is a
disproof. No disassembly, no compiler, no heuristic.

**The inverse direction needs one more condition**, and omitting it produced a
false positive on the first run. "A D1 always sits in a vtable" holds only for a
**polymorphic** class; a class with no virtual functions has a D1 that is simply
called, and no vtable for it to sit in. The polymorphism test is also read from
the ROM rather than from a name — a destructor of a polymorphic class always
writes its class's vptr, which shows up as a load-reloc *from* inside the
function *to* a vtable VA that `build/rtti.json` knows:

    named D2, a load-reloc points at it                        -> not a D2
    named D1, nothing points at it, AND it stores a vtable VA   -> not a D1

## 2. The result

    destructor-variant symbols: D0 261   D1 260   D2 17

**7 of the 17 are impossible.** Each is resolved back through `build/rtti.json` to
the vtable that references it, which also names what it should have been:

| symbol | occupies | is really | class named right? |
|---|---|---|---|
| `_ZN5SceneD2Ev` | `dScBoot_c` slot 16 | `_ZN9BootSceneD1Ev` | **no — wrong class** |
| `_ZN5StageD2Ev` | `dScStage_c` slot 16 | `_ZN5StageD1Ev` | yes |
| `_ZN5EnemyD2Ev` | `dEnemyBase_c` slot 16 | `_ZN5EnemyD1Ev` | yes |
| `_ZN6PlayerD2Ev` | `daPly_c` slot 16 | `_ZN6PlayerD1Ev` | yes |
| `_ZN8PlatformD2Ev` | `dBgActor_c` slot 16 | `_ZN8PlatformD1Ev` | yes |
| `_ZN8PathLiftD2Ev` | `dPathLiftActor_c` slot 16 | `_ZN8PathLiftD1Ev` | yes |
| `_ZN16MeshColliderBaseD2Ev` | `dBgW` slot 0 | `_ZN16MeshColliderBaseD1Ev` | yes |

Slot 16 is the D1 slot in the 18- and 32-slot Actor-family tables (`dScene_c`
slot 16 is `_ZN5SceneD1Ev`, slot 17 `_ZN5SceneD0Ev`); slot 0 is the D1 slot in
the small tables (`dBgW` slot 1 is `_ZN16MeshColliderBaseD0Ev`). **All seven land
on a D1 slot.** None is anything more exotic than a variant mix-up — except
`_ZN5SceneD2Ev`, which belongs to a different class entirely.

Every one of the seven correct names is currently **free**, and each of the seven
classes already carries a correctly-named `D0`. No class has both spellings, so
there is no collision to resolve — the rename is one-for-one.

## 3. The control, which is the reason to believe any of it

Invert the rule and it must find the other error: a `D1` of a polymorphic class
that **no** load-reloc points at cannot be a D1. Run over the same 260 D1
symbols, that returns exactly **one**:

    _ZN5ColorD1Ev   0x02017574   arm9   ->  _ZN10FaderColorD2Ev

which was **independently** proven in the same session by a completely unrelated
argument — it writes `data_0208eb2c`, which is `dFdColor_c`'s vtable, and six
derived-class destructors across three classes call it as their base-subobject
dtor (`notes/rtti-reconciliation.md` §4 for the chain). Two orthogonal signals,
one answer, zero false positives.

That is what makes the 7 credible. A rule firing on noise would fill this second
list with ordinary destructors.

**It did, before the polymorphism condition was added** — see §5.

## 4. What this does to Phase 2

`notes/plan-cpp-language-mode.md` reports `D2 | 17 | UNPROVEN -- 0 migrated` and
picks pilots from the per-class backlog. Both are affected:

- **The count is wrong in both directions.** 17 named D2s minus 7 impostors is
  10; plus the Fader family's three genuine D2s that carry no D2 name at all
  (`func_02017838`, `func_020177c4`, and `_ZN5ColorD1Ev`). The category was never
  measuring what it claimed.
- **Five of the plan's six pilot classes are affected** — Scene, Stage, Player,
  Enemy and Platform all show `D2:1` in `--by-class`, and all five of those are
  impostors. The one category Phase 2 most wanted to settle is the one whose
  membership was least reliable.

**Migrating any of these as a real `~Class()` D2 would have written the error into
C++**, where it is far more expensive to find than in a symbol table. That is
`notes/runbook-type-reconstruction.md` §7's "do not migrate before the types are
right", one level up: not the field types, the *symbol's identity*.

## 5. Three wrong first answers, recorded

**The inverse rule had a false positive, and it was the row with the most churn.**
The first version flagged `_ZN8Particle10SysTrackerD1Ev` (`0x02023194`) as a D2 on
the strength of "no load-reloc points at it". `Particle::SysTracker` has **no RTTI
record and no `_ZTV` symbol** — it is not polymorphic, its destructor writes no
vptr at all (it frees memory and clears two globals), and a non-polymorphic
class's D1 has no vtable to sit in. It was correctly named all along.

The rule now requires the function to store a known vtable VA before it will call
a D1 an impostor, and the inverse list drops 2 → 1. Two things worth keeping:

- It would have been the **most expensive** row to act on — 36 referencing files,
  against 1–5 for every other candidate. Cost and confidence were inversely
  correlated, which is the shape that gets a batch shipped on the strength of its
  cheap rows.
- The forward rule never depended on polymorphism. "A D2 is never dispatched" is
  true of every class, so the 7 were unaffected. Only the control was wrong, and
  the control is the argument, not the finding.

**A fixed-size window mis-attributed 2 of the 7.** The first version of `locate()`
searched `va <= addr < va + 0x400` for the containing vtable. Vtables are packed
far more tightly than that — `dBgCh`'s successor `dBgCh_Actr` begins **11 slots**
later — so a 256-slot window swallowed three sibling tables and returned whichever
the dict happened to yield first. It reported `MeshColliderBase` at "dBgCh slot
108" and `PathLift` at "dBgActor_c slot 94", and both were read as "not a
destructor slot at all — even more wrong than a variant error."

They were ordinary slot-16/slot-0 D1s. The tool now bounds each vtable by the next
vtable start in the same module and reports `None` rather than guessing when an
address lands in no known table. **The disproof itself never depended on this** —
that comes from the reloc kind alone — but the *correction* did, and a wrong class
here names the wrong symbol.

**`Scene` was picked as the Phase-2 pilot on two claims, both false.** The plan
says *"Its header is already mostly named, so the slice tests the migration rather
than migration plus field reconstruction at once."* `include/Scene.h` declares
**one** field (`u8 unk_013`) behind `0x13` bytes of padding, under the fabricated
`gen_header.py` banner that `runbook-type-reconstruction.md` §2 documents. And all
three Scene destructors are shadow-struct files that do not include it. The pilot
choice needs re-deriving from `--by-class` output that has been through this audit.

## 6. What is not covered

The rule decides `D2` versus `{D0, D1}`. It does **not** separate D0 from D1 — both
sit in vtables — so a D0/D1 swap would pass silently. Slot position distinguishes
them (D1 then D0, adjacent), which the table in §2 uses but the tool does not yet
check. That is the obvious next assertion to add.

The polymorphism test is a **one-way** guarantee. Storing a vtable VA proves the
class is polymorphic; not storing one is only evidence, since a size-0 symbol or a
module `rtti.json` cannot see would look the same. The tool reports that case as
undecidable rather than folding it into either answer.

The audit direction only inspects symbols already named `D0`/`D1`/`D2`, so a
genuine D2 under a `func_*` name is invisible to it. That is now covered by
`--discover`, below.

It also says nothing about whether a correctly-named destructor is attributed to
the right *class* — `_ZN5SceneD2Ev` was caught on the variant, and its wrong class
fell out of the correction rather than being tested for.

## 7. The other direction: D2s the tree never named at all

    python tools/dtor_variant_audit.py --discover

A base-object destructor has a signature nothing else shares: no `load` reloc
points at it, it stores a vtable address, and every caller reaches it by `bl`. And
**the vtable it stores names the owning class**, because a D2 writes its own
class's vptr.

    11 D2 candidates    9 C2 candidates    33 undecided

It reproduces both of `include/MeshColliderBase.h`'s hand-derived claims exactly —
`func_02039658` is `MeshColliderBase`'s D2, `func_020397fc` is `MeshCollider`'s —
and the tree-name join gets `dBgW_Kc` → `MeshCollider` on its own. Nine more D2s
follow that nothing in the tree had named: `dBgCh`, `dBgPi`, `dM3dGSph`,
`dBgW_KcMbg`, `dEnemyBase_c`, `dCapEnemy_c`, `dScMgBase_c`, and the Fader family's
`func_02017838` / `func_020177c4`.

### A C2 has the same signature as a D2

The first version of this pass returned 53 candidates and called them all D2s.
About a fifth were **constructors**: a C2 is also never in a vtable, also writes a
vptr, and is also only ever called directly. `func_02035514` came back as "dBgCh's
D2" on the strength of callers that were `SphereClsn::SphereClsn` and
`RaycastLine::RaycastLine`.

Two independent signals separate them, and the tool now requires both to agree:

| | D2 | C2 |
|---|---|---|
| callers | derived **destructors** | derived **constructors** |
| vptr write order | own class **first**, then bases | bases first, own class **last** |

The order falls out of what the two routines do: a destructor unwinds from the
derived class down, a constructor builds from the base up. `func_ov004_020b29c0`
writes `dScMgBase_c -> dScene_c -> dBase_c` and is a destructor;
`func_02017278` writes `dFader_c -> dFdBrightness_c -> dFdColor_c -> dFdDummy_c`
and is `dFdDummy_c`'s constructor.

That C2 population is worth its own look: `notes/plan-cpp-language-mode.md` Phase 5
records **0 constructors migrated, ever**, and treats them as research.

### Naming them is blocked, and the reason is worth knowing

Six of the eleven were named, byte-verified 6/6, built at 106/106 — and then backed
out, because **both** ways of spelling a named D2 fail a gate:

| spelling | what fails |
|---|---|
| `_ZN5FaderD2Ev` hand-spelt in a `.c` | the langmode ratchet: `unmigrated_total` 1274 → 1280. It is *right* — a hand-spelled mangled symbol is exactly the backlog `plan-cpp-language-mode.md` exists to shrink. Re-banking a **risen** count is the fake progress Phase 0 was built to make unrewarding. |
| a real `Fader::~Fader() {}` | `eligible.py`: **"extra sections: .data"**, and enrolled falls 10699 → 10695. |

The second is the interesting one. Defining a destructor makes it the class's **key
function**, so mwccarm emits the whole vtable group into that translation unit.
Dumping the object confirms it — alongside `_ZN5FaderD2Ev` it contains:

    _ZTV5Fader      _ZTI5Fader      _ZTS5Fader      _ZN5FaderD0Ev

Those already exist, delinked from the ROM. A source file cannot own them too.

`include/MeshColliderBase.h` and `include/ModelBase.h` had already named this
arrangement without spelling out the consequence: *"THE DESTRUCTOR IS DECLARED FIRST
AND NEVER DEFINED AS A METHOD -- the key-function arrangement"*, and
`include/MeshCollider.h`'s *"The structors stay C files."*

**This is why `D2` reads 0 migrated, and it is not a codegen problem.** The bytes are
perfect: `Fader::~Fader() {}` reproduces D2 at `0x02017838` *and* D1 at `0x0201786c`,
both exact. What blocks it is symbol ownership.

The 72 D1s the audit counts as migrated dodge it with a **shadow struct** —
`struct Actor { char pad[0xd0]; virtual ~Actor(); };` declared locally, destructor
never defined, so no vtable is emitted. That route is closed here: `plan-cpp-language-mode.md`
§6 forbids a slice from declaring a struct body locally, and Phase 4 exists to delete
the 1,638 that already do. Trading a correct symbol name for new shadow debt is the
§3.1 failure this plan is built around.

So the real prerequisite is **vtable ownership**: until `_ZTV5Fader` and friends come
from a source file rather than from delinked ROM data, no destructor of a polymorphic
class can be written as a real method against its real header. That is a
`runbook-type-reconstruction.md` §7 dead end, and it gates Phase 2 far more than the
per-class header work does.

### Caveats, unresolved

- **33 are undecided** — callers split evenly, or none of them is a named
  ctor/dtor. They are reported as undecided rather than assigned.
- **`func_0203ac60` and `func_0203ac70` both propose `dM3dGSph`'s C2.** They cannot
  both be right; one is likely a different class or a C1. Unresolved.
- Nested names (`dPa_c::level_c::callback_c`) get **no proposed symbol**. Guessing
  an Itanium nested mangling wrong writes a symbol that resolves to nothing, so
  those print the class and stop.
- Nothing here is renamed. Each candidate still wants its size compared against the
  class's D1/C1 and its callers read.
