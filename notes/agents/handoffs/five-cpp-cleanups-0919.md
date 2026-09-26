# Handoff: five-cpp-cleanups-0919

This commit removes fake model interfaces in four render methods and simplifies
one ground query. The queue records the immutable producer output SHA; independent
verification and GitHub checks are separate acceptance steps.

## Identity and ownership

- Task: `five-cpp-cleanups-0919`; producer: `codex-five-cpp-0919` (Codex).
- Branch: `cleanup/five-cpp-0919`.
- Source/input base: `4407c8471ed3299ea16d9cc899020e3232ac6ee9`.
- Installed tools: the same base; active source-review policy pin:
  `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Seven reserved outputs: the five source files below, this handoff, and
  `notes/experiments/five-cpp-cleanups-0919.json`.
- Next owner: the independently claimed verifier. The user's other integration
  session owns merging; this task does not authorize an integration train.
- Status at this commit: locally verified candidate; formal independent source
  acceptance and terminal GitHub validation pending. This is a bounded cleanup,
  not complete reconstruction of the five classes or their dependencies.
- Local scratch, objects and complete build logs remain under the producer
  worktree's `build/five-probes` and `build/five-*.json|log`; they are not PR files.

## Changed scope

| Source symbol | Module | Start | Bytes |
| --- | --- | --- | --- |
| `_ZN11PowerFlower6RenderEv` | [ov002](../../../config/arm9/overlays/ov002/symbols.txt) | `0x020b9aac` | 196 |
| `_ZN11RollingRock6RenderEv` | [ov021](../../../config/arm9/overlays/ov021/symbols.txt) | `0x0211281c` | 56 |
| `_ZN7daBtn_c6RenderEv` | [ov079](../../../config/arm9/overlays/ov079/symbols.txt) | `0x02125f78` | 84 |
| `_ZN12daBDonketu_c6RenderEv` | [ov064](../../../config/arm9/overlays/ov064/symbols.txt) | `0x0211764c` | 56 |
| `_ZN10dBgW_KcMbg10DetectClsnER9dBgCh_Gnd` | [arm9](../../../config/arm9/symbols.txt) | `0x02039cb8` | 256 |

Each symbol remains in its existing `src/<symbol>.cpp` path and enrollment.
The four render methods call their existing `Model` or `ModelAnim` member's real
`void Render(const Vector3*)` slot instead of declaring a dummy `Sub` interface
with virtual `g0` through `g5`. Existing receivers, conditions and arguments stay
the same. No new class identity, original spelling or TU ownership is asserted.

The ground query reads `probePos.y` directly and removes the unsupported claim
that its old volatile cast was original or required. The scratch line query's
`lineEnd` at +0x54 and real `dBgPi` base at +0x10 replace separately declared
interior globals. Their layout agrees with the existing shared declaration and
the symbol addresses. The probe-depth comment now describes the named field.

## Review findings and retained forms

- `FIVE-CPP-0919-01`: fixed dummy model-slot interfaces in all four methods.
- `FIVE-CPP-0919-02`: fixed artificial volatile read and unsupported provenance.
- `FIVE-CPP-0919-03`: retain the short integer flag temporary. Replacing only it
  with `if (mFlags & 0x40000) return 1;` gives 184 instead of 196 bytes and
  `NO-REPRO` under 2004/b56.
- `FIVE-CPP-0919-04`: retain the scale-vector view over inherited scalar fields.
  A local `Vector3` assigned x/y/z and passed by address gives 212 instead of
  196 bytes and `WRONG`. This measures that exact alternative; it neither proves
  all cleaner forms impossible nor changes the shared actor header.
- `FIVE-CPP-0919-05`: fixed both scratch interior aliases with real subobjects.
  The existing `func_020374b8(int*,int*)` accessor boundary still matches its
  actual definition, which copies the query position at +0x38/+0x3c/+0x40.
  Its casts are not newly invented interfaces or claimed compiler constraints.
- `FIVE-CPP-DEPENDENCY-01`: neighboring unchanged `func_02039e48.c` and
  `func_02039e30.c` erase their output pointer to int and redeclare the actual
  `MulVec3Mat4x3` output parameter incorrectly. This caller's `Vector3*`
  declarations remain correct. That separate definition debt is not repaired
  or accepted as reconstructed by this scoped review.

The experiment JSON records exact expressions, compiler hash/flags, object
signatures and relocation-aware verdicts. It carries producer measurements,
not independent acceptance. Lifecycle, vtables/RTTI, initializers, data ownership,
shared headers, attribution and enrollment are unchanged.

## Local proof

- Baseline and final `python -u tools/rombuild.py -j16 --report-json <report>`:
  exit 0, 106/106 exact modules, 11,208 reproducing source-built functions,
  zero source mismatches, all 26 source data claims exact. Both packaged ROMs
  have SHA256 `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`.
  Commands/results are retained in the experiment JSON; full local reports are
  `build/five-base-rom.json` and `build/five-final-rom.json`.
- Explicit module-qualified `tools/linkcheck.py` checks: all five final functions,
  648 bytes total, `VERIFIED`, `diffs: []`, `blind: 0`. Per-symbol reproducible
  commands and final candidate source hashes are in the experiment JSON.
- Complete allocated sections and global/weak definitions remain equal. The four
  render objects also retain identical relocation signatures. The ground object
  intentionally references `data_020a0d0c` plus the real subobject offsets instead
  of two interior symbols; resolved bytes/destinations remain exact.
- `python tools/port_refcheck.py`: exit 0, 423 references resolve.
- `python tools/check_duplicate_sources.py`: exit 0, 8,326 stems, no duplicates.
- `python tools/check_src_tu_compiles.py --quiet`: exit 0, 224/224 TUs compile.
- `python tools/check_decl_agreement.py --changed <base>`: exit 0, no new
  disagreements in the expanded 27-file scope; 383 existing disagreements are
  banked. This is not a claim that the whole dependency closure agrees.
- `git diff --check`: exit 0. Commit-based attribution and exact-range link
  checks run after this immutable commit and are recorded by queue evidence.

The baseline and final whole-ROM analyses/ROM-data reports are identical. The
broader ROM-data inventory still reports 753 verified, 260 partial, 3 differing
and 308 unnamed symbols; those existing inventory limits are not new failures
or changes to the 26 source-owned data claims. Nine existing symbol-audit errors
also remain identical to the baseline, with zero new symbol errors. No eligibility
reference report was generated, and no such reference-check pass is claimed.

Private `validate` and independent `Source review` must judge the published exact
head/base. Main movement requires renewed integration/composition acceptance.
