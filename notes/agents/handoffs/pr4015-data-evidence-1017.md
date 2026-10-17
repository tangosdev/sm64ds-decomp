# PR #4015: global-data evidence and pinned-build handoff

State recorded on 2026-10-17. Continue on PR #4015's branch, in the existing
`build/data-evidence` worktree. Implementation commit `fc7719d85a` is pushed to
[PR #4015](https://github.com/tangosdev/sm64ds-decomp/pull/4015); all 11 PR checks
passed on that commit. This handoff is a subsequent documentation-only commit.

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

## Verified control; production still blocked

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

## Next concrete step

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

This continuation has not been implemented or run. It would establish independent
same-worker packaging equivalence, not equality with the historical cartridge's
outer-file hash. Module-qualified pointer destination semantics, BSS startup and
constructors, and reviewed names/types/owners remain separate future work.

## Local artifacts for resumption

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
