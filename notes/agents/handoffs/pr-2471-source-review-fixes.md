# PR #2471 declaration-agreement corrections

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
