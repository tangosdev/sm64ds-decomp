# Moving-mesh coordinate conversions, 2026-10-31

Base: `90cb719e6d701eaf3e657d5426d97355ac3b0da6`. Continuation of PR #4498.

## Source evidence

These method names are inferred from the implementation and its callers;
they are not recovered Nintendo identifiers. The existing dBgW_KcMbg class
identity and layout are unchanged.

| Address | Member | Evidence |
| --- | --- | --- |
| arm9 0x02039e48 | WorldToLocalPosition | Applies invScaledMat, the inverse of the current scaled mesh transform. Ground and line queries transform both endpoints before KCL testing; sphere queries transform their centre. |
| arm9 0x02039e30 | LocalToWorldPosition | Applies newScaledMat. GetTriangleOrigin converts a decoded mesh vertex, and ground/line queries convert the local hit point back to world space. |
| arm9 0x02039e18 | WorldToLocalDirection | Applies invRotMat, constructed after clearing the transform's translation. SetFile/Transform convert the world up vector and horizontal movement direction for the collider's depth policy. |

All eleven call sites now call members declared in dBgW_KcMbg.h. The input
vectors are const references and outputs are references. The legacy
MulVec3Mat4x3 declaration still takes mutable pointers: its body reads the
input and matrix and writes only the output, so these three methods cast
away input constness only at that existing interface. RawVector3/PlainVec
views at existing scratch/up-axis call sites retain the proven representation.

## Verification

Complete before/after objects for 263 affected manifest-backed TUs preserve
allocated section payloads, types, flags, sizes and alignment, defined-symbol
attributes/order and typed relocation destinations. Three named function
identities resolve to their original addresses. Deleting three local
prototypes removes nine named prototype parameters, shifting this TU's
registration node from @251 to @242; only the manifest's source mapping changes.
Report: build/transform-control/comparison.json.

The standalone text audit reports 21/21 function matches, correct emission
order, clean isolation and relocation destinations. Its extra initializer
and BSS diagnostics are covered by the production intact-object gate.
Publication checks on the final sources:

- `python tools/rombuild.py -j16 --no-rom`: PASS, 106/106 exact modules,
  11,395/11,395 functions reproduced, 449/449 source-owned data claims,
  zero new symbol errors and exact storage aliases.
- `python tools/prepush_linkcheck.py --files src/engine/collision/d_bg_w_kc_mbg.cpp`:
  22/22 verified, including the static initializer, with zero warnings/blind slots.
- Declaration agreement and full source style: no new findings.
- Port references: 221 resolve. Dead-reference and language-mode ratchets,
  plus `git diff --check`: pass.

Logs are in build/transform-*.log in the isolated worktree. The PR's independent
CI validation and source review remain separate from these local proofs.

## Deferred work

The live queue snapshot was 8f656317f10c2cf8e2533c7eadc923192b245c78.
Callback-setter migration requires actor callers reserved by other tasks;
direction-to-world migration also requires the reserved decl_common.h.
Those interfaces are left for a separately coordinated continuation.
Open PRs #4555 and #4119 share config/arm9/symbols.txt, but neither changes
these three addresses. The Sound::PlaySub queue task shares the central symbol
file and rename ledger; its reserved arm9 range 0x02012be0..0x02012d2c is
disjoint. No open PR edits the moving-mesh source/header paths. The range service reported
0x02039db8..0x02039e60 free; write locking was unavailable (HTTP 401),
so this non-fleet continuation uses MERGE.md's best-effort fallback.
