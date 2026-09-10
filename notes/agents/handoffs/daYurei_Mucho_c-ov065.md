# Handoff: daYurei_Mucho_c (ov065)

This document describes this branch. Nothing below is a plan; every number was
measured at this tree.

## Identity and resumption

- Task: promote the class the tree carried as the coined `Snufit` into a single
  C++ translation unit under the cartridge's own RTTI name, `daYurei_Mucho_c`.
  Stage `reconstruct`, role producer, session `prod-yurei-0907`, harness Claude
  Code.
- Branch `cpp/daYurei_Mucho_c-tu`, based on `26f54f8fc`. Workflow/tool pin
  `f327f7b6460e157153eb7fc0749dbbe60dd854f1`.
- Worktree `C:/tmp/sm64ds-sm64ds-yurei`. A second wired worktree,
  `C:/tmp/sm64ds-yurei-base` on `base/yurei-26f54f8fc`, holds the base ROM build.
  Neither was created or removed with `git worktree remove`.
- `origin/main` MOVED during this session: it was `26f54f8fc` when the task was
  written and is `c27d8ea38` now. This branch still descends from `26f54f8fc`,
  and the base ROM build was taken there. Gate results below say which base each
  used.
- Three commits: the rename, the promotion, and the queue/worklist re-derivation.
- No PR was opened and nothing was merged.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `daYurei_Mucho_c` in ov065.
  The promoted TU owns `.text 0x02115ee0..0x02116f98`, 23 functions. No other
  section is claimed; this is the text-only route.
- The reserved neighbour was not touched. `daBasabasa_c` (Swoop) begins at
  0x02116f98 and shares `config/arm9/overlays/ov065/delinks.txt`; its entries are
  byte-identical to the base commit's.

### The name

The cartridge names this class. ov065 0x0211cb6c holds the `_ZTS` payload
`15daYurei_Mucho_c`, `_ZTI15daYurei_Mucho_c` at 0x0211cb60 points its +4 word at
that string, and the vtable's -4 header word points back at the `_ZTI`. The
string `Snufit` occurs nowhere in the cartridge. `tools/class_rename.py` did the
substitution, because a textual replacement corrupts the Itanium length prefix
(`6Snufit` is not `15daYurei_Mucho_c`).

Two things had to be done by hand around it, both recorded in the rename commit:

- `class_rename.py` refused with "`_ZTV15daYurei_Mucho_c` is already defined" --
  ov065's symbols.txt already carried the ROM-named vtable row at 0x0211cba4
  alongside a coined `_ZTV6Snufit` alias at the same address. The coined alias
  row was DELETED and nothing was added; exactly one spelling now survives at
  that address. The matching dead `extern int _ZTV6Snufit[];` came out of
  `include/decl_common.h`.
- `class_rename.py` regex-substitutes `symbols/actor_renames.tsv` in place, which
  erases the coined lineage the ledger exists to record. The eight rewritten rows
  had their `" (was <coined>)"` suffixes restored and eleven provenance rows were
  appended in the shape the landed `daBmb_c` and `daMky_c` rows use.
- `--with-derived` was NOT used: its `\bSnufit_` pattern matches `Snufit_Kill` in
  the reserved Swoop run. Plain `\bSnufit\b` does not, so the historical
  `Snufit_Spawn` / `Snufit_SpawnInfo` ledger rows survive untouched.

### The fold is 23, not the queue's 22

Confirmed by address rather than taken from the queue. The 22 functions
`tu_map.py` reports run 0x02115ee0..0x02116f48 without a gap; the next
`delinks.txt` entry, the retired shard `d_a_yurei_mucho.c`, runs
0x02116f48..0x02116f98 with a zero-byte gap, and the entry after THAT is at
0x02116f98, which opens the reserved neighbour's run.

The undercount's cause is in the code, not a judgement call:
`tools.srcpath._SPAWN_RE` is `^(\w+)_Spawn$`, so `class_of` returns `None` for
`daYurei_Mucho_c_classInit` and `tu_map` segments the factory as its own
unlabelled unit. Filed as issue #2436. Absorbing the neighbour under either
spelling gives the same answer here, because ov065 spells this factory
`_classInit` and no `_Spawn` symbol exists for the class.

Independent corroboration from a dated artifact:
`notes/data/tu-merge-candidates.json` already recorded this ov065 unit as
`0x2115ee0..0x2116f98`, 23 functions.

The factory had to fold anyway. `tu_names.candidate_stem('daYurei_Mucho_c')` is
`d_a_yurei_mucho`, the stem the retired factory shard already occupied.

### The vtable address point, re-derived

Not inherited from the dossier. In the cartridge, `ldr r1,[pc,#0x2c]` at
0x02116f60 loads the literal at 0x02116f94, which is `0x0211cba4`, and
`str r1,[r4]` at 0x02116f68 writes it to `this+0`. Vtable STORAGE begins at
0x0211cb9c -- offset-to-top `0x00000000` then typeinfo `0x0211cb60` -- so
0x0211cba4 - 0x0211cb9c = 8, the two-word Itanium preamble. 31 slots run
0x0211cba4..0x0211cc1c. Both destructor literals (0x02115f24 and 0x02115f7c)
hold 0x0211cba4 as well.

That measurement is what decides the spelling in the source. The retired factory
shard wrote the bare `(int)_ZTV6Snufit`, which was right while the symbol was
UNDEF and symbols.txt bound it to the address point. In the promoted TU the same
spelling would bind to this TU's own definition, two words lower, so the store is
written `(int)(_ZTV15daYurei_Mucho_c + 2)`.

**It is not, however, load-bearing in the shipped object, and an earlier draft of
this note and of the source comment both said it was.** Compiled both ways, the
two spellings isolate to byte-identical objects, sha256
`54f1b54d7368e7617103fffe891e05995649797122ae06e374ff701c69044f7d`, which is also
the sha256 of the shipped `build/src/actors/daYurei_Mucho_c.o`. A relocated word
is a wildcard to every byte gate, so `match.py` reports 23/23 MATCH with the bare
form too; and `rombuild.py` sends a multi-function source through
`objisolate.isolate_many`, whose whole-TU path explicitly accepts addend 0 for a
`_ZTV` target and rebases only a nonzero one, so addend 8 and addend 0 both leave
the pipeline as UNDEF `_ZTV` addend 0.

What refuses the bare form is the singular per-symbol path, `objisolate.plan`,
which requires `addend >= VTABLE_PREAMBLE` for a `_ZTV` target and reports
`_ZTV15daYurei_Mucho_c: unexpected reloc type=2 addend=0`. That is the path
`tubuild verify` and `tubuild partial` take per member, and it is where the
message was first seen. It was expected to be a known false positive; it was not,
and the control is recorded under Proof. So `+ 2` buys per-member isolation
rather than bytes -- and it stays, because it is the spelling the ROM literal and
mwcc's own D1/D0 vptr stores both write.

### A wrong callee the byte gate could not see

The retired shard `func_ov065_021162c0.c` called `_ZN6EyerokD0Ev` -- ov066's name
for address 0x02115f84. In ov065 that address is `func_ov065_02115f84`, the death
helper two ordinals below, and ov065 is the module this branch links. The ROM's
own `bl 0x02115f84` at 0x02116314 decides it. `match.py` wildcards the relocated
word, so the wrong callee reproduced the bytes; `linkcheck.py` is what closes it,
and it now reports VERIFIED with 0 blind slots.

### Style oracle

Templated from `src/actors/daObjCtMecha03_c.cpp` and
`config/tu_manifest.d/ov065/daObjCtMecha03_c.json` -- the most recent promoted
ov065 sibling and the one on the live `src/actors/` convention.
`daObjCtMecha05_c` uses the older `src/game/actors/` path and is not the
placement precedent. `src/actors/daBmb_c.cpp` supplied the folded-factory shape,
since Mecha03 has no factory in its TU.

### Destructor ordering

`include/daYurei_Mucho_c.h` declares `virtual ~daYurei_Mucho_c() {}` inline, which
is what makes mwccarm emit the cartridge's D1-then-D0 pair (0x02115ee0,
0x02115f28) with no homeless D2, and what makes this the class's key-function TU.

The ordering hazard was respected: `linkcheck --baseline` builds the WORKING
TREE, so the baseline was taken while the header still carried the
declared-undefined form, and the inline body was restored in the same edit that
folded the shards out. No `object 'X::~X()' redefined` compile refusal occurred.

`defer_codegen off` was NOT needed: `tubuild verify` reports all 23 functions in
the expected ROM-ascending emission order on the first ordering tried.

### Declarations

`_ZTV15daYurei_Mucho_c` is taken from `include/decl_common.h`, which the tree
already carried for this class, rather than restated locally. Measured:
`langmode_audit` `codegen_hacks/extern_vtable` is 164 at base and 164 at head,
and `extern_vtable_classes` 167 and 167. No ratchet rise.

Six constructor entry points the folded factory calls are restated inside the
TU's own `extern "C"` block rather than reached by adding `decl_ActorBase.h`,
`decl_Enemy.h`, `decl_dCcAc_c.h`, `decl_dBgCh_Actr.h`, `decl_ModelAnim.h` or
`decl_ShadowModel.h`. Adding one of those headers to satisfy a single extern is
what silently undoes a deliberate declaration choice elsewhere in the file.

Three of ov065's four `.bss` State objects could not be given their real type
here: `decl_common.h`, which this TU takes the vtable declaration from, already
declares 0x0211d650 and 0x0211d660 as `char[]` and 0x0211d680 as a bare `int`,
and a second differing declaration is a compile error. They are cast at each use.
Only the address ever reaches the code, so the cast costs no bytes.

The five-Vector3 stack aggregate in `func_ov065_02116364` was given a file-scope
name, `V3Quint`. As an unnamed local class mwccarm mangles its implicit
destructor with a file-and-counter tag that moves whenever the file moves, so the
manifest row licensing it would not have survived the move out of `src_tu/`. The
emitted bytes are unchanged. Same remedy as `V3Quad` in `ov002/Player`.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 23 of 23 declared functions MATCH
  under `tools/match.py` extract+compare; `objisolate.py plan()` clean on
  relocation type and addend; `reloc_audit.py check_destinations()` clean;
  emission order as expected. The TU claims `.text` only, so the byte gate says
  nothing about data -- `romdata_check` covers that, below.
- Genuine methods vs bridges: 10 of the 23 are compiler-spelled
  `_ZN15daYurei_Mucho_c*` members. 12 remain `func_ov065_*` free functions inside
  the TU, plus the `extern "C"` factory.
- Recovered layout: `include/daYurei_Mucho_c.h` is 103 lines with 2 `unk_` fields
  and 3 `pad_` runs. The class size 0x3e4 is the ROM's own allocation literal, not
  a rounded field span. `src/actors/daYurei_Mucho_c.cpp` still matches `tiers.py`'s
  `RAW_OFFSET` regex on 89 of its 888 lines (94 occurrences).
- The `State` struct is now typed: both words are pointers-to-member, evidenced by
  the module's static initializer copying each object in as two 8-byte halves from
  the `.data` records at 0x0211cb20..0x0211cb60, and by the two hooks being invoked
  from different callers. The names `mEnter` and `mMain` are COINED; the offsets,
  the encoding and the `int` return type are measured.

## Proof

All commands run in `C:/tmp/sm64ds-sm64ds-yurei` at this tree, on the pinned
`tools/mwccarm/2004/b56/mwccarm.exe`.

- **Full ROM build, head**: `python tools/rombuild.py -j16` -- 11,192 source-built
  functions, 11,192 reproducing, **0 mismatching** (2,140,712 / 2,238,108 code
  bytes, 95.65%); module fidelity **106/106 exact, 100.000000%** of compared bytes;
  intact TU gates "dsd modules PASS, zero new symbol errors, storage aliases exact";
  ROM data from source 696 verified / 220 partial / 4 differ / 397 unnamed.
- **Full ROM build, base** at `26f54f8fc` in the second worktree, regenerated, not
  reused: 11,192 reproducing, 0 mismatching, 106/106 exact, 100.000000%; ROM data
  695 / 219 / 4 / 399. The +1 verified and -2 unnamed come from the rename
  collapsing the duplicate vtable alias, and `compiler-emitted data bytes verified`
  rises 44,560 -> 44,572, the new typeinfo record.
  The baseline control in the same run reports **9** `dsd check symbols` error
  lines with NO TU substitution -- `overlay_100`, `overlay_102`, `data_020ad524`,
  `data_020ad560`, `func_01ff98f4`, `func_01ff99a4`, `func_01ff9d40`, `_deq`,
  `func_01ff9e2c`, all arm9/ITCM. By that control's own definition they belong to
  the tree, and the operative comparison (zero NEW symbol errors) passed.
- **`tools/tubuild.py verify ov065/daYurei_Mucho_c`** -- "23/23 MATCH, objisolate
  clean, reloc-destinations clean -> TEXT-VERIFIED", all 23 in expected
  ROM-ascending emission order, 13 compiler-only symbols all licensed.
- **`tools/linkcheck.py` driven BY SYMBOL** (not by file stem): all 23 symbols
  VERIFIED, 0 diffs, 0 blind slots, run as
  `--module ov065 --c src/actors/daYurei_Mucho_c.cpp --name <symbol> --addr <addr>
  --size <size>`. This is the gate that proves the corrected callee and the
  address-point bias link to the cartridge's exact bytes.
  Coverage limit stated plainly: `prepush_linkcheck.py --range 26f54f8fc..HEAD`
  exits 0 (912 checked, 618 verified, 294 warnings, **0 blocking**) but reports
  this TU itself as `[WARN] daYurei_Mucho_c NO-SYM`, because it resolves by file
  stem and a promoted TU defines many symbols. That gate proves nothing about this
  TU; the by-symbol run above is the proof.
- **`tools/romdata_check.py --files src/actors/daYurei_Mucho_c.cpp`** -- 11 emitted
  data symbols: VERIFIED 6 (180 bytes), PARTIAL 5 (52 bytes equal), **DIFFERS 0**,
  UNNAMED 0. The six verified are the four inherited typeinfo records, this class's
  own `_ZTI15daYurei_Mucho_c`, and **`_ZTV15daYurei_Mucho_c`, all 124 bytes of it --
  31 slots byte-exact at 0x0211cba4**, which independently confirms both the
  address point and the slot count. The five PARTIALs are the `_ZTS` type-name
  strings, short of the ROM's named extent by trailing alignment.
  CONTROL: `src/actors/daBmb_c.cpp` (landed, merged) reports 6/5/0/0, and
  `src/actors/daObjCtMecha03_c.cpp` (landed, merged) reports 6/5/0/0. Identical
  shape.
- **`tools/validate_merge.py --base origin/main --head HEAD`** with both ROM
  reports -- module fidelity 106/106 exact; byte-verified functions and code bytes
  both **+0**; **contributor credit 0 added, 0 changed, 0 lost**. It warns that
  "23 address range(s) left the byte-verified set while enrolled totals held
  steady", naming the 23 folded ranges.
  CONTROL: the same tool over the landed, merged `daBmb_c` promotion
  (`e4fb1300e^..e4fb1300e`) emits the identical warning for its 35 ranges. It is
  the signature of a consolidation, not a regression. Re-running with
  `--base 26f54f8fc` gives the same credit line.
- **`tools/premerge_check.py <head>`** -- all eight static gates pass on both base
  and merge tree: converted-ratchet, dead-references, duplicate-sources,
  header-offsets, langmode-ratchet, layout-check, src-tu-refs, source-coverage.
  "nothing goes green -> red". Its source-coverage note reads
  "entries: 9066 -> 9044 (-22) (consolidation, not a loss -- bytes are flat)".
- **`tools/check_rename_ledger.py`** -- "every mangled/vtable row agrees with its
  module's symbols.txt", 2007 rows checked. Coverage limit stated by the tool:
  1532 coined rows are out of scope and unchecked.
- **`tools/check_dead_references.py`** -- "no new dead references", no broken
  markdown links. Three baselined prose references and one baselined C/C++ comment
  reference now RESOLVE, which shrinks the baseline; `--update` was deliberately
  not run, as it needs a clean pinned main checkout.
- **langmode ratchet** -- `langmode_audit.py --check` against the banked
  `chaos-data` baseline: PASS. Every differing metric FELL.
- **`tools/tiers_ratchet.py --check`** -- "CONVERTED ratchet PASS, baseline 2699,
  current 2702, +3 gained, not yet banked". `--update` was NOT run: it appends, so
  a re-run double-banks rows permanently.
- **`tools/layout_check.py`** -- clean. **`tools/port_refcheck.py`** -- 423
  references checked, all resolve. **`tools/eligible.py`** -- 11197 / 11273.
  **`tools/cpp_tu_state.py`** -- exit 0.
- **`tools/queue_audit.py`** -- reports exactly the counts it reports on the base
  commit: already_promoted 3, compiler-only 2, shard_count 3, total_lines 3,
  unmatched 1. Measured both sides. Those five rows belong to other classes'
  landed promotions and were deliberately not rewritten here.

### The two gate results that were NOT clean, with their controls

1. **`tools/tubuild.py linkcheck ov065/daYurei_Mucho_c` refuses** at step 4/8:
   "isolate: intact TU preparation refused: ov065/daYurei_Mucho_c: intact
   production requires one .text claim and at least one non-text claim".
   CONTROL: the identical command on `ov065/daObjCtMecha03_c` -- landed, merged,
   also text-only, also `.text`-only -- fails at the same step with the identical
   message. This is the scratch-isolation path, not this TU. The intact-object
   link gate that does apply is the one inside `rombuild.py` step 4/6, which
   passed with "zero new symbol errors, storage aliases exact".

2. **`tools/prepush_attribution.py --base 26f54f8fc` reports 2 CREDIT LOST** --
   `d_a_yurei_mucho` [tangosdev] and `_ZN6Snufit13OnYoshiTryEatEv` [andrewboudreau]
   -- alongside 17 consolidated-with-credit-intact and 1 renamed-with-credit-intact.
   CONTROL: the same tool over the landed, merged `daBmb_c` promotion reports
   **13 CREDIT LOST**, including `d_a_bmb`, the exact analogue of `d_a_yurei_mucho`.
   `validate_merge`, which is what actually gates the merge, reports
   **0 added, 0 changed, 0 lost** on this branch against both bases.
   The two gates key differently -- `prepush_attribution` on the OLD basename stem,
   `validate_merge` on the CURRENT symbol name -- so neither covers the other. No
   override row was invented to silence the stem-keyed report; the landed precedent
   does not carry one either.
   Independently checked: the owner banked for every one of the 23 symbols equals
   that symbol's owner at `26f54f8fc`, computed through
   `prepush_attribution.lineage` on both revisions. The gate passed and no
   contributor's totals moved.

## What is deferred, and why

- **The 12 free helpers keep their address-derived names.** Eight of them are
  reached ONLY through the pointer-to-member records at 0x0211cb20..0x0211cb60,
  and `dsd` resolves those `.data` records BY NAME out of
  `config/arm9/overlays/ov065/symbols.txt`. Renaming one is therefore a
  same-commit symbols.txt edit, and no byte gate would catch a miss -- a PMF word
  is relocated, so `match.py` wildcards it. The evidence for individual names is
  not strong enough to spend that risk on in this stage. This is the single
  largest remaining improvement for the class.
- **89 raw-offset lines** (94 occurrences) remain in the promoted source, and two
  `unk_` fields and three `pad_` runs remain in the header. "Promoted" is a
  packaging state; this class is not a finished reconstruction.
- **The five `_ZTS` PARTIALs** are an extent question in symbols.txt, not a class
  model question, and match the landed siblings exactly. Closing them belongs to a
  separate change.
- **The scout's dossier is renamed, not rewritten.** `notes/data/class-facts/`
  now spells it `daYurei_Mucho_c.json`, following the landed `daBmb_c.json`
  precedent (renamed from `BobOmb.json`), and its `class` field carries the ROM
  RTTI name. Its `queue_key`/`tree_name`/`former_tree_name`/`name_status` block
  now says in the file what changed. Every tree-derived field below that -- the
  22 shard paths, the `_ZN6Snufit...` symbols, `symbols_txt_rows`, `queue_row` --
  is left exactly as measured: it is a dated observation at
  `tree_derived.base_commit`, and rewriting it would falsify a dated artifact.
  `name_status` says so explicitly so a reader cannot mistake it for a live
  claim. The same reasoning still leaves `notes/data/tu-merge-candidates.json`
  and `notes/data/c-cpp-classification.tsv` alone; `check_dead_references` is
  green over both.
- **The coined free function `Snufit_Kill` is retired too**, so the overlay does
  not carry a retired class name in one spelling and its ROM name in another.
  `extern int Snufit_Kill(int*);` was a dead declaration in
  `include/decl_common.h` -- no definition, no caller, and no such symbol in
  `config/arm9/overlays/ov065/symbols.txt`, which spells the function
  `func_ov065_021177e4`. It is deleted. It was NOT renamed to
  `daYurei_Mucho_c_Kill`, because the shard's own attribution turned out to be
  wrong on both halves, refuted by three ROM reads recorded in the shard's
  header comment: 0x021177e4 is in no vtable in ov065 (neither
  `_ZTV15daYurei_Mucho_c` at 0x0211cb9c nor `_ZTV5Swoop`/`_ZTV12daBasabasa_c` at
  0x0211cc98 contains it); it sits inside the neighbour's run between
  `_ZN5SwoopD0Ev` (0x02116fe8) and `_ZN5Swoop16CleanupResourcesEv` (0x02117aa4),
  past this class's end at 0x02116f98; and its body calls `ModelAnim::SetAnim`
  on `t + 0x364`, where `daYurei_Mucho_c` holds a `ShadowModel`, while reading
  `data_ov065_0211d6a0` from outside this class's bss band
  0x0211d600..0x0211d690. Naming it belongs to the `daBasabasa_c`/`Swoop` owner.

## Next action

Independent verification (verifier), then integration. No PR was opened. The
branch is `cpp/daYurei_Mucho_c-tu`; its head SHA is recorded by the queue.

## Source review corrections, 2026-09-10

The earlier proof is historical. This review continues PR #2456 at input
`a6ed71f26e750fadb84aeb4c55a8844d994fc502` under
`pr2456-source-repair-0910`; final acceptance also requires current-main
composition and independent review. The class remains a partial reconstruction
owned by the humanizer/integrator support session through @andrewboudreau in
[issue #2478](https://github.com/tangosdev/sm64ds-decomp/issues/2478).

YUR-02 corrects the tail layout. `mShotCount` is the signed word at +0x3dc,
cleared by the attack entry and compared with three/incremented by the attack
main helper. `mTargetAngle` is the signed halfword at +0x3e0, including the
old +0x300+0xe0 accesses. Only +0x3e2..+0x3e4 remains unobserved. The names
are reconstructed; the accesses, widths and 0x3e4 allocation size are measured.
Both fields replace the earlier padding, and the helpers now use their class
members. The appended class-facts correction supersedes the incorrect unread-tail
claim without rewriting the historical scouting snapshot.

YUR-03 corrects the State provenance. The two 8-byte PMF records and their caller
roles are measured. `mEnter`, `mMain` and their common `int` signatures are
reconstructed interfaces. The dispatcher forwarding r0 does not establish an
original C++ return type; the retained attack-entry helper is declared `short`.
The earlier statement that the `int` types were measured was incorrect.

YUR-04 replaces the exact `UpdateYoshiEat(mWithMeshClsn)` call with the existing
member interface and removes its redundant mangled declaration. All three methods
in question were already declared in the included headers. The remaining Init
calls have these measured constraints under 2004/b56:

| Alternative | Result |
| --- | --- |
| Real `dEnemyBase_c::UpdateYoshiEat` | 23/23 functions VERIFIED |
| Real `dCcAc_c::Init`, aggregate-initialized `Fix12<int>` radius/height | InitResources grows 0xfc to 0x10c; constants are copied from compiler-emitted data |
| Real `dCcAc_c::Init`, assigning radius.val and height.val | InitResources grows 0xfc to 0x114 |
| Real `dBgCh_Actr::Init` with its current scalar parameters | Body remains 0xfc, but emits `_ZN10dBgCh_Actr4InitEP8dActor_ciiP10Vector3_16S3_`, with no configured destination; BLIND-1 |

The existing ABI calls stay pending a coherent shared-interface repair. No alias
or shared declaration was invented to disguise the unresolved destination.

YUR-05 corrects the copy and metadata explanations. `Matrix4x3` is available
through the model headers, and `Vector3` is available in types.h. At this include
order, Matrix4x3 contains the existing non-POD Vector3. Replacing Mtx43's POD
copies grows func_ov065_0211696c from 0x17c to 0x1c8; replacing the V3A copy
with Vector3 grows func_ov065_02116364 from 0x224 to 0x234. The small POD views
remain documented matching constraints, not evidence that project types are absent.
The ordinary subobject constructors also exist; the manual factory is unfinished
reconstruction, not a requirement caused by missing constructors.

The inline destructor spelling reproduces D1/D0. The TU defines the class's
out-of-line virtual methods and emits its metadata; the inline destructor itself
is not the key function. The header, source and manifest now distinguish these
facts. The original long factory/vtable narrative remains available in the
input commit and the earlier address-point discussion above; the source retains
only the two-word vtable preamble adjustment it needs.

The combined class-local changes reproduce all 23 functions, 4280 text bytes,
with zero differing or blind words on the original source base. Raw emitted
metadata remains 6 VERIFIED / 5 PARTIAL, no DIFFERS. Scratch experiment artifacts
are `build/probes/*/strict.json`, `build/probes/*/candidate.o`, and
`build/class-fixes-proof.json` in `C:/tmp/sm64ds-yureifix-0910`.
These are class-local experiment results, not final current-main acceptance.

The 12 address-named free helpers, manual factory, remaining raw storage and
copy views continue under #2478. SharedFilePtr still has no recovered fields in
its header: retain the measured direct +4 file-pointer load rather than calling
LoadFile or inventing a field. The neighboring func_ov065_021177e4 belongs to
daBasabasa_c; its previously accepted provenance-only correction is preserved.
