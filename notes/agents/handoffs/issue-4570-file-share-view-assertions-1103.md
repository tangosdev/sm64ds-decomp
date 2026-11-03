# Handoff: issue-4570-file-share-view-assertions-1103

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue/task/stage/session: https://github.com/tangosdev/sm64ds-decomp/issues/4570; `issue-4570-file-share-view-assertions-1103`; producer; `codex-file-share-view-1103b`.
- Source branch and input: `audit/share-view-assertions-1103`, based on `76e2aed46eb35ebd944d1bf14e668e73fc3bad0a`.
- Workflow pin: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Next action: independent source review and exact-byte verification of this output commit and base.
- Status: verified candidate; independent review pending.

## What changed and why

- Scope: `arm9:0x02040c34..0x02040f30`, `func_02040c34`.
- Replaced `ParentNodes` and `ChildTables` with the shared `FileShareParentArea` and `FileShareChildArea` views; deleted their single-purpose header.
- The child block-info table stores 32-bit DS addresses, matching the shared `int` slots; the source now makes the pointer-to-32-bit conversion explicit.
- Typed `FileShareParentArea::mNodes` as `FileShareNode[3]`. The node size is asserted as `0xcc0`, the member offset is asserted as `0xc0`, and the enclosing area remains asserted as `0x2730`.
- The pool loop uses exactly the three records at `mNodes`; each is manipulated through its shared `FileShareNode` fields. This is source-layout evidence from the existing definitions and callers; it does not claim recovered EAD names for every file-scope detail.

## Reconstruction dimensions

- Exact coverage: `func_02040c34`, 0x2fc bytes; full source-built module remains 106/106 exact.
- Relocations: focused link check covered 22 functions across all 18 consumers of the changed shared header; all verified.
- Recovered layout: this change replaces two duplicate views and gives the known parent-node array its shared element type. Other file-share fields and original TU boundaries remain outside scope.
- Attribution, manifests, and symbol rows: unchanged.
- Remaining issue scope: continue auditing confirmed local shadows and field names against uses; do not infer source identity from a byte match.

## Proof

- `python tools/match.py --c src/unnamed/arm9/0204/func_02040c34.c --func func_02040c34 --addr 0x02040c34 --size 0x2fc --version 2004/b56 --strict-relocs --module arm9 --brief`: pass, exact match.
- `python tools/affected_src.py include/private/arm9_a1fc0_file_share.h --json`: 18 consumers.
- `python tools/prepush_linkcheck.py --files <all 18 affected files>`: exit 0; 22 verified, 0 warnings, 0 blockers.
- `python tools/rombuild.py -j16 --no-rom`: exit 0; 106/106 modules exact, 11,395/11,395 functions exact, zero mismatches; 2,200 data symbols verified, 690 partial, zero differing, zero unnamed.
- `python tools/check_decl_agreement.py`: no new declaration disagreements.
- `python tools/check_dead_references.py`: no new dead references or broken links.
- `python tools/port_refcheck.py`: 217/217 references resolve.
- `python tools/source_style.py --patterns-only --base origin/main --files src/unnamed/arm9/0204/func_02040c34.c include/private/arm9_a1fc0_file_share.h`: zero findings. Full formatting check could not run because clang-format 19.1.7 is unavailable in this worktree.
- `git diff --check`: pass.
- The private hosted PR validator and independent source review have not run yet.
