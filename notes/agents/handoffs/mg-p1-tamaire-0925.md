# Handoff: mg-p1-tamaire-0925

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/3171,
  task `mg-p1-tamaire-0925`, stage `revise` (role producer), session
  `claude-prod-mg-p1-tamaire-0925`, Claude Code.
- Source branch and previous accepted input SHA: `readable/mg-p1-tamaire-0925`, input
  `e63828444b7ad3fc20afb58de29857b618dddbcf`.
- Original source base SHA and installed workflow tool SHA: both
  `e63828444b7ad3fc20afb58de29857b618dddbcf`.
- Separate evidence commits and required artifacts in this commit: none. The source commit
  `a785d9ffeb8b0f464a02307ba4b1243349e8be9c` carries every source and header change; this
  handoff is the only file after it.
- Next action, responsible role and blockers: independent verification of this exact commit
  (byte, relocation, whole-object and source review). No blocker for verification.
- Status: locally byte-verified pass-1 candidate.
- Remaining uncommitted material: the base and candidate objects, experiment sources and
  logs lived in the producer worktree's ignored build directory and go with it. The
  rejected replacements and their measured results are recorded below.

## What changed and why

Tamaire is scene 0x171, profile MG_TAMAIRE, class `dScMgPachinko2_c` in ov006, all [ROM].
It derives directly from `dScMgBase_c` and exists only as one-function files. This is its
first readability pass: mechanical themes only, no new names, no symbol renames.

- `Behavior` and `InitResources` drop `char *c = (char *)this` and reach every field by the
  name `include/dScMgPachinko2_c.h` or `include/dScMgBase_c.h` already declares.
- `Render` includes `include/decl_common.h`, which declares its eight helpers with the same
  `(void*)` parameter, and drops its duplicate local block and the stale comment that said
  the header used empty parameter lists.
- `OnYoshiTryEat` drops a duplicate declaration of `func_ov006_02103bfc`.
- Comments in the six method files and the class header now state what the code does, in
  short form. Each shape the compiler forces carries one short comment naming what it
  forces.
- The factory `src/d_s_mg_pachinko2.c` is unchanged (R11, R12).

Reserved surfaces touched: the six method files, `include/dScMgPachinko2_c.h` (comments
only) and this handoff. `config/arm9/overlays/ov006/delinks.txt` and the factory path were
reserved for the theme-4 move, which did not land.

## Findings

- **R1, fixed.** `Behavior` read `unk_5660`, `unk_566c`, `unk_566e`, `mPromptBlinkCount`,
  `mPromptEnabled` and `mPromptBlinkTimer` through raw offsets on a `char *` alias of
  `this`. Now plain member access, and the helpers take `this`. The `(u32)` cast on the
  `unk_566e` decrement was not needed: `unk_566e--` emits the same code.
- **R2, fixed.** `InitResources` wrote `unk_0a4`, `unk_566c` and `unk_5660` the same way.
  Now by name. Its five helper calls cast `this` (four `(char *)this`, one `(int)this`)
  because `include/decl_common.h` declares those parameters as `char *` and `int`, so the
  `(char *)this` count in this file rises from 1 to 4 until the helpers become methods.
- **R3, deferred (codegen-forced).** `InitResources` passes each `MultiStore16` fill value
  through a `volatile u16` stack temporary. Passing the value directly, or dropping
  `volatile`, changes the code, so both stay with one comment.
- **R4, deferred.** `InitResources` still calls six GX, GXS and G2S functions through
  mangled `extern "C"` names. No header declares those namespaces, so pass 1 leaves them.
  Local `namespace` declarations compile to an identical object (see Theme results).
- **R5, fixed.** `Render`'s local helper declarations duplicated `include/decl_common.h` and
  its comment misdescribed that header.
- **R6, fixed.** `OnYoshiTryEat` redeclared `func_ov006_02103bfc` exactly as
  `include/decl_common.h` does.
- **R7, deferred (codegen-forced).** `OnYoshiTryEat` reads `unk_0bc` through a const view
  `ro`. Re-measured: `unk_0bc = unk_0bc + 1` grows the function from 0x6c to 0x70 bytes.
  The comment is now two lines.
- **R8, deferred (codegen-forced).** The clamp after `unk_0bc = 0` is dead but required:
  deleting it gives 0x5c bytes, and one clamp after the `if` gives 0x54. One comment added.
- **R9, fixed.** The header banner called slot 18 an unnamed helper that is not a declared
  method, and said fields are reached through a `char *` cast. Both were stale. The banner
  now lists the overridden slots, checked against the ROM vtable at 0x0213dbbc, and the
  comments on `unk_5660` and `unk_566e` say how the code uses them.
- **R10, fixed.** The D0 and D1 comments are one line each. Both still define the same
  empty destructor; each file keeps the variant its symbol marker names.
- **R11, deferred (measured negative).** Theme 4 in the factory: see Theme results. The
  factory keeps its hand-built form.
- **R12, deferred.** The factory banner is stale. It says the `data_*` symbol is unnamed,
  but `config/arm9/overlays/ov006/symbols.txt` also names 0x0213dbbc
  `_ZTV16dScMgPachinko2_c`. It says the base C2 call cannot be written in C++, but
  `new dScMgPachinko2_c` emits exactly that call. The brief leaves the factory as it is
  when theme 4 does not land, so the banner waits for the pass that moves the factory.
- **R13, deferred (theme 8).** `Behavior` cannot include `include/decl_common.h`: its local
  declarations of `func_ov006_02100084`, `func_ov006_021024e0`, `func_ov006_020fffec` and
  `func_ov004_020b0a54` take `void *`, and the header's take `char *` or `int`. mwccarm
  rejects the include with "illegal function overloading" on those four lines. The call
  `func_ov004_020b0a54((void *)0x10)` keeps its cast for the same reason.
- **R14, deferred (pass 2).** Locals named after stack slots (`sp4`, `sp8`, `spC`, `spE`),
  the unnamed helper functions and the remaining `unk_` fields need evidence-backed names.

## Theme results

- **Theme 1, proven here.** `dScMgPachinko2_c::Behavior` (`mPromptBlinkCount`,
  `mPromptEnabled`, `mPromptBlinkTimer`) and `dScMgPachinko2_c::InitResources`
  (`unk_0a4`).
- **Theme 2, proven here.** `dScMgPachinko2_c::Behavior` and
  `dScMgPachinko2_c::InitResources`, each converted in one step.
- **Theme 3, not applied here; measured neutral.** No header declares the mangled callees
  in `dScMgPachinko2_c::InitResources`. With local declarations
  `namespace GX { void LoadBGPltt(const void *, u32, u32); void LoadOBJPltt(...); }`, the
  same for `GXS`, and `namespace G2S { unsigned GetBG2CharPtr(); char *GetBG2ScrPtr(); }`,
  the object is identical, relocation symbols included. A pass that may add such
  declarations, or a shared header for them, can take this.
- **Theme 4, measured negative here.** `dScMgPachinko2_c_classInit` rewritten as
  `extern "C" void *dScMgPachinko2_c_classInit() { return new dScMgPachinko2_c; }` in a
  C++ file compiles to the same `.text`, since the implicit constructor inlines and no C1 is
  needed. The vptr literal's relocation, however, moves from `data_ov006_0213dbbc` + 0 to
  `_ZTV16dScMgPachinko2_c` + 8. `tools/objisolate.py` subtracts the 8-byte vtable preamble
  only while it rewrites an object, and `isolate()` returns early when there is nothing to
  drop, as in a one-function object. So the full link writes 0x0213dbc4 where the ROM has
  0x0213dbbc. rombuild's intact-TU control reported ov006 not byte-exact, with this
  function as its one mismatch. The D0 and D1 files store the same vtable correctly because
  their objects carry sections to drop. A tooling change that applies the correction to
  every object would unblock this theme for one-function factories; it belongs in its own
  change.
  The move would also turn the MG_TAMAIRE row of
  `symbols/profile_reconstruction_registry.tsv` from complete to pending under the filename
  rule in `tools/check_profile_campaign.py` (read from the tool, not run on the moved tree).
  That file and its `.json` twin, which also record the factory path, are outside this
  reservation. The experiment was reverted.
- **Theme 7, not applicable.** None of these files uses `data_ov004_020beb68`.
- **Theme 11, not applicable.** No hand-rolled vtable view here; slot 18 is already a real
  override.
- **Theme 14, not applicable.** The six methods handle no Fix12 value.

## Proof

Every command below ran in the producer worktree on source commit
`a785d9ffeb8b0f464a02307ba4b1243349e8be9c`, pinned compiler 2004/b56, and the retail
cartridge dump. The commit after it adds only this file.

- **Baseline, before any edit.** `python tools/rombuild.py -j8 --no-rom`: exit 0,
  7,069 enrolled sources, all compiled (0 reused), 11,214 of 11,214 source-built functions
  reproducing, 0 mismatching, 106 of 106 modules exact. The seven batch objects, and later
  the header's other consumer `func_ov006_02102fe8`, were copied aside as the base.
- **Per-edit loop.** After each of six steps (Behavior, InitResources, Render,
  OnYoshiTryEat, the two destructor comments, the header) `python tools/rombuild.py -j8
  --no-rom` exited 0 with 11,214 of 11,214 reproducing and 106 of 106 modules exact, and
  compiled exactly the edited files (1, 1, 1, 1, 2, then 7 for the header's consumers).
  Each step was also compared object against object; every step was identical.
- **No tubuild verify.** This batch has no TU; its sources are one-function files.
- **Object comparison** (pyelftools, base against candidate, the build's objects): all
  eight objects (six methods, the factory, `func_ov006_02102fe8`) are identical in every
  section's bytes, in relocations by (offset, type, symbol, addend), and in defined and
  undefined symbol sets. The object files are also identical as whole files: each
  candidate SHA256 equals its base, for example `8ef99cfc...a6ac15` for Behavior and
  `9b698157...6d310` for InitResources.
- **`python tools/rombuild.py -j8`**: exit 0. ROM built; `intactTuRom.identical` is true,
  SHA256 `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8` equal to the
  expected image. 11,214 of 11,214 source-built functions reproducing, 0 mismatching;
  106 of 106 modules exact; intact TU gates: modules pass, zero new symbol errors. This run
  took all 7,069 objects from the content-keyed cache; the batch's cache entries were made
  by the per-edit runs, which compiled these exact sources.
- **`python tools/prepush_linkcheck.py --range e63828444b..HEAD`**: exit 0. The changed
  header fans out to 7 sources; 7 VERIFIED, 0 warnings, 0 blocking.
- **`python tools/prepush_attribution.py --base e63828444b --head HEAD`**: exit 0.
  7,241 tracked, 0 changed, 0 lost.
- **`python tools/check_decl_agreement.py --changed e63828444b`**: exit 0. 139 files in
  scope; no new local redeclarations of header-declared symbols, no new disagreements.
- **`python tools/check_decl_agreement.py`**: exit 0. 15,213 disagreements, the same as
  the base; no new disagreements. Declarations fall from 35,896 to 35,887 (the eight in
  Render and one in OnYoshiTryEat). `config/decl-agreement-baseline.json` is untouched.
- **`python tools/check_dead_references.py`**: exit 0, no new dead references, no broken
  markdown links.
- **`python tools/tiers_ratchet.py --check`**: exit 0, CONVERTED ratchet PASS (3,216
  current against a banked 3,001, as on the base).
- **`python tools/check_tubuild_conflicts.py`**: exit 0, 302 manifest entries, every
  conflict recorded.
- **`python tools/queue_audit.py --check-promoted`**: exit 0.
- **`python tools/check_src_tu_compiles.py`**: exit 0, 302 of 302 compile.
- **`python tools/port_refcheck.py`**: exit 0, 408 references, all resolve.
- **Also run:** `python tools/langmode_audit.py --check langmode-baseline.json` exit 0
  (PASS); `python tools/check_profile_campaign.py --repo .` exit 0 (391 rows complete,
  0 pending, 0 diverged, as on the base). Every gate above also passed on the untouched
  base.
- **Line endings and markers.** All eight touched files keep CRLF. `git diff --check`
  flags only the carriage returns in `include/dScMgPachinko2_c.h`, whose blob was already
  stored with CRLF. Each of the seven sources keeps its one symbol marker, and ROM function
  order and `#pragma` use are unchanged (none of these files has a pragma).
- **Theme-4 experiment** (reverted): with the factory as a C++ file and its
  `config/arm9/overlays/ov006/delinks.txt` entry moved to match, `python tools/rombuild.py
  -j8 --no-rom` exited 1: ov006 not byte-exact, one mismatching function
  (`dScMgPachinko2_c_classInit`, one word). The linked word at 0x02104288 was 0x0213dbc4;
  the ROM has 0x0213dbbc.
- **Not run:** private validation and GitHub checks; no PR exists.

## Residue before and after

Measured with `MSYS_NO_PATHCONV=1`, `wc -l` and `grep -c -F` per pattern. The `unk_`
count rises where raw offsets became the header's `unk_` field names.

| File | lines | `func_` | `unk_` | `_ZN` | `extern "C"` | `goto` | `(char *)this` |
|---|---|---|---|---|---|---|---|
| `src/_ZN16dScMgPachinko2_c13InitResourcesEv.cpp` | 123 to 123 | 10 to 10 | 0 to 3 | 14 to 14 | 1 to 1 | 0 to 0 | 1 to 4 |
| `src/_ZN16dScMgPachinko2_c13OnYoshiTryEatEi.cpp` | 34 to 29 | 2 to 1 | 6 to 6 | 1 to 1 | 1 to 1 | 0 to 0 | 1 to 1 |
| `src/_ZN16dScMgPachinko2_c6RenderEv.cpp` | 32 to 18 | 16 to 8 | 0 to 0 | 1 to 1 | 1 to 0 | 0 to 0 | 0 to 0 |
| `src/_ZN16dScMgPachinko2_c8BehaviorEv.cpp` | 73 to 71 | 36 to 36 | 0 to 7 | 1 to 1 | 1 to 1 | 0 to 0 | 1 to 0 |
| `src/_ZN16dScMgPachinko2_cD0Ev.cpp` | 9 to 7 | 0 to 0 | 0 to 0 | 1 to 1 | 0 to 0 | 0 to 0 | 0 to 0 |
| `src/_ZN16dScMgPachinko2_cD1Ev.cpp` | 10 to 7 | 0 to 0 | 0 to 0 | 1 to 1 | 0 to 0 | 0 to 0 | 0 to 0 |
| `src/d_s_mg_pachinko2.c` | 27 to 27 | 0 to 0 | 0 to 0 | 2 to 2 | 0 to 0 | 0 to 0 | 0 to 0 |
| `include/dScMgPachinko2_c.h` | 71 to 66 | 2 to 2 | 10 to 10 | 0 to 0 | 2 to 0 | 0 to 0 | 0 to 0 |

## Changed paths

`git diff --name-only e63828444b..HEAD`:

- `include/dScMgPachinko2_c.h`
- `notes/agents/handoffs/mg-p1-tamaire-0925.md`
- `src/_ZN16dScMgPachinko2_c13InitResourcesEv.cpp`
- `src/_ZN16dScMgPachinko2_c13OnYoshiTryEatEi.cpp`
- `src/_ZN16dScMgPachinko2_c6RenderEv.cpp`
- `src/_ZN16dScMgPachinko2_c8BehaviorEv.cpp`
- `src/_ZN16dScMgPachinko2_cD0Ev.cpp`
- `src/_ZN16dScMgPachinko2_cD1Ev.cpp`

All are within the task's reservation. No symbol, delinks, manifest, attribution or
baseline file changed.
