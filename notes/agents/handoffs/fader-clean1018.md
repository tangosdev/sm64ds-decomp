# Fader cleanup handoff

Task: `fader-clean1018`. Producer: `codex-fader-clean1018`.
Base: `39e85ae247da338885b6063c69bf7fef40e99454`.
Candidate: this commit; use the immutable output SHA recorded by the v2 queue.
Review policy: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
Working branch: `cpp/fader-clean1018`. PR and merge were not requested.

## Scope and source review

The production fader TU owns all 30 functions in arm9
`0x020171c8..0x0201787c` (1,716 bytes) and data
`0x0208ea50..0x0208eb54` (260 bytes), under existing complete delinks and
intact-object policy. No enrollment, layout, shared header, symbol, attribution,
actor, or minigame change is included.

- `src/runtime/graphics/fader/d_fader.cpp`: remove seven unnecessary field casts;
  use native cstd, CP15, GX/GXS and ApproachLinear calls; agree with the latter's
  int return/reference parameter; type three matrix pointers; remove stale claims.
- `config/tu_manifest.d/arm9/dFader_c.json`: correct two obsolete trimmed-tail
  descriptions; production already includes all base destructor variants.
- `notes/real-cpp-migration-runbook.md`: extend the existing guide with the small
  library/runtime strategy and running evidence log, excluding actors/minigames.
- `notes/experiments/fader-clean1018.json`: structured producer proof and limits.
- This handoff: exact review scope, repeatable commands, remaining work.

RTTI authenticates the stored fader class names and base relationships. It does
not establish original method names or helper parameter types. The source
continues to document partial reconstruction; see
[the naming provenance note](../../symbol-name-provenance.md).

## Producer proof

The final raw compiler object is identical to the baseline object: 12,920 bytes,
SHA-256 `93cfa557e878e4ef846d30012690703b06c50d919cbfc3793f19f8887e270f49`.
All 49 allocated sections, 128 symbols and 137 relocations agree. The object
contains compiler-emitted extras beyond the 30 production-owned functions;
existing metadata externalization policy was not changed.
Final source SHA-256:
`ffe77bcda5c661173978225292d8647abfa2fc2dbc177c53c57d1a62de0f7a2b`.
Compiler: 2004/b56 with
`-O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc -Cpp_exceptions off`.

Commands run from the producer worktree:

```powershell
python tools/prepush_linkcheck.py --files src/runtime/graphics/fader/d_fader.cpp --json build/fader-clean1018-linkcheck.json
python tools/check_decl_agreement.py --changed 39e85ae247da338885b6063c69bf7fef40e99454
python tools/source_style.py --base 39e85ae247da338885b6063c69bf7fef40e99454 --json build/fader-clean1018-style.json
python tools/check_dead_references.py
python tools/tubuild.py linkcheck --baseline -j16
python tools/rombuild.py -j16 --no-rom
git diff --check
```

Visual Studio is installed at
`C:\Program Files\Microsoft Visual Studio\18\Community`; no custom toolset was selected.
Strict proof: 30/30 verified, zero blind functions, warnings or differences.
Declaration ratchet: no new disagreements/local copies, whole scan 6,935 -> 6,933,
no baseline edit. Source style: zero findings/deferrals. Port: 247 references
resolve. Dead-reference ratchet: no new findings or broken Markdown links.
Host smoke: all math, Timer and fader checks pass, including fade endpoints.

Fresh baseline control and final production both reproduce 106/106 ARM9 modules.
The final production run reproduces 11,334 functions and all 348 source data
claims (14,532 bytes), plus 246 BSS claims (31,320 bytes), with no mismatches or
new symbol errors. Nine inherited symbol errors remain. The broader data advisory
still reports one differing `dCcAcPos_c` vtable outside this scope. Do not describe
these diagnostics as all-green. The baseline scratch ROM hash is control evidence,
not proof of an all-source ROM; the final `--no-rom` run skips container packaging.

Private logs: `build/fader-clean1018-{linkcheck,declarations,baseline,rombuild-final}.log`,
`build/fader-clean1018-host-{configure,build,smoke}.log`, and `build/rombuild-report.json`.
Raw A/B probe artifacts are under the producer's audit snapshot
`build/audit-821-main-39e85ae/build/fader-ab/production-final/` in the primary checkout.
No ROM, object, receipt, or credential is committed.

## Remaining reconstruction and next work

The literal G2x brightness bridge still declares its final argument as int while
G2x.h and the current native definition use short. Direct native calls add signed
16-bit extension: Wipe AdvanceFade 320 -> 328 bytes, Color 148 -> 156 bytes.
Fifteen tested variants did not match. This does not prove impossibility or the
original signature. Study the callee and caller population before changing it.
The base header retains the encoded Memory::operator_delete2 bridge; matrix type
consistency and scene/frame manual dispatch also remain incomplete. Nine emitted
metadata/name records retain their existing licensed externalization.

Next candidate: a separately reserved audit of the three tiny matrix helpers;
then narrow math/cache/palette declarations. Keep actors and minigames out of scope.

## Independent verification

Pending at producer commit. The verifier must claim this exact immutable candidate
in its separate wired worktree and record its own raw-object, relocation,
production, host and source-review evidence in the v2 queue. Review all six changed
files above and retain the partial status. No PR, merge or current-main integration
acceptance is implied by this local candidate.
