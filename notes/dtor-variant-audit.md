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

**The biggest gap: it only inspects symbols already named `D0`/`D1`/`D2`.** A
genuine D2 sitting under a `func_*` name is invisible to it, and those exist in
numbers — `include/MeshColliderBase.h` names two (`func_02039658` is
`MeshColliderBase`'s D2, `func_020397fc` is `MeshCollider`'s), and the Fader family
holds two more (`func_02017838`, `func_020177c4`). Each shows the same signature:
referenced only by `kind:arm_call`, never by `kind:load`, called by destructors,
and the same size as the class's D1.

Finding them is a *discovery* pass rather than an audit — sweep every `func_*`
that writes a known vtable VA and has no `load` reloc pointing at it. That is the
obvious next tool, and it would name D2s the tree has never had names for, rather
than only correcting the ones it named wrongly.

It also says nothing about non-destructor symbols, and nothing about whether a
correctly-named destructor is attributed to the right *class* — `_ZN5SceneD2Ev`
was caught on the variant, and its wrong class fell out of the correction rather
than being tested for.
