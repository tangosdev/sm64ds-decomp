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

That measurement is load-bearing in the source. The retired factory shard wrote
the bare `(int)_ZTV6Snufit`, which was right while the symbol was UNDEF and
symbols.txt bound it to the address point. In the promoted TU the same spelling
binds to this TU's own definition, two words lower, so the store is written
`(int)(_ZTV15daYurei_Mucho_c + 2)`.

**A relocated word is a wildcard to every byte gate, so `match.py` reported
23/23 MATCH with the bare form too.** `tools/objisolate.py`'s addend check is
what refused it, with `_ZTV15daYurei_Mucho_c: unexpected reloc type=2 addend=0`.

That message was expected to be a known false positive. It was not. The control
is recorded under Proof.

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
- **The scout's dossier was deliberately left untouched**: it is a dated record of
  what was known before the rename, and rewriting it would falsify a dated
  artifact. The same reasoning left `notes/data/tu-merge-candidates.json` and
  `notes/data/c-cpp-classification.tsv` alone; `check_dead_references` is green
  over both.
- **`include/decl_common.h` still declares `extern int Snufit_Kill(int*);`** at
  line 408. That is a phantom declaration for `func_ov065_021177e4`, which lies
  inside the RESERVED neighbour's run, so it was left for that owner.

## Next action

Independent verification (verifier), then integration. No PR was opened. The
branch is `cpp/daYurei_Mucho_c-tu`; its head SHA is recorded by the queue.
