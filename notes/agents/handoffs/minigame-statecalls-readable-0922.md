# Handoff: minigame-statecalls-readable-0922

This document describes this commit. The queue records its immutable output SHA.

## Identity and completion boundary

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/2923.
- Producer/coordinator: `codex-statecalls-readable-0922` (Codex); task `minigame-statecalls-readable-0922`, stage `produce`.
- Branch: `cleanup/minigame-statecalls-readable-0922`; wired worktree `C:/tmp/sm64ds-mg-statecalls0922`.
- Original source base, exact stage input and task workflow/tool revision: `9889d2374ac7d3974da30f6d45f751bfc737fd47`.
- Active independent-review policy observed in the queue: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`; this is separate from the task/source/tool pin.
- Status: locally proved producer candidate, awaiting independent verification. Reconstruction completion remains **partial**: this is a three-function first pass, not a recovered class or original TU.
- Next owner: an independent verifier, assigned by the coordinating root to `build_wiring_audit`; then the integrator owns current-main composition and any PR/merge. This producer does not verify its own acceptance or merge main.

Only these production files and this handoff changed. Their exact half-open intervals are all in ov004:

| Source | ROM interval | Bytes |
| --- | --- | ---: |
| `src/func_ov004_020b586c.cpp` | `[0x020b586c,0x020b58c4)` | 88 |
| `src/func_ov004_020b5a54.cpp` | `[0x020b5a54,0x020b5aac)` | 88 |
| `src/func_ov004_020b5f6c.cpp` | `[0x020b5f6c,0x020b612c)` | 448 |

## Source changes and evidence

The first two helpers now call native `int ApproachLinear(int&, int, int)` rather than spelling its mangled symbol. `src/_Z14ApproachLinearRiii.cpp` defines that contract and returns whether the reference reached the target. The result tests and global stores are preserved.

The third helper includes the existing `Sound.h` and calls `Sound::PlayBank2_2D`. Both that header and `src/_ZN5Sound12PlayBank2_2DEj.cpp` specify an unsigned result; the former local declaration incorrectly returned void. This caller still discards the result. The Sound wrapper's inherited declaration of its own `Play2D` callee differs from the shared header; this change makes no claim to repair that separate owner.

`timedTargetPos`, `targetPos`, `targetX` and `targetY` describe actual data use. `src/func_ov004_020b5108.c` stores the two supplied components at offsets `0x10/0x14`, subtracts the current two-component position, and derives steps. The timed branch waits for ApproachLinear and resets its counter to `0x18`; `src/func_ov004_020b04c0.c` reads the screen-offset global used by the y calculation. These local names are descriptive inferences, not recovered original identifiers. Both arrays, signed widths/shifts, control flow, index/stride expressions, and store/call order remain intact.

Preliminary independent source review by the coordinating root found no blocker and requested a bounded simplification trial for the two `targetX ? targetY : targetY` expressions. A private-copy control, first-only removal, second-only removal, and combined removal each emitted the identical complete 1,624-byte object. Both expressions are therefore simplified to `targetY`. No compiler barrier is claimed: these alternatives were actually measured and succeeded under this pin.

## Producer proof

Compiler: `2004/b56`, production `build_pin.flags_for` flags (`-O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc -Cpp_exceptions off`). Fresh raw compiles were captured before edits and after final source changes. Every byte of each complete ELF is identical, including symbols, relocations and section tables:

| Source suffix | Raw ELF bytes | SHA-256, same before and after |
| --- | ---: | --- |
| `020b586c` | 840 | `e1ecf3c2f9c4299b120bb84eabca07c0c8558ec0f47ca2f31941637e4c2aee85` |
| `020b5a54` | 840 | `19edb119b9837294d2ad2bacee8ed9c06d20edd1c5aad667c19a37e1fd7e6c2e` |
| `020b5f6c` | 1624 | `ace86ed19dd971a1175d6bfb0a77f67078c4c00dcfe60035fd96a167b54fd03c` |

The three objects contain only their own nonzero-size function and no allocated non-text sections. They add no lifecycle, vtable, RTTI, initializer or data ownership. There are no source moves or symbol renames; original attribution remains attached to each path.

- `python build/statecalls-readable/compile_objects.py final`: PASS, all three complete objects identical to the captured base controls. Logs/identity: `final-objects.json`; reproducible driver and raw objects remain local.
- `python tools/prepush_linkcheck.py --files src/func_ov004_020b586c.cpp src/func_ov004_020b5a54.cpp src/func_ov004_020b5f6c.cpp --json build/statecalls-readable/strict.json`: 3/3 VERIFIED, 624 function bytes, every result `diffs: []`, `blind: 0` before the independently measured byte-identical ternary simplification. The final committed-range rerun is recorded separately in producer evidence.
- `python -u tools/rombuild.py -j 16 --report-json build/statecalls-readable/rombuild.json --data-json build/statecalls-readable/romdata.json`: final-source PASS, exit 0; 11,209/11,209 source functions reproduce; 106/106 exact modules; 26/26 source-data claims and 3 BSS claims (172 NOBITS bytes) pass. The actual 16 MiB ROM was packaged at private `build/sm64ds.nds`. Final run reused 7,835 cached production objects and compiled the one source changed by the ternary simplification; the separate three raw object compiles above were fresh.
- Global compiler-data census reports 775 verified, 272 partial and 3 distinct differing symbols across 6,096 object records. The differing symbols are existing `dCcAcPos_c` vtable and `daDemo_c::anmModel_c`/`simpleModel_c` RTTI emissions from unrelated unchanged shards (eight records). These are not a claim of globally clean raw metadata. No changed object emits allocated non-text data. Intact-TU link checks report zero new symbol errors; the baseline control retains its nine historical symbol-check errors.
- `python tools/check_decl_agreement.py --changed 9889d2374ac7d3974da30f6d45f751bfc737fd47`: PASS, no new disagreements; eight files in expanded scope, thirteen banked disagreements. The final committed-range result is recorded separately after this commit. No baseline was edited.
- `python tools/port_refcheck.py --json build/statecalls-readable/port.json`: PASS, 408 references resolve.
- `python tools/tiers_ratchet.py --check`: PASS, baseline 2,995/current 3,007. No banked values were changed.
- `python tools/langmode_audit.py --check build/statecalls-readable/langmode-current-baseline.json --json build/statecalls-readable/langmode-final.json`: PASS against the public baseline at `9f797e7bc2f4c4a620f37f241785afd198256253`. The earlier invocation against stale local chaos-data `0895b9b...` failed and is retained separately; it is not the final gate or a source regression. The baseline file is private evidence, not a repository override.
- `git diff --check`: PASS. Committed-range attribution and strict/declaration checks are run after creating this commit and supplied in `build/statecalls-readable/producer-evidence.json` with the exact candidate identity. Those later logs do not change this commit.

Private proof directory: `build/statecalls-readable/`. `probes/results.json` records the four ternary trials with exact source/object hashes; each probe directory retains its complete source, raw object and compiler log. ROM, compiler inputs, queue receipts and raw objects are not committed or published as source artifacts.

## Remaining work and acceptance limits

The three exported address-based functions, global identities, `0x24` record stride, raw helper interfaces and wider state-machine/class layout remain reconstructed only to the existing level. They require their own evidence-backed continuation; this pass does not invent a class owner, shared layout or original TU boundary. The current issue and coordinator retain that partial reconstruction assessment. Existing declaration debt and Sound's deeper cross-owner contract are not silently declared fixed.

Shared headers, configuration, enrollment, attribution mappings and all baselines are unchanged. No independent verifier verdict, GitHub private validation, PR publication, current-main composition or merge is claimed by this producer handoff. Normal queue publication offers this exact candidate to the independent verifier; later acceptance is separately recorded.
