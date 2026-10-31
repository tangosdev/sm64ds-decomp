# KCL collision predicates, 2026-10-31

Base: `ede5458e426e0776ab968039fdca5a85ad24e01d`. This continuation is independent of PR #4557.

## Evidence and source changes

The names are inferred from their bodies and ITCM callers. Existing RTTI class
identity and layout are unchanged. Both helpers take one scalar and have no
receiver; they become static members of the class whose queries use them.

| Address | Member | Evidence |
| --- | --- | --- |
| arm9 0x020397dc, size 0x20 | dBgW_Kc::IsNearZeroDivisor(int) | Tests -8 <= value <= 8. The ITCM ground query guards division by normal.y; line queries guard their intersection denominator; sphere vertex/edge calculations guard near-singular denominators and hypotenuse division. |
| arm9 0x020397b8, size 0x24 | dBgW_Kc::IsWallNormal(Fix12i) | Tests -0xccc < normalY < 0x600. The line query obtains the surface normal and passes this result to ShouldPassThroughImpl's wall-filter argument. |

The threshold comparisons and bool return ABI are unchanged. In particular,
the upper wall boundary is strict: normalY == 0x600 returns false here,
although func_02039794's slope classifier returns its wall band at that value.
These predicates must not be consolidated merely because their thresholds
look similar.

All five divisor-call expressions (including three macros expanded into the
sphere query) and the wall-filter call use the shared class declaration.
Two copied local extern declarations are removed. The symbol table, two
manifest function identities and rename ledger identify the same ROM addresses.

## Verification

Complete before/after objects for 166 affected manifest-backed TUs preserve
allocated section bytes, type, flags, size and alignment, defined-symbol
attributes/order and typed relocation destinations after canonicalizing the
two explicit function renames. No compiler-local manifest mappings change.
Report: build/prism-control/comparison.json.

Declaration agreement reports no new disagreements or local header
redeclarations. Other final gate results are recorded below.

## Scope and coordination

The live queue has no reservation on these source/header/manifest paths.
The range service reports 0x020397b8..0x02039800 free. Its write authentication
was unavailable in the preceding continuation, so this non-fleet slice retains
the MERGE.md best-effort fallback. Central symbol/rename files are shared,
with this change limited to the two addresses above.

The prism-index and slope-classifier interfaces remain unchanged. The former
still has differing definition/caller return types that require byte-backed
ABI investigation; the latter has callers in the reserved dBgCh_Actr source.
Neither is silently renamed or given a speculative shared signature here.
