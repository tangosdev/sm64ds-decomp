# PR #2484: falling-block source repair

This checkpoint repairs the existing leaf and its separate factory. The factory
uses ordinary C++ construction through the actor allocator; the leaf declarations
now match the actual shared helper definitions. The four-function TU retains its
existing text range. This does not absorb the factory into a five-function TU.

## Identity and ownership

- Public input: `3030d667a05b14669ae8d67ecdb1516aeebd8699`, branch
  `deslop-daobjfl-fall-block`.
- Actual Git source base: `224e660ea0219d0999c58483eb2dd9b38991f5ef`.
- Existing follow-up: [#2486](https://github.com/tangosdev/sm64ds-decomp/issues/2486).
- Task: `pr2484-source-review-0910`; producer
  `codex-r2398-fall-block-producer-0910`; review policy
  `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Pure rename/enrollment commit: `1ec41f86e7e32154d931e187383dc068043321a9`.
  Git records the factory move as R100 before any rewrite. The public symbol
  `daObjFl_Fall_Block_c_classInit` and its separate enrollment remain intact.
- Resources include the class header/TU/manifest, old/new factory paths, ov022
  delinks, this handoff, and the two live profile-registry files. No shared base
  header, shared helper, attribution mapping, or global ledger is edited.

## Findings for independent review

The prior GitHub reviews had no stable IDs. These IDs preserve their findings
and distinguish the newly discovered contract issue from remaining shared work.

| ID | Kind | Producer disposition |
| --- | --- | --- |
| FALL-01 | reconstruction | Fixed: the prior verified natural-new experiment is now implemented in the separately enrolled factory. The class allocation wrapper forwards to the existing actor allocator; no manual vptr writes or raw base-constructor calls remain in that source. |
| FALL-02 | provenance | Fixed: the earlier request to replace manifest notes[0]'s deleted-banner reference was addressed by public e2e19994. The current note retains that correction and now names the .cpp factory and precise four-function TU scope. |
| FALL-03 | correctness | Fixed: the leaf previously declared shared helpers with a leaf pointer and a local ResourceDescriptor pointer, unlike their actual definitions. Setup now uses char*/char**, cleanup char*/void**, with explicit casts at the existing raw boundary. The data anchor matches the common int[] export. |
| FALL-04 | provenance | Fixed: cleanup-history prose and unsupported original Init/Cleanup linker-name claims are removed. The factory symbol is a reconstructed spelling supported by RTTI/registry context, not recovered original source text. Vtable evidence describes the actual definitions anchoring its emission. |
| FALL-05 | provenance | Fixed: both live FL_KUZURE registry representations follow the factory rename. Only current_factory_file and factory_filename change in that one row. Historical census entries are retained as dated observations. |
| FALL-06 | reconstruction | Partial under #2486: shared ov098 helper/base interfaces, common generic helper exports and a coordinated typed model/KCL/CLPS descriptor remain work for humanizer/integrator support through @andrewboudreau. This leaf does not establish original helper identities or a general compiler restriction. |

## Scope and measured behavior

ROM RTTI identifies `daObjFl_Fall_Block_c` with direct base `daObjFallBlock_c`.
The leaf adds no fields. The factory allocation is `0x34c` bytes; RTTI supplies
class/base identity, not allocation-size evidence. The factory retains its public
symbol at ov022:`0x0211245c`, size `0x3c`. Normal construction calls the existing
actor allocator and `dBgActor_c` constructor, with direct-base and leaf vptr
stores supplied by the compiler's inlined construction.

The table at ov022:`0x0211427c` contains three words: `0x02114648`,
`0x02114640`, and `0x0211ba8c`. The actual setup definition in
`src/func_ov098_0213a794.cpp` uses them as model file, collision file and CLPS
pointers. Cleanup in `src/actors/daObjFallBlock_c.cpp` releases the first two
resources. The current raw helper contracts differ (`char **` versus `void **`);
the leaf follows each actual definition and does not invent a common typed
interface locally. `decl_common.h` still has the older generic helper exports;
coordinating those shared declarations with the definitions and sibling leaves
remains explicit in #2486. The resource table stays under its existing ROM data
ownership.

The renamed `.cpp` factory starts with `//cpp`, the repository's actual language
selector. A filename extension alone does not select C++ in this build. The pure
rename precedes this source rewrite so Git can retain the original credit.

## Fresh proof

| Scope | Functions | Bytes | Strict result | Module-correct references |
| --- | ---: | ---: | --- | ---: |
| Existing leaf TU | 4 | 220 | 4 VERIFIED, zero blind/diffs | 18 |
| Separate factory | 1 | 60 | VERIFIED, zero blind/diffs | 4 |

The public control and repaired leaf TU have identical complete compiled objects.
The separate public C factory and natural C++ factory each reproduce the full
60-byte relocated ROM body. Its four configured destinations are preserved.
Whole-TU policy inspection finds four licensed functions, four clean isolation
plans, ROM-ascending emission, and no unlicensed output or policy errors.

Leaf metadata remains eight VERIFIED and five PARTIAL rows, with no DIFFERS.
The five partial type-name extents are fBase_c, dBase_c, dActor_c, dBgActor_c and
daObjFl_Fall_Block_c. Neither factory object emits metadata. No partial record is
reported as fully verified, and no new data/BSS ownership is claimed.

An isolated current-header probe removing the leaf allocator leaves a 60-byte
factory but changes its allocation call to unresolved global `_Znwm` (BLIND-1).
The retained wrapper produces the verified actor-allocator destination. This is
one measured alternative, not a claim that every other form is impossible.
Both actual shared helpers were freshly compiled and verified: setup 332 bytes,
cleanup 72 bytes, each with zero blind references or differences.

Header fanout is exactly the leaf TU and new factory, both fully checked above.
The port reference check resolves all 423 references. The registry's own checks
pass for 401 rows covering 391 actor IDs and 390 unique factory filenames; all
JSON/TSV values agree, and the authoritative source resolver returns the new path.
The classification and TU-candidate inventories are the 2026-08-25 snapshots
identified by `notes/tu-cpp-census-2026-08.md`; their old path and source-form
observations are preserved rather than rewritten as current results.

Local proof is in `C:/tmp/sm64ds-r2484-0910/build/`: `baseline/`, `candidate/`,
`factory-proof.json`, `whole-tu-proof.json`, `interface-probes.json`,
`registry-path-proof.json`, `header-consumers.log`, and `port-refcheck.log`.
Scripts and ignored experiment objects remain there. The immutable output and
canonical contributor-credit check are recorded in producer evidence. Final
current-main composition, independent source acceptance and required terminal
private validation remain separate; old PR comments and historical manifest
build reports do not establish those results for this candidate.
