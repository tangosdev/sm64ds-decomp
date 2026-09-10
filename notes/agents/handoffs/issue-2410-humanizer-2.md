# Handoff: issue-2410-humanizer-2

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/2410. Task
  `issue-2410-humanizer-2`, stage `revise`, role producer, session
  `codex-humanizer-ukiki-20260907-01`, harness Codex.
- Branch: `cpp/humanizer-ukiki-0907`. Accepted input and source base:
  `26f54f8fc8faaf00568e86d33f37860a46470906`. The accepted promotion handoff is
  `notes/agents/handoffs/issue-2410-daMky_c.md`.
- Queue workflow/tool version: `f327f7b6460e157153eb7fc0749dbbe60dd854f1`.
  Source verification tools are the unmodified tools from the accepted source base.
- Required artifacts: `src/actors/daMky_c.cpp`, `include/daMky_c.h`, and this handoff.
  No separate evidence commit is included.
- Status: locally verified producer candidate; independent verification and
  integration remain pending. The next owner is the independent verifier after
  coordinator review and publication of this immutable output.
- Local evidence is retained under ignored `build/humanizer/`. Private queue
  receipts remain ignored and are not part of the handoff artifact.

## What changed and why

The reserved class is ov030/daMky_c and its existing text TU is the half-open
range `[0x02111688,0x021145e0)`, 44 functions and 12,120 bytes. This continuation
changes only the source, class-header comments, and this handoff.

`daMky_c::EnterState0`, ov030:0x02114124, size 0x10, now writes `mState = 0`
instead of casting `this` to `int*` and writing `p[237]`. The existing header
already declares `s32 mState` at 0x3b4. The new expression reproduces the exact
retail bytes and relocations under the pinned 2004/b56 compiler.

Header comments now count five constructed members and four nontrivial member
destructors, identify the matrix storage at 0x350..0x380 as live, and accurately
describe the position triples' reads and later writes. Deleted method-shard
citations now name the promoted TU and relevant functions. Header token comparison
finds the same 269 non-comment tokens before and after; no declaration, type, field
offset, signature, or layout changed.

The evidence for these descriptions is already present in this TU and
`notes/data/class-facts/daMky_c.json`: `func_ov030_02112094` copies the matrix,
`func_ov030_02111734` reads the first triple and restores positions, and
`func_ov030_02111dd0` updates the saved triple after a ground check. Original field
names remain unknown. `mState` and `EnterState` retain their existing disclosed
reconstruction names; this continuation introduces no recovered-name claim.

## Reconstruction dimensions

- All 44 licensed functions pass exact source-backed link verification, with no
  blind words or differing bytes. The full TU also passes object-isolation,
  relocation-destination, licensed-output, and ROM-order checks.
- The TU still has 20 method symbols represented by 19 C++ definitions, including
  D1/D0 generated from one destructor, and 24 free functions. Eleven remaining
  free functions are documented PMF targets; thirteen are not proven members.
- This change removes the raw receiver access in one method. Other raw offsets,
  shadow receivers, unnamed fields, and explicit ABI calls remain future work.
- Lifecycle emission, key-function ownership, vtable/RTTI policy, initializer
  ownership, and all licensed ROM ranges are unchanged. The generated Bundle
  destructor still has the manifest's existing `$724` identity.
- The nine emitted metadata verdict records are identical to an exact-input
  source control: four VERIFIED and five PARTIAL, with zero DIFFERS or blind
  words. PARTIAL is not promoted to VERIFIED by this handoff.
- No file moves, symbol renames, attribution edits, ledger edits, or tool changes
  are included. The five repaired live contributor overrides are already present
  in the accepted input and remain unchanged. The separate contributor-checker
  and chart discrepancy tracked in #2433 is outside this scope.

## Proof

Checks ran on the accepted base plus exactly the source/header contents committed
here. Their Git blob identities are:

- `src/actors/daMky_c.cpp`: `c90eb23a6e13491bbb296214607b933c93d28d5b`
- `include/daMky_c.h`: `1269958abb0a8f9d635fbd30f84977b76c54db71`

The ignored `build/humanizer/*.meta.json` records retain command arguments, UTC
start times, exit codes, and log names. `scope.json` binds tested source/header
and unchanged manifest hashes; those hashes were checked again before committing.

| Check | Command or exact API used | Result and evidence |
| --- | --- | --- |
| Compiler canary | `build_pin.verify(Path('src/func_ov006_020cb030.cpp'), 'func_ov006_020cb030', 0x020cb030, 0x104, 'ov006')` | Exit 0; `(True, '2004/b56')`; `build/humanizer/canary.log` |
| Changed method | `python tools/linkcheck.py --name _ZN7daMky_c11EnterState0Ev --c src/actors/daMky_c.cpp --addr 0x02114124 --size 0x10 --module ov030 --json build/humanizer/state0-link.json` | Exit 0; VERIFIED, `diffs: []`, `blind: 0`; `build/humanizer/state0-link.log`. This CLI's single-symbol path prints JSON; the log is the actual artifact. |
| Complete TU | `python tools/tubuild.py --manifest build/humanizer/tu-manifest.json verify ov030/daMky_c --version 2004/b56` | Exit 0; 44/44 MATCH, isolation and destinations clean, all 44 in ROM order, no output-policy refusal; `build/humanizer/tu-verify.log`. The scratch manifest was copied from the committed entry so the check would not rewrite it. |
| Every licensed member | `linkcheck.linkcheck(symbol, address, size, 'ov030', names, candidate='src/actors/daMky_c.cpp')` for all 44 manifest rows | Strict wrapper exit 0; 44 VERIFIED, 12,120 bytes, no blind or differing words; `build/humanizer/members-source-link.json` and `.log` |
| Consumer expansion | `python tools/affected_src.py include/daMky_c.h --json` | Exit 0; only `src/actors/daMky_c.cpp`; `build/humanizer/affected.log`. All 44 consumer members are covered above. |
| Full ROM | `python tools/rombuild.py -j 16 --no-rom --report-json build/humanizer/rombuild.json --data-json build/humanizer/romdata-full.json` | Exit 0; 106/106 modules exact; 11,192 source-built functions reproduce, zero mismatch; intact-TU gates pass with zero new symbol errors and exact storage aliases. 9,036 objects reused and one compiled; `build/humanizer/rombuild.log`. |
| Metadata control | `python tools/romdata_check.py --files build/humanizer/baseline/daMky_c.cpp --json build/humanizer/romdata-baseline.json` | Exit 0; exact accepted source copied to ignored storage. `romdata_check.check_object` record comparison also finds all nine records identical to the current full-ROM records; `build/humanizer/metadata-comparison.json`. |
| Header layout | `python tools/check_header_offsets.py include/daMky_c.h` | Exit 0; 17 commented fields, zero mismatched/unparsed, span 0x3cc; `build/humanizer/header-offsets-corrected.log`. An initial bare-class invocation was invalid and its exit 1 is retained separately. |
| Port references | `python tools/port_refcheck.py` | Exit 0; 423 references resolve; `build/humanizer/port-refs.log` |
| Language-mode ratchet | `python tools/langmode_audit.py --check langmode-baseline.json --json build/humanizer/langmode.json` | Exit 0, PASS; `build/humanizer/langmode.log` |
| Legacy prepush check | `python tools/prepush_linkcheck.py --files src/actors/daMky_c.cpp --json build/humanizer/prepush-links.json` | Exit 0 but NO-SYM: zero verified. It is not this TU's proof; the 44 explicit source-backed checks above supply that coverage. |

A diagnostic that passed the raw, unisolated TU object directly to linkcheck
reported 42 VERIFIED and D1/D0 as BENIGN, with zero blind words. Its strict wrapper
correctly exited 1. That bypassed the production vtable-addend adjustment in
`reloc_audit._as_the_build_links_it`. Standard candidate-source checks, which
perform that adjustment, return VERIFIED for both destructors on both the exact
input and this candidate; the complete 44-member source-backed pass above is the
operative result. The initial diagnostic is retained in `members-link.json`.

The full-tree metadata report contains four distinct differing symbols outside
this class's consumer set, as well as unrelated partial/unnamed records. This is
not an all-tree metadata clearance. For this TU, the unchanged PARTIAL records are
the vtable (124 compared bytes versus a 132-byte configured extent) and four RTTI
name strings. Their known extent/alignment limits remain recorded in the accepted
promotion handoff. No configuration extent was changed to improve a verdict.

Private validation and independent verification have not run on this commit.
The integrator must use their exact candidate and composed base; a previous
promotion's green result is not proof of this continuation. A log generated after
this commit belongs in separately recorded evidence, without amending this
candidate merely to insert its own SHA.
