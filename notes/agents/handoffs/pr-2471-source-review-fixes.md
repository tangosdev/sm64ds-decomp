# PR #2471 declaration-agreement corrections

## September 10 parser continuation

This owned producer stage continues the preserved checkpoint
`f2bf9cea85c7144580566c432a125860a7151d80` under session
`codex-r2398-declgate-fix-0910`, in `C:/tmp/sm64ds-r2471-0910`.
The original source base and prior review history below remain preserved.
Only the checker, its tests and this handoff change in this stage.

Four independently reproduced false passes are repaired:

- Braced data initializers retain every later comma-separated object, including
  nested initializers, pointers and call-valued scalar initializers.
- A leading `@symbol` marker names only the first object in its statement.
- Static functions cannot supply an external symbol's reference signature.
- Unmarked namespace C++ functions cannot supply a flat global identity. Explicit
  C linkage and marked namespace definitions retain their proven identities.

Brace nesting is enabled only for data declarator parsing. Existing typedef,
parameter and other token normalization keep their previous behavior. No scan
floor, gate scope, baseline, source, header or executable workflow rule changes.
The earlier corrected explanation of reconstructed interface consistency remains
intact; this checker does not establish original-source identity or byte accuracy.

Validation of this parser revision:

- All 70 tests pass, including four new full-size Git controls. Each uses the
  production scan floors with 2,000 padding files and 6,000 declarations: clean
  input passes, and each incorrect mutation fails both full and changed gates.
- Python-name checks pass with no unresolved names, syntax failures or advisories.
- The actual full-tree gate still exits 1. It scans 9,635 files and 39,980
  declarations, with 69 data definitions and two unparsed declarators retained.
- The unchanged baseline has 23,049 unique keys; the corrected parser observes
  23,056, with 12 added and five removed. Relative to the preserved parser at
  `f2bf9cea`, five parameter keys replace two incorrect arity keys. Leading symbol
  markers previously attached to static G3 helpers in `func_ov007_020caeac.c` and
  `func_ov075_0211a948.c`; excluding those helpers selects the actual external
  definitions and exposes their callers' parameter disagreements.

The read-only inventory is `build/declgate-final-inventory.json`; the test log is
`build/declgate-tests-final.log`. The original failing controls remain under
`build/comparison2471/`. The producer evidence records the immutable output and
artifact hashes. No ROM build is required or claimed for this tooling change.

### Exact diagnostic delta on the preserved source tree

Each key is `(symbol, file, kind:got)`; line numbers identify the inspected source
but are not part of the key. **New** means a real mismatch between two existing
source interfaces that the earlier parser did not report. **Re-keyed** means an
existing reported inconsistency now uses the corrected definition or identity.
These are source consistency classifications, not proof that a particular type
is the correct ROM reconstruction or that the mismatch changes retail bytes.

| Symbol | Declaring source | Added key | Selected reference and expected type | Classification |
|---|---|---|---|---|
| `data_020a4b6c` | `include/decl_common.h:855` | `return:int` | `src/_ZN7fBase_c21AfterCleanupResourcesEj.cpp:16`: `char` | Re-keyed |
| `data_020a4b6c` | `src/func_02044120.c:2` | `return:int` | `src/_ZN7fBase_c21AfterCleanupResourcesEj.cpp:16`: `char` | Re-keyed |
| `data_020a4ba8` | `src/func_02043880.c:41` | `return:int` | `src/_ZN7fBase_c21AfterCleanupResourcesEj.cpp:18`: `char` | Re-keyed |
| `data_020a4ba8` | `src/func_02044120.c:2` | `return:int` | `src/_ZN7fBase_c21AfterCleanupResourcesEj.cpp:18`: `char` | Re-keyed |
| `data_ov045_02112fdc` | `include/decl_common.h:1129` | `return:int []` | `src/game/actors/d_a_obj_km2_gura.cpp:29`: `GuraResourceDescriptor` | New |
| `data_ov100_02148390` | `include/decl_common.h:1670` | `return:signed char []` | `src/game/actors/d_a_star_gate.cpp:97`: `daStarGateInfo []` | New |
| `data_ov100_02148948` | `include/decl_common.h:38` | `return:Vector3` | `src/game/actors/d_a_star_gate.cpp:71`: `StarGateVector3` | Re-keyed |
| `func_ov007_020caeac` | `src/func_ov007_020ca5f0.c:5` | `param:#1 int` | `src/func_ov007_020caeac.c:34`: `#1 Vector3 *` | Re-keyed |
| `func_ov007_020caeac` | `src/func_ov007_020ca5f0.c:5` | `param:#2 int` | `src/func_ov007_020caeac.c:34`: `#2 Vector3_16 *` | Re-keyed |
| `func_ov007_020caeac` | `src/func_ov007_020ca5f0.c:5` | `param:#3 int` | `src/func_ov007_020caeac.c:34`: `#3 int *` | Re-keyed |
| `func_ov007_020caeac` | `src/func_ov007_020ca5f0.c:5` | `param:#4 int` | `src/func_ov007_020caeac.c:34`: `#4 int *` | Re-keyed |
| `func_ov075_0211a948` | `src/func_ov075_0211afb0.c:24` | `param:#1 void *` | `src/func_ov075_0211a948.c:34`: `#1 char *` | Re-keyed |

All five removed keys are reference/identity corrections; no source was healed:

| Symbol | Source file and line | Removed key | Reason |
|---|---|---|---|
| `data_020a4b6c` | `src/_ZN7fBase_c21AfterCleanupResourcesEj.cpp`:14 and 16 | `return:char` | Recognized char definition at line 16 replaces the int declaration plurality; two existing int declarations now receive the keys. |
| `data_020a4ba8` | `src/_ZN7fBase_c21AfterCleanupResourcesEj.cpp`:18 | `return:char` | Recognized char definition replaces the int declaration plurality; two existing int declarations now receive the keys. |
| `data_ov100_02148948` | `src/game/actors/d_a_star_gate.cpp`:70; definition 71 | `return:StarGateVector3` | Recognized StarGateVector3 definition replaces the Vector3 declaration plurality; the existing header disagreement receives the key. |
| `func_ov007_020caeac` | `src/func_ov007_020ca5f0.c`:5 | `arity:13` | Marker previously selected private G3_Vtx at src/func_ov007_020caeac.c:18; actual external definition at line 34 has the same arity as the caller but four parameter mismatches. |
| `func_ov075_0211a948` | `src/func_ov075_0211afb0.c`:24 | `arity:1` | Marker previously selected private G3_TexImageParam at src/func_ov075_0211a948.c:22; actual external definition at line 34 has the same arity as the caller but one parameter mismatch. |

The two **New** rows are concrete source interface inconsistencies:
`GuraResourceDescriptor` contains three pointers (`d_a_obj_km2_gura.cpp:17`), while
its header extern claims an integer array. `daStarGateInfo` contains two signed
bytes and two signed halfwords (`include/daStarGate_c.h:13`), while its header
extern claims a signed-byte array. Their corrective source scope and byte/consumer
proof must be established separately. The other ten additions relocate or refine
existing diagnostics and require a proven migration or source repair rather than
an undifferentiated baseline reset.

The gate remains red until this integration issue is resolved. The orchestrator
will arrange the source repairs or a narrowly proven diagnostic migration after
independent review; this handoff does not transfer the unresolved work to an
unspecified future owner. The machine-readable classification is
`build/declgate-diagnostic-triage.json`, supported by
`build/declgate-reference-migration.json`. The baseline is unchanged.

Finding IDs remain `declgate-shared-type-scope`, `declgate-config-scope`,
`declgate-renames`, `declgate-data-definitions`, `declgate-baseline`,
`declgate-source-provenance` and `declgate-function-ownership`. The parser and
provenance corrections are offered for independent verification. The baseline
finding remains open: the baseline was not edited, and this checkpoint is not a
passing full-tree gate or merge acceptance. The earlier seven-addition,
three-removal baseline proposal below is historical and is not the current delta.
Do not apply that patch or narrow acceptance to make this checkpoint pass.

## Preserved September 8 handoff

This continues the existing tooling PR; it does not start class reconstruction.
The original PR branch and its independent review checkout remain preserved.

- Task: `pr-2471-source-review-fixes`, [PR #2471](https://github.com/tangosdev/sm64ds-decomp/pull/2471).
- Producer: `codex-declgate-fixes-producer-20260908`.
- Accepted input: `0418f92541c1e2db2fb67304b1442aec73ae395a`.
- Preserved source base: `c1c8846df622f3a49b6b107e0845e9929473d059`.
- Task workflow: `f327f7b6460e157153eb7fc0749dbbe60dd854f1`.
- Continuation branch: `fix/decl-agreement-review-0908`.
- Producer checkout: `C:/tmp/sm64ds-decl-agreement-review-0908`.

## Corrections

Header, symbol-table, gate-tool and baseline changes now require a whole-tree
verdict, so a changed shared typedef or symbol record cannot discard findings in
untouched callers. Git diff collection retains both sides of renames and removed
source paths; removed source also requires the whole-tree verdict.

The parser now records externally visible initialized and tentative data objects,
including array initializer heads. Function declarations naming data definitions
and incorrect data types therefore reach comparison. Internal static/const objects
are excluded. Qualified and namespace data require a linker-identity marker;
aggregate members and static assertions are not declarations of external symbols.

No file under src/ or include/ changed. No byte, relocation, attribution or class
ownership claim is being changed by this tooling continuation.

## Historical baseline delta awaiting authorization

The initial baseline remains unchanged at this checkpoint. Automatic approval
review rejected its write because changing the tolerated disagreements requires
trusted user authorization for the exact delta. No workaround was attempted.
The ignored review artifact `pr2471-fix.baseline.patch` in the coordinator's
review-watch directory contains the concrete proposed change.

The repaired parser finds 69 external data definitions. Against the unchanged
accepted source, it adds exactly seven baseline keys and removes three:

| Symbol | Declaring file | Added key |
|---|---|---|
| data_020a4b6c | include/decl_common.h | return:int |
| data_020a4b6c | src/func_02044120.c | return:int |
| data_020a4ba8 | src/func_02043880.c | return:int |
| data_020a4ba8 | src/func_02044120.c | return:int |
| data_ov045_02112fdc | include/decl_common.h | return:int [] |
| data_ov100_02148390 | include/decl_common.h | return:signed char [] |
| data_ov100_02148948 | include/decl_common.h | return:Vector3 |

The removed keys are `return:char` for data_020a4b6c and data_020a4ba8 in
src/_ZN7fBase_c21AfterCleanupResourcesEj.cpp, and `return:StarGateVector3` for
data_ov100_02148948 in src/game/actors/d_a_star_gate.cpp. Those sites now supply
the data definition instead of voting as extern declarations. The new reference
types are char, GuraResourceDescriptor, daStarGateInfo[] and StarGateVector3.
These are existing source disagreements; the gate does not establish which
spelling accurately reconstructs the ROM.

The proposed unique baseline-key count is 23,049 to 23,053. Raw findings change
from 23,149 to 23,152; repeated declarations can share a baseline key. The full
69-definition inventory and exact added/removed rows are retained in
`build/declgate-baseline-delta.json` in the producer checkout and in the
coordinator's ignored review bundle. The two previously unsupported declarators
remain explicitly counted.

## Validation and next action

- `python -m unittest tools.test_check_decl_agreement -v`: 62 tests pass.
- Original five full-size Git controls: each starts clean; every bad mutation now
  exits 1 in both full and changed modes. Each fixture keeps the real scan floors,
  with at least 2,000 files and 6,000 declarations.
- `python tools/check_decl_agreement.py --check`: expected exit 1 before baseline
  authorization, naming exactly the seven historical additions above.
- `python tools/check_python_names.py tools/check_decl_agreement.py tools/test_check_decl_agreement.py`:
  pass, no fatal names, syntax errors or advisories.
- Wired toolchain canary: `2004/b56`. No source/header/ROM build is needed for this
  tooling-only change.

Await authorization for the exact baseline patch, apply only its audited delta,
and rerun the full and changed gates before offering the final immutable candidate
for independent verification. The producer must not verify its own correction.
No normal source push, PR edit, merge, queue activation or repository setting
change is part of this checkpoint.

## Provenance rework after independent review

This stage continues input `591ecfe1a056b8782fbc322bc6b8c66e6373f91c` under
producer `codex-declgate-provenance-producer-20260908`, preserving the original
input and source/workflow identities above. The independent review reproduced
all four fixes, 62 tests, and the five original full-size controls. It returned
the checkpoint for inaccurate source-provenance prose and the still-pending
baseline decision.

The tool docstring, failure explanation and workflow comments now describe
consistency among reconstructed interfaces. Declarations and definitions can both
be wrong; ROM instructions, call sites and RTTI provide evidence. Byte validation
uses local declarations but does not compare contracts across translation units.
Calling-convention or code-generation damage is a possible consequence of a
mismatch, not an inevitable stack failure. The diagnostic also says "selected
reference" because comparisons can use a plurality declaration when no definition
exists. No parser, comparison, scope, baseline, test or executable workflow rule
changes in this stage.

The baseline remains unchanged and the exact seven-addition/three-removal proposal
above is still pending direct user authorization. This checkpoint is blocked, not
a gate pass. Its new ignored evidence records the prose-only checks and preserves
the prior independent semantic/control evidence. Publish it only as an immutable
checkpoint for independent review, without a normal source push or PR change.

Fresh checks for this provenance revision:

- AST comparison against the accepted input: only the module docstring and six
  diagnostic string constants differ. Parser, comparison, filtering and exit
  logic are identical. Workflow content excluding comments is identical.
- 62 existing unit tests pass. Python-name and dead-reference checks pass.
- Full and changed scans both exit 1 and report exactly the same seven unbanked
  historical keys. The baseline, tests, AGENTS.md, src/ and include/ are unchanged.
- The five full-size controls were not repeated: their independently tested
  implementation is unchanged, and the earlier exact-SHA evidence is preserved.

Commands, exits, log hashes and the structural comparison are recorded in the
ignored `build/declgate-provenance-evidence.json`. No fresh ROM build is claimed
or needed for diagnostic/comment changes.
