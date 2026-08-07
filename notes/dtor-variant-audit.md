# The D2 backlog is 41% mislabelled

**Status:** answered, and reproducible — `python tools/dtor_variant_audit.py`.
**Scope:** which destructor variant a symbol actually is. Nothing here renames anything.
**Provoked by:** picking `Scene` as the Phase-2 pilot per `notes/plan-cpp-language-mode.md`,
and finding its `D2` is not a D2 and not Scene's.

---

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

Invert the rule and it must find the other error: a `D1` that **no** load-reloc
points at cannot be a D1, because a D1 always occupies a vtable slot. Run over
the same 260 D1 symbols, that returns exactly two:

    _ZN5ColorD1Ev                  0x02017574  arm9
    _ZN8Particle10SysTrackerD1Ev   0x02023194  arm9

`_ZN5ColorD1Ev` was **independently** proven to be `_ZN10FaderColorD2Ev` in the
same session by a completely unrelated argument — it writes `data_0208eb2c`,
which is `dFdColor_c`'s vtable, and six derived-class destructors across three
classes call it as their base-subobject dtor (`notes/rtti-reconciliation.md` §4
for the chain). Two orthogonal signals, one answer.

That is what makes the 7 credible. A rule firing on noise would put ordinary
destructors in this second list. Instead it holds precisely the D2s that were
misfiled the other way — the two lists are each other's control.

`_ZN8Particle10SysTrackerD1Ev` is a **new** D2 candidate on the same evidence and
has not been separately corroborated. Treat it as the weakest row here.

## 4. What this does to Phase 2

`notes/plan-cpp-language-mode.md` reports `D2 | 17 | UNPROVEN -- 0 migrated` and
picks pilots from the per-class backlog. Both are affected:

- **The count is wrong in both directions.** 17 named D2s minus 7 impostors is
  10; plus `_ZN5ColorD1Ev`, plus `Particle::SysTracker`, plus the Fader family's
  three genuine D2s that carry no D2 name at all (`func_02017838`,
  `func_020177c4`, and `_ZN5ColorD1Ev` itself). The category was never measuring
  what it claimed.
- **Five of the plan's six pilot classes are affected** — Scene, Stage, Player,
  Enemy and Platform all show `D2:1` in `--by-class`, and all five of those are
  impostors. The one category Phase 2 most wanted to settle is the one whose
  membership was least reliable.

**Migrating any of these as a real `~Class()` D2 would have written the error into
C++**, where it is far more expensive to find than in a symbol table. That is
`notes/runbook-type-reconstruction.md` §7's "do not migrate before the types are
right", one level up: not the field types, the *symbol's identity*.

## 5. Two wrong first answers, recorded

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

It also says nothing about non-destructor symbols, and nothing about whether a
correctly-named destructor is attributed to the right *class* — `_ZN5SceneD2Ev`
was caught on the variant, and its wrong class fell out of the correction rather
than being tested for.
