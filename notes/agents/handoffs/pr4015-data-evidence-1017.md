# PR #4015: global-data evidence and pinned-build handoff

Updated on 2026-10-17 after resumption: the full production build and both requested
data-provider proofs now pass on clean `f97fdef9e902b010423c5a8995273875c49f02df`.
See the continuation below for the independent retail reference, exact receipts,
and remaining limits. This update changes documentation only.

Continue in `C:/tmp/sm64ds-pr4015-resume1017`, local branch
`resume/pr4015-1017`, publishing back to the existing PR #4015 branch.
The earlier `build/data-evidence` worktree was absent on this machine; its private
artifacts were not available. The original handoff evidence is retained below as
history. All 11 PR checks passed on `f97fdef9e9` before this documentation update.

This is the tooling companion to #3937. It does not change game source, headers,
symbol inventories, or admission manifests, and it does not verify #3937's current
head. The [design note](../../llm-assisted-global-data-migration.md) distinguishes
semantic identity, historical ownership, configured placement, and compiled proof.

## Implemented and reviewed

- Static evidence packets preserve module-qualified symbols, boundaries,
  relocations, references, ownership claims, input fingerprints, and uncertainty.
- Raw data comparisons include trailing bytes after the last four-byte word.
- `rombuild.py --data-provenance-requests <JSON>` follows actual prepared compiler
  objects through linker selection and final ELF placement. Initialized objects
  require every requested byte to match; BSS requires source-provider and storage
  layout proof. A source-named retail gap object cannot satisfy either request.
- Both `rombuild.py` and `tubuild.py linkcheck` now enforce the documented `dsd`
  0.11.0 version before preparing inputs or cleaning scratch output. Reports record
  the executable path, version, and SHA-256. Baseline fingerprints include the new
  helper. Tests cover wrong versions and refusal before destructive preparation.

Local regression results: 450 passed, one Windows symlink test skipped. The runs
comprised 155 evidence/relocation/enrollment/linkcheck tests, 41 TU tests, 242
build/merge tests including that skip, and 13 pin tests. Python-name, dead-reference,
and whitespace checks passed. Independent review found no substantive issues with
the preflight or its entry-point tests.

## Local tool mismatch resolved

The shared tool was `ds-decomp-cli 0.12.0`, while repository setup specifies 0.11.0.
Its generated exception-table linker directive left the correct relocation in
`func_02071844` resolving to zero instead of `data_020737a8`. The resulting four-byte
ARM9 mismatch was a local tool mismatch; no game-source repair was needed.

The worktree now has a private `tools/bin/dsd.exe` from the official 0.11.0 release:
SHA-256 `a9dd4dc6273e41614efb8119eb9c075d60ed6106106edd3fd8a8fd2c7606c3ad`.
The shared checkout's executable was preserved. Its extracted configuration also
used the newer schema, so the existing local ROM was freshly extracted with 0.11.0
into a private worktree `extracted/dsd/`. All 871 binary files matched the previous
extraction. Among 1,322 non-binary files, only `config.yaml` differed: 0.11 uses
uniform section padding while 0.12 records per-section padding. Shared inputs were
preserved; do not replace files through their junctions.

## Original handoff: verified control; production blocked

The ordinary baseline command completed successfully:

```sh
python tools/tubuild.py linkcheck --baseline --module arm9 -j16 --clean
```

Independent inspection established:

- All 264 admitted intact TUs were demoted to retail gaps, with exact source-path
  coverage of the current admitted inventory.
- All 106 modules and all 5,646 compiled control functions match retail bytes.
- The nine symbol-error strings exactly match every admitted manifest's inventory.
- Executable-module fingerprint:
  `f9852ffaf80ef19662ab5535df4b13b935188ac3087b5b1d4561c089e4dd802b`.
  This matches all 264 admission records.
- Control ROM SHA-256:
  `ddab9300b24aeae254192c2a30d812b07ece3adff83230e75c865ed915c9de40`.

The full production run on clean `fc7719d85a` stopped at its existing intact-TU
bootstrap gate. All admissions record a different complete-ROM SHA-256:
`d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`.
There is no independent same-worker `build/sm64ds.nds` comparison. Consequently,
the run did not reach the requested data-provider proof. Do not describe the
106-module control pass as a complete production-build pass, or infer the cause
of the older packaging hash solely from the local extraction comparison.

The earlier focused pilot and negative control on `270482abe8` remain historical
checker evidence: a compiled 12-byte initializer and an 8-byte BSS object passed;
substituting a source-named retail object preserved module bytes but correctly
failed both provider requests. Those experiments are not current-head production
proof.

## Experiment proposed at the original handoff

Investigate an independently generated all-retail stock packaging reference.
Independent review judged this a legitimate continuation under the existing
same-worker comparison contract, subject to all of these conditions:

1. Use separate scratch configuration and freshly generated gap objects, linker
   script, and outputs. Demote every complete source; compile zero sources. Do not
   consume the candidate control's linked ELF, module outputs, or packaged ROM.
2. Verify all 106 resulting modules against extracted retail and the admitted
   executable-module fingerprint. Preserve a separate receipt binding the inputs,
   tool hashes, actual providers, outputs, and resulting ROM hash.
3. Only an independently produced and verified reference may supply the stock
   comparison. Rerun the ordinary strict baseline so it computes `matchesStockRom`
   itself. Never copy its own output into the expected-reference path, edit report
   flags, re-bank diagnostics, or weaken the existing gates.
4. Then rerun the normal production build with `build/data-requests.json`, retaining
   the exact-head receipt. Freeze all source/configuration/Python tool files during
   that run: the provenance checker detects changed inputs, including test files.

At the original handoff this continuation had not been run. It would establish independent
same-worker packaging equivalence, not equality with the historical cartridge's
outer-file hash. Module-qualified pointer destination semantics, BSS startup and
constructors, and reviewed names/types/owners remain separate future work.

## Historical local artifacts from the original worktree

All paths below are relative to the worktree and remain ignored:

- `build/tu/_baseline/link/linkcheck.json` and `build/pinned-baseline.log`:
  successful pinned control, its fingerprints, nine diagnostics, and demotions.
- `build/data-provenance-full-build.json` and corresponding `.log`:
  production refusal on `fc7719d85a` at the ROM-hash bootstrap gate.
- `build/data-requests.json`: ov047 `data_ov047_02112408` (12 initialized bytes)
  and `data_ov047_02112638` (8-byte BSS), both owned by
  `src/game/actors/d_a_obj_km3_kuruma.cpp`.
- `build/data-provenance-pilot.json` and `build/data-provenance-gap-control.json`:
  historical positive and negative controls on `270482abe8`.
- `build/extraction-compare.json`, `build/dsd-v0.11-extract.log`, and
  `build/build-regression-tests.log`: extraction and test observations.

Keep ROMs, executable tools, extracted assets, generated linker artifacts, and
machine-specific configuration local. No class/TU queue claim was needed or taken
for this tooling work. Do not alter unrelated worktrees or their active tasks.


## Resumption: independent reference and full production proof

On clean `f97fdef9e902b010423c5a8995273875c49f02df`, the new isolated worktree
used the installed dsd 0.11.0 executable with SHA-256
`a9dd4dc6273e41614efb8119eb9c075d60ed6106106edd3fd8a8fd2c7606c3ad`.
The original dirty checkout and shared inputs were preserved. Source, configuration,
and Python tools stayed unchanged throughout the production run.

The independently generated reference used a fresh stock-profile scratch copy,
demoted all 2,467 complete sources, compiled zero sources, and linked 2,713 newly
generated retail-gap objects. Every actual object-list provider was checked to be
under the independent scratch delinks directory. No candidate-control ELF, module,
or ROM output was used as an input.

The first attempt stopped at mwldarm's overlapping symbol-size check in the ITCM
soft-float object. Its 0xb6c-byte parent symbol contains four separately sized
interior function entries. A second, fresh scratch tree changed only that parent's
size metadata from 0xb6c to 0x11c, ending at the first interior entry at 0x01ff98f4.
It retained all symbol addresses, section ranges, relocations, and retail bytes.
This adjustment exists only in the independent reference's scratch configuration;
it is not a production symbol change or a historical function-boundary claim.
The first failed attempt is preserved separately.

The second reference passed all 106 module comparisons and `dsd check modules -f`.
Its executable-module fingerprint is
`f9852ffaf80ef19662ab5535df4b13b935188ac3087b5b1d4561c089e4dd802b`, matching
all 264 admissions. Its complete ROM SHA-256 is
`d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`, also
matching the historical admission hash. This current observation does not identify
the cause of the previous machine's different packaging hash.

The reference receipt binds original inputs (including the extracted ROM tree),
tool hashes, scratch configuration, actual object providers, linker inputs, and
outputs. These were rehashed before installing that reference at the previously
absent stock comparison path. The normal baseline then ran unchanged:

```sh
python tools/tubuild.py linkcheck --baseline --module arm9 -j16 --clean
```

It demoted exactly 264 intact TUs, passed all 106 module comparisons, reproduced
the admitted nine-symbol diagnostic inventory, and computed `matchesStockRom: true`
itself. No admission record, report flag, diagnostic baseline, or gate was edited.

The normal full production build then ran, with a separate output ROM:

```sh
python tools/rombuild.py -j16 --data-provenance-requests build/data-requests.json --rom-out build/production-data-proof.nds --report-json build/resume-production.json
```

Result: exit 0, report status `passed`, all 106 modules exact, all 11,334
source-built functions reproducing, and all 338 source-owned data claims reproducing.
The intact-TU checks passed with zero new symbol errors and exact storage aliases.
The production ROM equals both the independent reference and the historical
admission hash. Both actual compiler-provider requests passed: the complete 12-byte
initializer `data_ov047_02112408` and the 8-byte NOBITS object `data_ov047_02112638`.
The receipt records the clean tested Git head and bound inputs before compilation.

The broader raw-object data inventory remains 974 verified, 1,559 partial, one
differing, and 15 unnamed records. That inventory is distinct from the two requested
provider proofs and the full-module pass. Neither this experiment nor its scratch
metadata adjustment proves historical source ownership, recovered names/types,
BSS runtime initialization, or same-address wrong-overlay pointer semantics.
No verification of PR #3937's current head or independent reviewer acceptance is
claimed. No merge was performed.

Fresh targeted regression run: 174 tests passed across evidence, provenance,
provenance orchestration, raw-data comparison, dsd pin, rombuild, and TU production.

Current local artifacts, relative to `C:/tmp/sm64ds-pr4015-resume1017`:

- `build/independent-retail-reference-v2.py` and its `.log`: repeatable experiment;
  script SHA-256 `188a2060f81baae13a0bbf061e65bb2ea52dcc9dd1b24f6c5e2b01cbcac8d34b`.
- `build/retail-reference-v2/receipt.json`: full independent reference receipt;
  SHA-256 `88895604bc0179767ef8dbef2f49f5e45c9ff040974c421315926db6bbd32502`.
- `build/retail-reference/`: preserved first attempt and exact linker error.
- `build/retail-reference-install.json`: installation identity, before baseline.
- `build/tu/_baseline/link/linkcheck.json` and `build/resume-baseline.log`:
  fresh strict control and its automatically computed same-worker comparison.
- `build/resume-production.json` and `.log`: complete production receipt and log;
  receipt SHA-256 `7858316e4258613b505aa44dd495149cf4fda9dabf856eb0dd19a8758959dfaa`.
- `build/data-requests.json` and `build/resume-regression-tests.log`: exact requests
  and targeted test results.

Next action: review this evidence and the tooling PR. Check CI for the new
documentation commit before any integration decision. Private ROM/tool/build
artifacts remain local; do not publish them.
