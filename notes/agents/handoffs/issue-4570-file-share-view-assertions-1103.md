# Handoff: issue-4570-file-share-view-assertions-1103b

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue/task/stage/session: https://github.com/tangosdev/sm64ds-decomp/issues/4570; `issue-4570-file-share-view-assertions-1103b`; producer; `codex-file-share-view-1103c`.
- Prior accepted candidate: `c8199d1587d5855e7612e7a1127be8a8ad4578e3`; task base: `76e2aed46eb35ebd944d1bf14e668e73fc3bad0a`.
- Workflow pin: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- This is a formatting and handoff correction to the prior candidate; no source tokens or compiler output changed.
- Next action: independent source review and exact-byte verification of this output commit and base.
- Status: verified candidate; independent review pending.

## What changed and why

- Scope: `arm9:0x02040c34..0x02040f30`, `func_02040c34`.
- The prior candidate replaced the local `ParentNodes` and `ChildTables` shadows with shared `FileShareParentArea` and `FileShareChildArea` views and typed `FileShareParentArea::mNodes` as `FileShareNode[3]`.
- This continuation applies the pinned formatter to the changed source/header lines and corrects the handoff wording that the dead-reference check parsed as a nonexistent path.
- The child block-info table stores 32-bit DS addresses, matching the shared `int` slots; the source makes the pointer-to-32-bit conversion explicit.
- The node size is asserted as `0xcc0`, the member offset as `0xc0`, and the enclosing area as `0x2730`.

## Reconstruction dimensions

- Exact coverage: `func_02040c34`, 0x2fc bytes; full source-built module is exact.
- Relocations: all 18 consumers of the changed shared header, 22 functions total, verified with no warnings or blockers.
- Recovered layout: this work replaces two duplicate views and gives the known parent-node array its shared element type. Other file-share fields and original TU boundaries remain outside scope.
- Attribution, manifests, and symbol rows: unchanged.
- Remaining issue scope: continue auditing confirmed local shadows and field names against uses; byte identity does not establish a unique source spelling.

## Proof

- `func_02040c34` at 0x02040c34, size 0x2fc: exact `2004/b56` match with strict relocation checking after formatting.
- `python tools/affected_src.py include/private/arm9_a1fc0_file_share.h --json`: 18 consumers.
- `python tools/prepush_linkcheck.py --files <all 18 affected files>`: exit 0; 22 verified, 0 warnings, 0 blockers.
- `python tools/rombuild.py -j16 --no-rom`: exit 0; 106/106 modules exact, 11,395/11,395 functions exact, zero mismatches; source-owned data 468/468 reproduces.
- `python tools/check_decl_agreement.py`: no new declaration disagreements.
- `python tools/check_dead_references.py`: no new dead references or broken links on the corrected handoff.
- `python tools/port_refcheck.py`: 217/217 references resolve.
- `python tools/source_style.py --base origin/main --files src/unnamed/arm9/0204/func_02040c34.c include/private/arm9_a1fc0_file_share.h --clang-format <clang-format-19.1.7>`: zero findings.
- `git diff --check`: pass.
- The hosted PR validator and independent source review have not run on this corrected candidate yet.
