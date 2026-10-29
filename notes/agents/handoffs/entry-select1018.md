# Entry player-selection introduction

This document describes this commit. The queue records its immutable SHA.

- Task `entry-select1018`, continuation of accepted `entry-sync1018`, under
  <https://github.com/tangosdev/sm64ds-decomp/issues/4084>.
- Producer `codex-entry-select-producer1018`, Codex, branch `cpp/entry-select1018`.
- Source/workflow base: `06fef4f6b6e89e9ccd4116c256cb48e8e717e4e3` (PR #4120).
- Active review policy: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Scope: ov075 `[0x021190a4,0x021192bc)`, two native callbacks, their shared
  field declarations and enrollment, and a stale comment in the methods TU.
- Next owner: independent verifier, then coordinator for a PR based on #4120.
  Publication is authorized by the user's request to continue and repeat the
  preceding recovery. Merging either PR is not authorized.

## Recovered source and evidence

`0211919c` becomes `dScEntry_c::EnterPlayerSelectIntro()` (288 bytes), and
`021190a4` becomes `dScEntry_c::UpdatePlayerSelectIntro()` (248 bytes). All
addresses and sizes remain unchanged. These are inferred method names, not
identifiers preserved in the ROM. The class identity is the already established
Entry scene; this work makes no new original-TU-boundary claim.

The initializer builds step set `0211d768` from the enter PMF at `0211c810`
and the update PMF at `0211c7e8`. Player synchronization schedules this set;
the introduction subsequently schedules interactive selection set `0211d780`.
The screen assets identify the UI: encoded `0x840a` is `en1.narc` member 10,
`d_2d_battle_player_select_BG1_nsc.bin`; `0x980c` is the FGIS-language counterpart
in `c2d.narc` member 12; `0x840b` is `en1.narc` member 11, the corresponding BG2
screen. Asset names describe the files; the actual calls install them on hardware
sub BG2/BG3 respectively.

The enter callback colors a 9-by-10 tile rectangle for the local wireless station,
sets a 180-tick delay, and initializes client readiness. Update positions player
figures, waits for their readiness, then joins the network readiness barrier.
`02020334` sets local flag `0x08`; `02116d40` tests it on each active player.
The host publishes completion through flag `0x20` using `02020124`; all peers
observe it through `02020168` and schedule the interactive step after five ticks.

The header now types the spawned player-figure object at `0x50` as `dEntObj_c *`.
Both scene initialization paths store the `Spawn(0x15a, ...)` object there, and
its actual methods agree with all uses in the reconstructed callback. A forward
declaration avoids adding its full header to every Entry consumer; only the new
update source includes it. The existing member functions are called directly,
retiring both hand-spelled mangled declarations.

`mStepDelay` at `0x264` is a signed countdown in every observed consumer.
`mPlayerSelectReady` is a signed-word alias of the first shared work slot at
`0x268`, alongside the waiting-state view recovered in the preceding PR. Its
union does not claim exclusive ownership of the slot. The class still occupies
`0x288` bytes. The remaining opaque transition pointer at `0x6c` and other legacy
callbacks remain outside this slice.

The G2S getters have no shared header in this revision. Local namespace
declarations use the existing definitions' unsigned-word return type, followed
by explicit pointer casts. This removes the old pointer-returning mangled-name
externs without creating a speculative shared SDK header.

## Matching constraints and bookkeeping

Under production `2004/b56`, declaring `row`, `col`, `tiles`, and `palette` at
function entry in that order preserves the 288-byte initializer. Moving those
declarations into the loops or down to their assignments changes register
allocation. The unsigned palette expression is equivalent to the prior signed
shift/cast expression and independently reproduces the same bytes. Scratch
probes, assembly and strict reports are in `build/select-enter-experiment/`.

The legacy readiness helper currently declares no argument in its own source,
but all four observed callers pass the scene. Using a no-argument call here
removes the retail `r0` setup and emits 244 bytes instead of 248. The existing
scene-argument boundary remains until the helper and all its callers receive
their own reconstruction scope. This is retained partial reconstruction, not
a new public interface claim. Reproduce the probe by removing the `char *`
parameter and the scene argument at the one call, then link-checking
`_ZN10dScEntry_c23UpdatePlayerSelectIntroEv` at ov075 `0x021190a4`, size `0xf8`.

The user's continuation applies the same bounded Entry bookkeeping workflow:
two function-name fields, two appended inferred-name lineage rows, and one
existing declaration-exception file-key move. Authors remain `lunavyqo` at
`021190a4` and `tangosdev` at `0211919c`. The other tasks' reservations and entries
remain intact. There is no baseline reset, additional disagreement, new style
exception, or source edit to the network helper.

## Proof and handoff

Both original functions and the reconstructed member bodies link exactly to the
ROM with zero blind relocations: 536 bytes total. The header consumer closure
contains eight sources, including all three preceding synchronization methods.
Final full-ROM, 30-function strict closure, static gate and independent review
results are recorded in subsequent queue evidence against the immutable commit.
The inherited handoff records the unchanged-base control limitations; these
must remain distinguished from complete source-built code/data fidelity.

The Entry class remains partially reconstructed. Outstanding work includes the
transition step-set helpers and their callers, the remaining state callbacks,
the legacy readiness-helper declaration, and the inherited OAM/network interface
boundaries. Issue 4084 and the next scoped producer retain that continuation.
