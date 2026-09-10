# PR #2483: floating-plank source checkpoint

The leaf keeps ordinary C++ construction and virtual methods. This repair aligns
its shared initializer declarations with their actual definitions, corrects the
profile priority types, and removes unsupported source-history claims. It is a
source checkpoint for independent review; current-main composition and terminal
private validation remain separate.

## Pins and scope

- Public input: `a5bb70d08c2e31e0ddaf78e26fd9a9b0166fed5b`, branch
  `deslop-daobjkiita`.
- Actual Git common base: `c1c8846df622f3a49b6b107e0845e9929473d059`.
  GitHub's cached `224e660e` base does not describe the current comparison.
- Queue task: `pr2483-source-review-0910`; producer session
  `codex-r2398-ki-ita-producer-0910`; policy
  `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Owned files: the leaf header, actor TU, ov016 manifest, and this handoff.
  No shared header, shared initializer, attribution, or enrollment changes.

## Findings carried into independent review

| ID | Kind | Producer disposition and evidence |
| --- | --- | --- |
| KIITA-01 | provenance | Fixed before this checkpoint by public `7bb2a1bb`: the earlier review's `long_calls` rationale was false. The directives remain absent. The current controlled pragma-enabled build also verifies all four functions with identical arrays. This ID preserves the original review finding, which had no stable ID. |
| KIITA-02 | correctness | Fixed: profile fields at +4 and +6 are `u16`, matching the actual unsigned priority consumer and setter contracts. |
| KIITA-03 | provenance | Fixed: concise game-purpose comments replace cleanup history and unsupported original-method/linker-name claims. Manifest size evidence is the factory's allocation, while RTTI proves identity and direct base. Factory/profile spellings are reconstructed names. |
| KIITA-04 | correctness | Fixed: local helper and data declarations now agree with `decl_common.h` and the shared helper definition (`char *`, `char *`; external `char` data anchor). The isolated typed descriptor claimed a different cross-TU interface. |
| KIITA-05 | reconstruction | Retained with measured evidence: normal `new daObjKi_Ita_c()` uses the actor allocator through the existing class allocator. Removing that allocator against these headers changes the call to unresolved `_Znwm`; this one probe does not establish that every alternative is impossible. |
| KIITA-06 | reconstruction | Partial, tracked in [#2563](https://github.com/tangosdev/sm64ds-decomp/issues/2563). The shared ov002 setup still has a legacy `char *` interface, and the table/metadata remain ROM-owned. The active PR review orchestrator owns follow-through, with the leaf producer supplying this evidence; broader shared-header, initializer, and sibling-consumer reconstruction requires a coordinated task. |

## Source and ROM evidence

`daObjKi_Ita_c` derives from `daObjFloatBoard_c`, which derives from `dBgActor_c`.
The leaf adds no storage. The factory allocates `0x348` bytes and installs the
leaf vptr after base construction. Its external constructor relocation is
`_ZN10dBgActor_cC2Ev`; the direct FloatBoard construction is inlined. RTTI at
ov016:`0x02114b80` establishes the leaf and direct base, independently of the
factory's size evidence. The leaf vtable spans `0x02114bcc..0x02114c4c` (32 slots).

The profile is 28 bytes at ov016:`0x02114ba8`. `fBase_c` construction obtains its
profile using the actor ID in the spawn global, then loads the profile priorities
with `ldrh` at arm9:`0x02043eac` (+4) and `0x02043ebc` (+6). Its local profile type
and both priority setters use `u16`. The two small priority constants alone do
not distinguish signedness; the actual consumer contract supports these types.

The resource table occupies ov016:`0x02114b8c..0x02114b98`, with words
`0x02114e44`, `0x02114e4c`, and `0x02113bec`. The shared helper at
ov002:`0x020b5e58` consumes these as model file, collision file, and CLPS pointers.
Its current definition is `int func_ov002_020b5e58(char *, char *)`, matching
`include/decl_common.h`; the common data export is an external `char` anchor.
The leaf explicitly casts `this` at this existing boundary. This does not claim
an original member name, complete typed shared reconstruction, or compiler
impossibility. Issue #2563 preserves that remaining work.

## Fresh local proof

The complete TU was compiled once per public control and repaired candidate with
pinned `2004/b56` and its normal build flags. Both complete objects are identical:
SHA256 `8e542cc4bb7c4bfab9df1486fc35411896fa707af3725f061746d6598114499b`.

| Function | ov016 address | Bytes | Strict result |
| --- | --- | --- | --- |
| `_ZN13daObjKi_Ita_cD1Ev` | `0x02112ef4` | 80 | VERIFIED |
| `_ZN13daObjKi_Ita_cD0Ev` | `0x02112f44` | 100 | VERIFIED |
| `_ZN13daObjKi_Ita_c13InitResourcesEv` | `0x02112fa8` | 20 | VERIFIED |
| `daObjKi_Ita_c_classInit` | `0x02112fbc` | 60 | VERIFIED |

All four functions (260 bytes) have zero blind references, differences, or size
mismatches. All 20 relocations resolve to the configured module identities.
Whole-object policy inspection finds four licensed functions, four clean
isolation plans, ROM-ascending emission, and no unlicensed output or policy
errors. The manifest remains text-only: compiler-only D2 functions and metadata
are handled by its existing deadstrip/externalization policy.

Metadata checks report 10 VERIFIED and 4 PARTIAL, with no DIFFERS. The partial
rows are the inherited `fBase_c`, `dBase_c`, `dActor_c`, and `dBgActor_c` RTTI name
extents; they are not full verification claims or new data ownership. The
adjacent resource table is not added to the manifest's production data scope.

Fresh independent consumer compilation verifies `fBase_c` C2 (arm9:
`0x02043dec`, 352 bytes) and the shared initializer (ov002:`0x020b5e58`, 324 bytes),
both with zero blind references or differences. Header fanout contains only
`src/game/actors/d_a_obj_ki_ita.cpp`, already covered by the four-function proof.
`port_refcheck.py` resolves all 423 references, and `git diff --check` passes.

The pragma-enabled control preserves every function array. Removing the leaf
allocator leaves three functions VERIFIED but makes the factory BLIND-1:
its allocation relocation at +8 changes from `_ZN7fBase_cnwEj` to unresolved
`_Znwm`. The normal candidate retains the verified actor allocator call.

Local artifacts are under `C:/tmp/sm64ds-r2483-0910/build/`:
`baseline/` and `candidate/` contain strict, module, metadata, identity, and
object reports; `whole-tu-proof.json`, `compiler-consumer-probes.json`,
`factory-resource-evidence.json`, `header-consumers.log`, and
`port-refcheck.log` contain the remaining checks. The proof scripts and compiler
probe sources are retained there. No new whole-ROM or private-CI result is
claimed for this source checkpoint; final actual-main integration must supply
its own acceptance and required terminal gates.
