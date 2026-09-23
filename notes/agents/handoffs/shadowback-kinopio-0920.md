# shadowback-kinopio-0920

Producer: `codex-shadowback-0920` (Codex). Initial source and workflow base:
`077483245976d72477eba0d2a7ccca4390da2b1b`. Branch: `fix/shadow-backports-0920`.
This records the candidate prepared for the user-requested draft PR. The producer
lease is held in the v2 task named above. No successful verification handoff has
been published through the queue; `formal_acceptance: false`.

The current scope is one already-enrolled production file. Shared headers, symbols,
delinks, attribution, lifecycle definitions and the shadow TU are unchanged.
No whole-TU promotion or completion of the parent reconstruction is claimed.

## Current 24-file production candidate

Source candidate: `9b825c3d3c0ff0af5f04ff1151649069a3872136`. Tested composition base:
`5c075024562a469c6e4336d8540e3dfa9ffa84a1`, still the live main when this continuation began.
The user approved retrying the exact 18-helper proposal. Its six BookShot and
12 daTgz_c file/range reservations succeeded under the existing producer tasks;
no full-class ROM range or extra source scope was acquired. The original workflow
input and stage artifact lists remain unchanged. Source-review coverage follows
all amended file resources, not the original initializer-only artifact list.

The combined candidate contains 24 existing production files and covers 6,956
function bytes: the prior six methods plus 18 helpers (14 C++ and four C files).
The state-3 helper change is comment-only. No file was moved or renamed, and no
header, compiler configuration, manifest, delink or baseline was changed.

- All 24 strict relocation checks pass: VERIFIED, empty differences, zero blind
  relocations. The independent verifier freshly compiled both base and candidate
  for all 18 helpers and compared whole objects; all are byte-identical. The prior
  six-file independent proof was retained only after confirming its source,
  header and tool inputs remain unchanged.
- Five objects carry an unchanged four-byte Vector3 destructor passenger:
  Toad St_Talk_Main and helpers 02111fc4, 02124d08, 021253a4 and 02125550.
  All defined symbol bindings were inspected, including STB_LOPROC.
- Full `rombuild.py -j 16 --no-rom`: PASS; 11,208/11,208 source-built functions,
  106/106 exact modules, 26/26 initialized-data claims and three BSS claims.
  7,987 cached objects and 18 compiles; candidate ROM packaging skipped.
  Intact-TU gates retain zero new symbol errors. Broad emitted-data diagnostics
  remain three differing and 296 unnamed records, unaffected by these
  object-identical changes.
- Port: 418/418 references resolve. Attribution: 8,182 tracked, zero changed/lost.
  Language-mode and CONVERTED ratchets pass against the unchanged baselines;
  CONVERTED remains 2,990 against 2,973. Whitespace check passes.
- Independent BookShot and daTgz_c source reviews found no new source blocker,
  subject to the recorded partial-reconstruction limits. This does not establish
  full class reconstruction, TU promotion or formal Source review acceptance.

### Open declaration gate: SHADOWBACK3-DECL-01

`check_decl_agreement.py --changed 5c075024562a469c6e4336d8540e3dfa9ffa84a1` fails on
one newly reported return mismatch in `func_ov077_02125a54.c`. Its corrected
`int _Z14ApproachLinearRsss(short*, short, short)` declaration agrees with the
actual native `int ApproachLinear(short&, short, short)` definition in
`src/_Z14ApproachLinearRsss.cpp`. That definition compiles to the exact linker
symbol and independently passes strict comparison at `arm9:0x0203adec`, 108 bytes.
The checker does not associate the unmarked native definition with its mangled
name, so it instead votes among older declarations and chooses void.

An independent, in-memory annotation experiment identifies the real definition,
but then exposes 185 unbanked diagnostic rows (179 unique keys across 114 files),
including a literal comparison of the C pointer with the C++ reference spelling.
There is no supported narrow override for this caller. The correct int return is
retained. No annotation, checker change, baseline entry or incorrect void rollback
was applied. Definition recognition and cross-language reference comparison need
separate scoped work; this gate remains failed and the candidate is not claimed
merge-ready. Byte correctness and source plausibility remain separately passed.

Current producer evidence: `build/shadow-backport3/checks.json`, `baseline.json`,
`gates.json`, `declarations.log`, `rombuild-report.json`, `rombuild.log`,
`romdata.json`, `attribution.json`, and `approach-linear-definition-proof.json`.
Independent byte evidence is in the verifier's `build/shadow-backport-review3/`.
The source and this handoff are prepared for draft-PR publication at the user's
request. Raw local evidence and private compiler objects remain untracked. The
declaration blocker is retained; `formal_acceptance: false`.

The Toad source is unchanged by the helper continuation. Its previously
verified source, headers and tool inputs were checked for identity before
retaining its independent byte proof in this combined candidate.

## Earlier backport description and proof

Backport the star-ID argument correction from shadow PR #2829 into
`daKinopio_c::St_Talk_Main`, then in its own file and now in
`src/game/actors/d_a_kinopio.cpp`. Both the declaration and call now say
that `IsStarCollectedInCurLevel` receives `int starID`, agreeing with its actual
production definition in `src/IsStarCollectedInCurLevel.c` and the existing
`daKinopio_c::InitResources` caller.

The old zero-argument declaration happened to reproduce the retail instructions
because the preceding comparison already left the star ID in the argument
register. The source now expresses that real dependency. The function's existing
`opt_propagation off` pragma and other reconstructed interfaces are unchanged.
This two-line repair does not claim whole-TU promotion or repair the remaining
Toad declarations, fields or lifecycle ownership.
Source lineage: merged PR #2829, commit `2c8493b80`.

The Toad source is unchanged by this continuation. It was recompiled and
strictly verified as part of the six-function candidate against the refreshed
composition base; its existing Vector3 destructor passenger is still present
and unchanged.

## Previous six-function candidate (historical proof)

Current composition base: `5c075024562a469c6e4336d8540e3dfa9ffa84a1` (merged locally before
this continuation). Source candidate: `a5edafd2280d93bb61c002b9ca2849ec96e53341`.
The original workflow input remains `077483245976d72477eba0d2a7ccca4390da2b1b`;
its three-function evidence below is retained as history. Current production
source changes are BookShot InitResources/Behavior, daTgz_c
InitResources/Behavior/Render, and daKinopio_c St_Talk_Main. All six are enrolled
in the normal `src/` production build.

- Strict `prepush_linkcheck.py --range 5c075024562a469c6e4336d8540e3dfa9ffa84a1..a5edafd2280d93bb61c002b9ca2849ec96e53341`:
  six checked, six VERIFIED, zero warnings or blocking. Explicit per-function
  checks also report `diffs: []`, `blind: 0`.
- Independent compilation of all six sources at this source commit and the
  exact composition base: complete unisolated objects are byte-identical.
  All defined symbol bindings were inspected. The only extra nonzero function
  remains the existing four-byte Vector3 destructor in the Toad object.
- `rombuild.py -j 16 --no-rom`: PASS; 11,208/11,208 source-built functions,
  106/106 exact modules, 26/26 initialized-data claims and three BSS claims.
  8,001 object-cache hits, four fresh compiles. Candidate ROM packaging skipped.
  The newly generated stock control retains nine symbol errors; intact-TU gates
  introduce zero new symbol errors. The broad emitted-data diagnostic retains
  three differing and 296 unnamed records; these six object-identical changes
  introduce no emitted-data change.
- Declaration ratchet: no new disagreements. Port references: 418/418 resolve.
  Attribution: 8,182 tracked, zero changed or lost. Whitespace: pass.
- Language-mode ratchet: PASS against the preserved chaos-data baseline at
  `1203652ceff9b6e2c82e4d130762bb7366e31869`; CONVERTED ratchet: PASS,
  baseline 2,973 / candidate 2,990. No baseline was edited.
- Independent source/object review at the source commit found no source blocker.
  Finding SHADOWBACK2-REVIEW-01 identified the outdated one-function handoffs;
  this continuation section and corrected scope address that finding. Final
  review of this documentation-only successor is recorded in the verifier's
  local report. Private CI and formal queue review publication have not run;
  `formal_acceptance: false`.

Current local producer evidence is under `build/shadow-backport2/`:
`checks.json`, `gates.json`, `secondary-gates.json`, `rombuild-report.json`,
`rombuild.log`, `romdata.json` and `attribution.json`. Independent controls and
review live in the separate wired verifier's `build/shadow-backport-review2/`.
At this historical checkpoint the additional 18 helpers were still a read-only
proposal after automatic approval review rejected their reservation. The current
24-file section records the later explicit approval and completed backports.

## Initial slice proof (077483 base)

- Scope: `ov085:0x21291ac`, 656 bytes,
  `_ZN11daKinopio_c12St_Talk_MainEv`.
- Before and after explicit `tools/linkcheck.py --c <source> --name <symbol>
  --addr <address> --size <size> --module <module>`: `VERIFIED`, `diffs: []`,
  `blind: 0`.
- Complete unisolated compiler object: 2320 bytes, byte-for-byte
  identical to the source-base object. Both SHA-256 values:
  `97dfe933cced8e873450bedbc417b1143ef9deb19fb45fc96111f9774120f7a1`.
  The object also defines the existing four-byte `_ZN7Vector3D1Ev` compiler
  passenger. It is unchanged from the base; no new lifecycle/RTTI/vtable output
  was added.
- Compilation uses `2004/b56` and `rombuild.CFLAGS` with `-lang c++`.
- Three-file prepush linkcheck: 3 checked, 3 verified, no warnings or blocking.
- `check_decl_agreement.py --changed 077483245976d72477eba0d2a7ccca4390da2b1b`:
  no new declaration disagreements; no baseline was edited.
- `port_refcheck.py`: 418 references, all resolve. `git diff --check`: pass.
- `rombuild.py -j 16 --no-rom`: PASS. 11,208/11,208 source-built functions
  reproduce; 106/106 modules exact; all 26 owned initialized-data claims exact,
  plus three BSS claims. 8,021 object-cache hits and exactly these three fresh
  compiles. Final candidate ROM packaging was skipped. The stock control retains
  nine symbol errors, and intact-TU comparison introduces zero new errors.
  The broad emitted-data report still has 3 differing and 298 unnamed records;
  none is emitted by these three code-only, object-identical changes.
  Log: `build/shadow-backport/rombuild.log`; structured report:
  `build/shadow-backport/rombuild-report.json`.
- Language-mode check against chaos-data `c2a172f73a18dc02b8f416f8fe598341cbc28b7f`:
  reports `extern_vtable` 104 -> 105 and classes 105 -> 106. The independent
  clean worktree at the exact source base reproduces both failures. This is an
  unchanged cached-baseline failure. After refreshing the live chaos-data
  baseline at `1203652ceff9b6e2c82e4d130762bb7366e31869`, the language-mode
  ratchet PASSES. The CONVERTED ratchet passes at 2989 against 2973, identically
  on the base and this candidate. No repository baseline was edited.
- The initial-slice independent review later completed at
  `94c021afa4305bacd224f3ef1cbb523e4a6e9633`; its local report is retained in
  `build/shadow-backport-review/`. Private CI was not run.

Producer reservations are retained for continuation. Remaining full-class
reconstruction stays with its existing follow-up issue and ownership. Initial
reports remain under `build/shadow-backport/`; current reports are listed above.
Compiler objects are private ignored output and must not be committed or published.
