# Handoff: minigame-projection-readable-0922

This document describes this commit; the queue records the immutable output SHA.

## Scope and identity

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/2921.
- Producer/session: `codex-minigame-projection-readable-0922`.
- Branch: `cleanup/minigame-projection-readable-0922`.
- Accepted input and tested base: `9889d2374ac7d3974da30f6d45f751bfc737fd47`.
- Source checkpoint: `197e89ee99b47ebfbc27018101e333e947b5db69`.
- Active source-review policy: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`;
  task/tool revision is the accepted input above.
- Owned production files: `src/func_ov006_020bfec0.cpp` and
  `src/func_ov006_020bfff8.cpp`; ov006 `[0x020bfec0,0x020c0134)`, two functions,
  628 bytes. Both were already enrolled. This task adds no shadow source,
  promotion, shared header, baseline, layout, symbol or attribution change.

## Changes and evidence

- **PROJECTION-READ-01 fixed:** replace register-style parameters and intermediates
  with camera, world/view position, screen coordinates, tangent, half-height,
  half-width and normalized/scaled coordinate names. The existing matrix multiply,
  sine/cosine ratio, negative view depth and 4:3 scale support those names. The
  caller shapes retain short output pairs in one helper and separate int outputs
  in the other. Every expression, constant, signed shift and cast is unchanged;
  y is still written before x.
- **PROJECTION-TRIG-01 fixed:** the old cosv/sinv names were reversed. The shared
  table at 0x02082214 stores sine then cosine: its first pair is (0,4096), followed
  by (6,4096). `src/func_0203cd80.c` uses the same pair order in its rotation
  formula, and `src/Camera_UpdateMatrices.c` supplies it to PerspectiveW.
  Only the names changed; the existing sine/cosine division remains intact.
- **PROJECTION-NATIVE-01 fixed:** call native `cstd::fdiv(int,int)` using a local
  namespace declaration. `src/_ZN4cstd4fdivEii.cpp` defines that scalar interface
  and returns s32/int. The compiler now supplies the mangled symbol; the linked
  call target and complete object are unchanged.

The completed first-pass read was originally pinned to aggregate `5f50a061`.
Both entire source bodies and the relevant matrix/division/table definitions were
read again at this task's exact input before editing. Call sites in the existing
Jump3DMario TU and the separate-int projection callers confirm output roles.
No caller or adjacent owner was edited.

## Remaining reconstruction and experiments

Completion remains **partial** under issue #2921, owned by
`codex-minigame-projection-readable-0922`.

- **PROJECTION-RECON-01 deferred:** preserve the opaque camera pointer and the signed
  halfword access at +0xb8, raw three-int view-position storage and existing erased
  MulVec3Mat4x3 declaration. The nearby Camera_UpdateMatrices source describes the
  local minigame camera, but this is not proof that the engine Camera header is
  interchangeable. A shared camera/vector contract and native entry names need
  separate ownership and byte proof; no layout or original naming is asserted here.
- This was one combined source trial, and both complete objects matched on the
  first compile. No failed source experiment or compiler limitation is claimed.
- An initial private table diagnostic mistakenly assumed ARM9 load base 0x02000000.
  That result was rejected before source editing. The corrected read uses the
  repository's `match.target_bytes` and pinned base 0x02004000; the rejected probe
  and corrected evidence are preserved separately. It is not a byte-gate result.

## Producer proof

Private logs are under ignored `build/projection-readable/`. Claim/coordinator
receipts there are credentials and must not be published.

- Pinned compiler `2004/b56`, flags from `build_pin.flags_for`.
- Entire baseline/candidate ELF objects are identical, 1,032 bytes each:
  - func_ov006_020bfec0: SHA256
    `da5d4991623e4f725afd6d9aa7959a34ae47105cbd5aa965bf0b33374dcee2e5`.
  - func_ov006_020bfff8: SHA256
    `93552073d1806a7a5be477bbf1d1944bada5f3ceafe51effdcd7d1202b8283b5`.
  `baseline.json` and `object-comparison.json` retain all defined symbol bindings.
  Each object has one positive-size function plus its existing ARM mapping symbols;
  no extra function or data object was introduced. Whole-object equality includes
  allocated sections, literal pools, symbols and relocations.
- Exact source-checkpoint range relocation gate: 2/2 VERIFIED, `blind: 0`,
  `diffs: []`; `strict.json` and `strict.log`.
- Declaration check against the pinned base: no new disagreement; 386 inherited
  scoped disagreements remain in fifteen reached files. `declarations.log`.
- Attribution: 8,012 tracked, zero changed/lost credits. `attribution.json`.
- Port references, source layout, duplicate-source checks, diff check and
  CONVERTED ratchet pass. Banked 2,995/current 3,007; gains are not re-banked.
- Language ratchet passes against chaos-data revision
  `6e1c95ed7eee315ca3b71ee8bdb34a0bd53d9cf1`; `langmode-check.json`.
- Metadata checker emits zero standalone data records for these two sources,
  with zero differing or unnamed records. This is no data-ownership claim;
  `romdata.json` and `data.log`.
- Full packaged ROM build: exit 0, **11,209/11,209** source-built functions and
  **106/106** modules exact; **26/26** initialized-data claims and three BSS claims
  (172 bytes) pass. Intact-TU gates pass with zero new stock symbol errors and
  exact storage aliases. `rombuild.json`, `rombuild.log`, `rombuild-command.json`.
  The broader metadata census remains 775 verified, 272 partial, three differing
  and 272 unnamed; that inherited corpus debt is not attributed to these two
  functions or claimed resolved. An actual 16MiB stock ROM was packaged.
- Final committed-range proof is attached to the producer evidence after this
  handoff is committed, so no commit is amended merely to embed its own SHA.

Independent exact-commit source review, current-main composition, PR checks and
integration remain separate. No private validator, public CI or merge acceptance
is claimed by this producer handoff.
