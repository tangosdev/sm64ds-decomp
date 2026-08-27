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
| real `Class::Method` definitions | 443 | **455** |
| `extern "C"` free functions under the mangled name | 17 | **5** |

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

## The wall, measured

Five files remain, and they are not one problem.

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

### A by-value class parameter — 1 file, measured cost

`dEnemyBase_c::IsGoingOffCliff(dBgCh_Actr &, Fix12<int>, short, bool, bool, Fix12<int>)`
(ov002 0x020ae2b8, 0x19c) compiles as a real method and lands at **0x1ac — four words
over**, whether the `Fix12<int>` parameters are copied into locals at entry or used in
place through their `.val`. `Fix12<int>` is a one-`int` POD aggregate and
`sizeof(Fix12<int>) == 4`, and it still costs. The free-function form, whose
parameters are plain `int`, matches exactly. This is the standing
by-value-class-parameter blocker with a number attached: **+0x10 on this function**.

### A disputed name — 1 file, deliberately not migrated

`_ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player.c` converts to a real
method and matches, and it was reverted anyway. `include/dEnemyBase_c.h` marks the
signature PROVISIONAL and says "do not migrate a caller against it yet": the name is
attached to ov004 0x020ada40, which cannot be this method (it reads only `r0` and
range-checks it as a scalar; the code that IS the method is ov002 0x020ada40, still
`func_ov002_020ada40`), and three call sites materialise a fourth argument in `r3`
that the arity does not admit.

The general point, which is not about this file: **`extern "C" int _ZN…(void *self)`
and `void Class::Method(T &, U &)` assert different things.** The first says only
"these bytes live at this address". The second asserts the name, the owning class, the
arity and every parameter type. The byte gates cannot tell the two claims apart — they
pass identically. So where a header records a dispute, the prose is the binding
evidence and the `MATCH` line is not.

### The rest — 3 files, each its own reason

- `_ZN7daKrb_c13InitResourcesEv.cpp` — a **NONMATCHING** near-miss at 916 bytes
  against the ROM's 912, one instruction over. Its `//cpp` marker also sits behind an
  `#include` and is therefore inert, but that is not what stands between it and a
  match: 916 comes out identical under `-lang c99` and `-lang c++`.
- `_ZN11RollingRock8BehaviorEv.c` — a documented three-word near-miss, one
  register-allocation choice, with a ruled-out list of ten spellings in the file
  header. Do not re-grind it without a lever that changes pressure at the struct copy.
- `_ZN7daOts_cD1Ev.cpp` — **not actually a holdout.** `daOts_c.h` defines
  `~daOts_c() {}` in the class body so descendants inline it, and the file's
  `_force_daOts_cD1` scaffold is the sanctioned way to force the out-of-line variant
  (`dtor-migration.md` §3). The destructor here is already compiler-generated; only
  the *forcing function* is hand-written, and `objisolate` drops its `.text`.

## One thing this sweep found that is not about enemies

`rombuild.compile_one` and `build_pin.flags_for` both test `text.startswith("//cpp")`
and **never** consult the extension. Ten `.cpp` files tree-wide carry the marker behind
an `#include`, so the build compiles them as **c99**:

```
_ZN14BlueCoinSwitch13InitResourcesEv   _ZN6Number13InitResourcesEv
_ZN7daKrb_c13InitResourcesEv           _ZN8Goomboss13InitResourcesEv
func_ov002_020f6618                    func_ov006_020e6e78
func_ov006_020e7fe8                    func_ov060_02111cc0
func_ov075_021143e4                    func_ov075_02114ddc
```

The standing advice has been to leave them alone because they match as c99. For
`Goomboss` that turned out to be wrong: moving the marker to byte 0 still matches, and
the file is now a real method. The other nine were not tested here. The advice should
be "measure it", not "leave it" — the marker's position is a property of the file, not
evidence about the function.
