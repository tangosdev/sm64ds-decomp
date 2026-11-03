# Handoff: issue-4570-actionwalk-opcode-reader-1103b

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue/task/stage/session: https://github.com/tangosdev/sm64ds-decomp/issues/4570; `issue-4570-actionwalk-opcode-reader-1103b`; producer; `codex-actionwalk-opcode-1103c`.
- Prior accepted candidate: `d9ce67a6666deeeaa886719e151638ad356a2e19`; task base: `76e2aed46eb35ebd944d1bf14e668e73fc3bad0a`.
- Workflow pin: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- This continuation applies pinned formatting to the function definition and corrects the handoff wording flagged by the dead-reference check. No compiler output changes.
- Next action: independent source review and exact-byte verification of this output commit and base.
- Status: verified candidate; independent review pending.

## What changed and why

- Scope: `arm9:0x02071d3c..0x02071f1c` (`func_02071d3c`) and `arm9:0x02072d90..0x02072dac` (`func_02072d90`).
- The prior candidate replaced the local three-word `S72d90` view in `func_02072d90` with shared `ActionWalk`; the opcode reader names the pointer it reads as `mAction` at offset 0x08.
- It also updated `func_02071d3c` declarations and calls to pass the `ActionWalk` view. Its wider `S72d90` scratch buffer remains because it holds copied exception context consumed by the following walker; only its first words are viewed as `ActionWalk`.
- No original exception-runtime translation-unit boundary or complete context type is claimed.

## Reconstruction dimensions

- Exact coverage: both changed functions match; `func_020729f4` remains unchanged but is included in consumer relocation verification.
- Relocations: all three consumers of `arm9_action_walk.h` verified, zero warnings or blockers.
- Recovered layout: the first three iterator words agree with the shared header and the opcode reader's actual offset-0x08 load. The remaining context buffer fields stay in their current partial representation.
- Attribution, manifests, and symbol rows: unchanged.
- Remaining issue scope: continue per-consumer source reconstruction; byte identity does not establish a unique source spelling.

## Proof

- `func_02072d90` at 0x02072d90, size 0x1c: exact `2004/b56` match with strict relocation checking.
- `func_02071d3c` at 0x02071d3c, size 0x1e0: exact `2004/b56` match with strict relocation checking.
- `python tools/affected_src.py include/private/arm9_action_walk.h --json`: 3 consumers.
- `python tools/prepush_linkcheck.py --files <all 3 affected files>`: exit 0; 3 verified, 0 warnings, 0 blockers.
- `python tools/rombuild.py -j16 --no-rom`: exit 0; 106/106 modules exact, 11,395/11,395 functions exact, zero mismatches; source-owned data 468/468 reproduces.
- `python tools/check_decl_agreement.py --changed origin/main`: no new declaration disagreements.
- `python tools/check_dead_references.py`: no new dead references or broken links on the corrected handoff.
- `python tools/port_refcheck.py`: 217/217 references resolve.
- `python tools/source_style.py --base origin/main --files src/unnamed/arm9/0207/func_02071d3c.cpp src/unnamed/arm9/0207/func_02072d90.c --clang-format <clang-format-19.1.7>`: zero findings.
- `git diff --check`: pass.
- The hosted PR validator and independent source review have not run on this corrected candidate yet.
