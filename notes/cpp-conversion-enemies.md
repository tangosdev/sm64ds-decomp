# The enemy family's C++ conversion — sweep and remaining wall

**Written 2026-08-27**, branch `cpp/period-accurate-form`. Scope: every `src/` file
whose mangled stem names a class that derives, transitively, from `dEnemyBase_c`.
Companions: `notes/ctor-migration.md`, `notes/dtor-migration.md`,
`notes/plan-cpp-conversion-queue.md`, and the `decomp-cpp-class-form` skill.

The goal this sweep serves is narrow and worth stating: a file where the compiler
generates the constructor, the destructor variants and the name mangling is a
*different artefact* from a file that spells all three by hand inside `extern "C"`,
even when the two produce identical bytes. The second kind cannot be read as the
original source. Byte-matching was never the question here; it is the floor.

## Where the family stood

Resolving `include/*.h`'s inheritance edges transitively gives **60 classes** under
`dEnemyBase_c`. Counting every `src/` file whose stem mangles to one of them:

| | before | after |
|---|---|---|
| real `Class::Method` definitions | 443 | **461** |
| `extern "C"` free functions under the mangled name | 17 | **1** |

So the family was already 96% real C++ and the sweep is a tail, not a campaign. That
is worth recording because the tail is where the *interesting* blockers live — the
bulk conversion is done, and what remains are distinct reasons a file cannot be
written the way the original was.

## What landed

| file | ROM | what the compiler now generates |
|---|---|---|
| `_ZN12dEnemyBase_cC2Ev` | ov002 0x020aed98 0x24 | base call + vptr store |
| `_ZN12dEnemyBase_cD2Ev` | ov002 0x020aed18 0x24 | vptr store + base D2 |
| `_ZN7BooCageD0Ev` | ov063 0x0211600c 0x5c | 4 sub-object dtors, base chain, `operator delete` |
| `_ZN11dCapEnemy_cD1Ev` | arm9 0x0200651c 0x38 | vptr store, 2 members, base D2 (was named D2) |
| `_ZN11dCapEnemy_cD2Ev` | ov002 0x020aedbc 0x38 | the same body, bound to the other variant |
| `_ZN7daTrs_cD1Ev` / `D0Ev` | ov063 0x02115ee0 / 0x02115f48 | 6 members, base D2 chain |
| `_ZN7daKrb_cD1Ev` / `D0Ev` | ov084 0x02129020 / 0x02129070 | 5 members, base D2 chain |
| `_ZN14UnchainedChompD0Ev` | ov100 0x02143290 0xe0 | 3 `__destroy_arr` + 4 sub-object dtors + chain |
| `_ZN11dCapEnemy_cD0Ev` | ov002 0x020aedf4 0x4c | 2 sub-object dtors, base chain |
| `_ZN15daObjMarioCap_cD0Ev` | ov002 0x020b6f68 0x64 | 5 sub-object dtors, base chain |
| `_ZN8Goomboss13InitResourcesEv` | ov074 0x02121e98 0x404 | name mangling; header decl added |
| `_ZN8Fireball8BehaviorEv` | ov002 0x020f8c94 0x570 | name mangling |

Every one passed `tools/match.py` (byte comparison *and* relocation destination)
under 2004/b56 and then `rombuild.py -j16`: **106/106 exact** after each commit. The
`eligible.py` bracket across the two header edits was 11071/11191 on both sides with
a zero-line diff — neither new declaration cost an enrolment, and neither made its
file the class's key-function TU.

## Two corrections to the standing notes

**1. `notes/dtor-migration.md`'s 2026-08-24 sweep is out of date on two classes.** It
lists `dCapEnemy_c` and `daObjMarioCap_c` among the nine D0s `d0_migrate.py` could not
keep, blocked on "a member whose type's destructor is still a `func_` shell (the
CapIcon ordering)". Both migrate cleanly now and the full relink agrees. The CapIcon
ordering was fixed underneath the note.

**2. `tools/d0_migrate.py` does not update `delinks.txt`.** It performs the
`.c` -> `.cpp` rename itself and reports `OK`, but leaves the enrolment entry pointing
at the old path. The next `rombuild` fails in `layout_check` with *"delinks names a
path with no file there (function silently falls back to ROM bytes)"* — which is the
honest failure, but it arrives one step after the tool's own `kept 2 / tried 2`.
Re-point the delinks line by hand after any `d0_migrate` run.

## The wall, measured — and dismantled

Nine files remained when this note was first written. All nine are now
converted; what follows is what each one actually turned out to be.

### The cross-module `dCapEnemy_c` D2 duplicate — SOLVED, 4 files (2026-08-27)

`daKrb_c` D0/D1 and `daTrs_c` D0/D1 all reproduced the ROM **byte for byte** as real
destructors and were all rejected by the relocation leg with the same line:

```
+0x4c  cand _ZN11dCapEnemy_cD2Ev (0x0200651c) != config 0x020aedbc:ov002
```

The reading recorded here first — one vague-linkage COMDAT destructor duplicated into
two modules, so naming the ov002 copy would put one mangled name in both
`config/arm9/symbols.txt` and an overlay's, which this tree has never done — was
wrong. There is no duplicate and no collision. `dCapEnemy_c` has no virtual bases, so
its **D1 and D2 are byte-identical code**, and the two addresses are the two ABI
variants. The names were on the wrong ones.

What settles which is which is not the bytes but how the ROM reaches each address, and
both questions are cheap to ask:

- **Who calls it?** Scanning every module's `bl` instructions for the two targets
  returns four call sites, *all* of them to ov002 0x020aedbc, and *none* to arm9
  0x0200651c. The four are exactly daTrs_c's and daKrb_c's two destructors each —
  derived destructors tearing down their base sub-object. That is what D2 is for.
- **Who points at it?** Scanning every module for a data word equal to either address
  returns one hit: ov002 0x021082c4, which is slot 16 of `_ZTV11dCapEnemy_c`
  (0x02108284) — the destructor pair's slot, with slot 17 already holding the matched
  `_ZN11dCapEnemy_cD0Ev`. A vtable slot holds the complete-object destructor. So arm9
  0x0200651c is **D1**.

So `config/arm9/symbols.txt`'s `_ZN11dCapEnemy_cD2Ev` at 0x0200651c became
`_ZN11dCapEnemy_cD1Ev`, and ov002's placeholder `func_ov002_020aedbc` at 0x020aedbc
became `_ZN11dCapEnemy_cD2Ev`, each with its source file renamed to follow. Both are
now `dCapEnemy_c::~dCapEnemy_c() {}` — mwcc emits all three variants from one
definition and `objisolate` keeps whichever variant the delink entry names, so the
same two-line body serves both files. With the chain target correct, all four blocked
destructors became compiler-generated C++ with no source change at all beyond dropping
their hand-spelled bodies.

The general lesson is worth more than the four files. **A wall that says "the compiler
names the wrong function" may be the config naming the wrong address.** Byte-identical
ABI variants cannot be told apart by comparing them; ask the image who calls each one
and who stores a pointer to it. Both scans are a dozen lines against
`modules.modules()` and either one alone would have settled it.

### A by-value class parameter — SOLVED, and the wall was the name

`dEnemyBase_c::IsGoingOffCliff` was the standing example of
notes/mwccarm-codegen.md 6az: declared with `Fix12<int>` parameters it landed
0x10 over, 17 spellings got no closer than +0x8, and the rule was that a symbol
carrying `5Fix12IiE` "cannot be DEFINED as a real C++ method".

Two measurements dissolved it.

`KillByInvincibleChar` (below) takes a `Fix12<int>` by value, never reads it, and
matches exactly -- so the homing is emitted in response to a USE, not by the
calling convention, and 6az's "the cost is in the parameter passing, not in how
the member is reached" was the wrong way round. On this function, reading one such
parameter costs +0x8 and reading both +0x10: two words per used class parameter.

And the premise was unchecked. **The image contains zero `_Z...` strings and zero
occurrences of `5Fix12IiE`**, censused over arm9 and all 105 overlays -- which is
just notes/symbol-name-provenance.md's opening line ("a retail NDS cartridge has
no symbol table") arriving from the other direction. Every mangled name here is
reconstructed, so a `5Fix12IiE` inside one is somebody's decision, and mwcc's
homing is the ROM saying that decision is wrong. Renamed
`_ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi` -- `Fix12i`, the `s32`
typedef that keeps the intent and mangles as plain `i` -- it matches as a method.

One trap on the way: the parameters must be used DIRECTLY. Aliasing them into
locals at entry keeps parameter and copy both live and costs r7, r8 and a spill.

### Two names on the wrong function — SOLVED, 2 files

ov002 and ov004 are alternates in one overlay slot, both based at 0x020ad3e0, so
one address is two unrelated functions and every actor overlay's `relocs.txt`
records such a call as the ambiguous `module:overlays(2,4)`. Two `dEnemyBase_c`
names had landed on the ov004 side.

`KillByInvincibleChar` was on ov004 0x020ada40, which reads only r0 and
range-checks it as a scalar. The ov002 body at the same address is the method:
`Vec3_HorzAngle(player + 0x5c, &this->pos)`, then `Player::IncMegaKillCount`. Its
arity was refuted too -- all 19 `bl`s to the address were disassembled, and the
split IS the proof: the four inside ov004 leave r3 alone, all 15 outside it
materialise a fourth argument. ov081 0x02123988 passes the return of vtable slot
29, OnAimedAtWithEgg -- which this method's own body then calls again itself, so
the parameter is handed over and never read. Type from the ROM's sibling
`dBgActor_c::UpdateKillByMegaChar(s16, s16, s16, Fix12<int>)`. Moved, given the
third parameter, migrated: matches first try.

`KillByAttack` was on ov004 0x020aea30, whose body walks an 8-byte table to an
0xffff sentinel. The ov002 function there dispatches a pointer-to-member from
`data_ov002_0210db80[mDeathState - 1]`, and is what all 34 enemy-overlay call
sites reach. Same arity shape: an unused fourth argument makes it match.
Deliberately left as `func_ov002_020aea30` -- the name was coined against the
wrong body, and inheriting it would repeat the mistake -- with the evidence
recorded in the file for a naming pass.

### The last two near-misses — SOLVED, 2 files

`daKrb_c::InitResources` sat 4 bytes over (916 against 912) behind a NONMATCHING
banner. One instruction, and it is the field-address CSE: `*(int*)(c + 8) =
*(int*)(c + 8) & 0xf0ff;` spelled identically on both sides lets mwcc compute the
address once where the ROM re-issues `[r4, #8]`. Eleven variants swept, and the
split is clean -- all six that differ the spelling match, all three that do not
miss by the same word. It is now a real method AND enrolled, so ov084 gains 912
bytes from source.

`RollingRock::Behavior` settled at three words with a note saying "spelling alone
does not reach it" after ten spellings. The lever was already written down, just
not applied at the copy: `struct AngleWords { u16 w[3]; };` and
`*(AngleWords *)&v16 = *(AngleWords *)(c + 0x92);`. Copying through a struct whose
only member is an array is the documented remedy for C++ struct-copy
scalarisation, and it turns out to fix load ORDER as well as load width -- the
address is not 4-aligned, so no `ldm` is involved; what it buys is that the copy
stops being three member assignments, and the register reuse that forced the
ROM-unlike interleave goes with it. notes/mwccarm-codegen.md gains 6ay.

### What is left: nothing, and one thing that was never a holdout

`src/_ZN7daOts_cD1Ev.cpp` is the only `extern "C"` definition left under a family
class's mangled name, and its `_force_daOts_cD1` scaffold is the sanctioned way to
force an out-of-line variant of a class-body-inline destructor. It is not a
holdout and should not be "fixed".

## What the five walls had in common

Not one of them was a compiler limit.

| the wall | what it actually was |
|---|---|
| cross-module `dCapEnemy_c` D2 duplicate | D1 and D2 names on each other's addresses |
| by-value `Fix12<int>` parameters | a coined parameter type the ROM refutes |
| two "disputed signatures" | the name on the other overlay's function |
| `daKrb_c::InitResources`, 4 bytes | a documented codegen lever, unapplied |
| `RollingRock::Behavior`, 3 words | a documented codegen lever, unapplied |

Three were naming errors and two were levers already written down in this repo.
The common failure is the same in both halves: a note recording a measurement,
then a later reader treating the note's *explanation* as measured too. The
measurements were all sound -- 916 really is 4 over, the reloc really was wrong,
`Fix12<int>` really does cost 0x10. It was the sentence after each measurement
that was the guess, and each one closed off the question for months.

Cheap habits that would have caught all five, in the order they cost least:

1. **Ask the image who calls it.** A dozen lines against `modules.modules()`
   scanning `bl` targets, and another dozen scanning for data words equal to an
   address, settled both naming errors and the D1/D2 swap. Neither needed a
   disassembler.
2. **Grep `relocs.txt` for `module:overlays(`.** Both wrong-function names were
   already flagged there as ambiguous.
3. **Re-read the lever list before declaring a near-miss dead.** Both remaining
   near-misses fell to a lever in notes/mwccarm-codegen.md.

## One thing this sweep found that is not about enemies

`rombuild.compile_one` and `build_pin.flags_for` both test `text.startswith("//cpp")`
and **never** consult the extension. Ten `.cpp` files tree-wide carry the marker behind
an `#include`, so the build compiles them as **c99**:

The standing advice has been to leave them alone because they match as c99. Twice now
that has been wrong. `Goomboss` moved to byte 0, still matched, and became a real
method. `daKrb_c::InitResources` was worse: the inert marker was not what stood between
it and a match -- 916 came out the same either way -- but it stopped `fdiff` and the
permuter from compiling the file at all, which is presumably why a four-byte near-miss
sat unexamined long enough to grow a NONMATCHING banner. Move the marker first; then
measure.

Eight are left untested:

```
_ZN14BlueCoinSwitch13InitResourcesEv   _ZN6Number13InitResourcesEv
func_ov002_020f6618                    func_ov006_020e6e78
func_ov006_020e7fe8                    func_ov060_02111cc0
func_ov075_021143e4                    func_ov075_02114ddc
```
