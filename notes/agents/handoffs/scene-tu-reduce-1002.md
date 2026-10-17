# Scene production TU consolidation

Base: `5fa32e37cbe517c15b01cd902cb15fb5d0ab8950`.
Candidate: this commit. Local worktree: `C:/tmp/sm64ds-tu-reduce-1002`.
Queue task: `scene-tu-reduce-1002`; producer/integration owner: `codex-tu-reduce-1002`.
No PR, remote source publication, or merge is authorized by this handoff.

## Production result

`src/game/stages/d_scene.cpp` replaces 21 per-function sources in the default stock
build. The complete arm9 text entry owns exactly `0x0202e140..0x0202ec9c`
(2,908 bytes, 21 functions). The shadow source is retired. Tracked C/C++ files
under `src` fall from 5,148 to 5,128; no source coverage is relinquished.
All 21 contributor identities have explicit `path#symbol` attribution. The
18 previously converted identities migrate to the same members.

The source contains 19 marked handwritten definitions and two compiler-emitted
inline destructor variants. Current native fader calls are preserved. A boolean
`ResetFadersAndSound` result expresses its two 0/1 exits and allows both dScStage_c
and Entry callers to return the result through exact native tail calls. The
unused conflicting generated alias is removed. Bank-reset return declarations,
affine-register pointer type and sound storage view agree with inspected owners.

## Boundary and reconstruction limits

The interval, order and exact compiled output are measured. The original file
boundary is an inference with medium confidence: interleaved Scene/graphics
functions and nearby referenced data support co-residence, but adjacency alone
does not prove static linkage or original object ownership. GraphCallbacks are
outside the measured interval and are not claimed. No data ranges are promoted.

The local two-argument Fader dispatch view remains because the shared family
interface does not yet express the second argument present at these call sites.
Mangled external graphics interfaces and the scene-spawn veneer return/argument
contract remain partial reconstruction. These are preserved limits, not claims
that packaging completes every interface.

## Validation

- `python tools/tubuild.py verify arm9/Scene`: 21/21 MATCH, clean isolation and
  relocation destinations, ROM-ascending emission, all emitted output accounted for.
- Production `python tools/rombuild.py -j16`: 106/106 modules exact; 11,255
  source-built functions reproduce, zero mismatches; no new linked-symbol errors.
  ROM SHA-256: `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`.
- 147 affected sources were enumerated across the changed headers; the production
  build recompiles affected consumers. Both direct caller linkchecks independently
  report VERIFIED, zero warnings and zero blocking failures.
- `port_refcheck.py`: 408 references resolve. Converted ratchet passes without
  backslide exceptions. Commit-range relocation and attribution checks follow
  the immutable checkpoint and are recorded in the worktree build logs.
- The stock control retains nine existing dsd symbol diagnostics; the intact-TU
  check reports zero new errors. Global advisory data totals still include three
  differing records elsewhere; no claim is made that all tree data is reconstructed.

## Metadata coverage

The seven exact `deadstrip-data` policies name configured ROM homes outside the
claimed text. Standard romdata_check reports four VERIFIED records and three
PARTIAL type-name records; it omits the vtable preamble and incomplete final words.
Independent review and producer reproduction compare every emitted byte instead:
all 140 bytes equal, including the 8-byte preamble and string tails; zero blind
relocations. The object is unchanged by the declaration repairs.

```json
{
  "sourceSha256": "18d3f7286b1672e733a87c71e49e117aed5bd35ff92c1082c15b40cd4962d8d2",
  "objectSha256": "0dadeaa0149790c8967029f6f95447fbeb6753c061f376016336e454fa5d2839",
  "totalBytes": 140,
  "records": [
    {
      "symbol": "_ZTI7fBase_c",
      "address": "0x2086d70",
      "bytes": 8,
      "blind": [],
      "equal": true
    },
    {
      "symbol": "_ZTS7fBase_c",
      "address": "0x2086e60",
      "bytes": 9,
      "blind": [],
      "equal": true
    },
    {
      "symbol": "_ZTS7dBase_c",
      "address": "0x2086e54",
      "bytes": 9,
      "blind": [],
      "equal": true
    },
    {
      "symbol": "_ZTS8dScene_c",
      "address": "0x20914b0",
      "bytes": 10,
      "blind": [],
      "equal": true
    },
    {
      "symbol": "_ZTI7dBase_c",
      "address": "0x2086e78",
      "bytes": 12,
      "blind": [],
      "equal": true
    },
    {
      "symbol": "_ZTI8dScene_c",
      "address": "0x20914d4",
      "bytes": 12,
      "blind": [],
      "equal": true
    },
    {
      "symbol": "_ZTV8dScene_c",
      "address": "0x2092678",
      "bytes": 80,
      "blind": [],
      "equal": true
    }
  ]
}
```

Reproduction uses the ELF symbol's complete section slice, its typed relocations
from `linkcheck.func_relocs_typed`, the configured canonical address (minus eight
for vtable storage), and `linkcheck.link_function`. Normalize `_ZTV` relocation
addends by the existing eight-byte address-point convention, then compare the
entire resulting byte string with `romdata_check.RV.rom_bytes`. Assert the emitted
symbol set equals the seven policy symbols, every comparison equals, and every
blind-relocation set is empty. The exact runnable script and report are retained
in the producer/reviewer conversation and ignored build evidence.

## Independent source review

Reviewer `scene_review` inspected all 21 bodies and contributor identities.
SCENE-SR-01 qualifies boundary claims; SCENE-SR-02 repairs the retired-source
citations; SCENE-SR-03 corrects standard metadata coverage claims and adds the
complete comparison; SCENE-SR-04 repairs dScStage_c's second missing-return workaround.
All four producer repairs are present. Final exact-commit re-review is pending.

## Pending declaration bookkeeping

The user authorized isolated edits to attribution, converted-baseline and arm9
delinks despite their existing reservations; other claims/worktrees are untouched.
The fourth file, `config/decl-agreement-baseline.json`, remains unchanged pending
the user's separate scope approval. The tracked declaration gate therefore reports
six existing disagreements under the new TU filename. The prepared proposal moves
exactly those six already-banked identities and removes 21 repaired/retired old
entries; it adds no disagreement exceptions. Running the actual checker against
that proposed baseline passes the complete changed scope (182 files).

Prepared artifacts: `build/scene-declaration-baseline-proposal.json` and
`build/scene-declaration-migration.json`. The clean base's whole-tree declaration
gate independently fails on an unrelated data_ov002_02111154 disagreement;
that baseline failure is not absorbed into this task.

Next action: after the pending scope decision, apply only the reviewed baseline
path migration, rerun the tracked declaration gate, checkpoint and obtain final
independent acceptance. Keep this source candidate local until publication is
explicitly requested. The broader file-reduction goal remains active.
