# Handoff: issue-2411-humanizer-2

This is a local WIP checkpoint, not a published verifier input. The shared actor
return-contract correction remains outstanding. No queue publication or source
push has been performed for this checkpoint.

## Identity and resumption

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/2411; continuation of
  PR #2398, class `da1up_c` (ov002).
- Task `issue-2411-humanizer-2`, producer stage `revise`, session
  `codex-humanizer-oneup-20260907-01`, Codex. The successful producer lease
  preceded edits. `fleet-0907` retains global fleet coordination; the scoped
  coordinator is `codex-humanizer-fixes-20260907-01`.
- Branch `cpp/humanizer-oneup-0907`, worktree
  `C:/tmp/sm64ds-humanizer-oneup-0907`, adopted input
  `19df96cc21d5e2a87a97a22d4de67191319505bd`, source base
  `26f54f8fc8faaf00568e86d33f37860a46470906`, workflow/tool protocol
  `f327f7b6460e157153eb7fc0749dbbe60dd854f1`.
- Earlier accepted source output:
  `bb5bbfae682dccfc1cdb0aab0defdd702ad919c9`. Its history and proof remain in
  [the preceding handoff](issue-2411.md); that proof is not relabeled as proof
  of this modified source.
- Next action: coordinator preserves the accepted Moneybag and Ukiki repair
  outputs, resolves overlapping reservations, then releases/amends/reclaims this
  task before any shared-header edit or adoption merge. The producer resumes
  this checkpoint and proves a coherent actor-hook correction. Independent
  verification follows the final candidate, not this WIP checkpoint.
- All source changes are committed in this checkpoint. Compiler output, probe
  logs and private receipts remain in ignored local build storage.

## What changed and why

The reserved source range remains ov002 `[0x020aee40,0x020b0530)`: 36 functions
in `src/actors/da1up_c.cpp`. Four functions have source-expression changes:

| Function | Address / size | Change |
|---|---|---|
| `func_ov002_020af248` | `0x020af248 / 0x68` | Reads the typed timer member. |
| `func_ov002_020af7cc` | `0x020af7cc / 0x6c` | Initializes that member to `0xffff`. |
| `_ZN7da1up_c8BehaviorEv` | `0x020b00e8 / 0xd8` | Increments and resets that member directly. |
| `_ZN7da1up_c6RenderEv` | `0x020b0070 / 0x78` | Calls `mModel.Render(0)` and removes the fake six-slot `Obj` vtable. |

`include/da1up_c.h` replaces two bytes mislabeled padding with `u16 mStateTimer`
at `0x38c`, preserving the 0x398-byte class layout. The name is explicitly coined
from the observed count/reset behavior when `unk_388` changes; no original member
identifier is claimed. The Model call uses the existing shared class declaration;
no Model header changes were needed.

The header, source comments, `notes/data/class-facts/OneUpMushroom.json`,
`config/tu_manifest.d/ov002/da1up_c.json`, and preceding handoff also correct
virtuality, naming provenance, method-conversion counts, the padding inference,
and the claim that absent original names end reconstruction. The five overrides
were already virtual through inheritance, and all nine method symbols were
already native members before TU packaging. RTTI proves the class name, not all
reconstructed method/signature spellings. The original immediate-offset census
is preserved as historical evidence; its zero count did not prove unused storage.

## Reconstruction dimensions

- Eight native C++ definitions still emit nine method symbols, including D1/D0;
  27 free-function bodies remain. This checkpoint does not count typed accesses
  as new method conversions.
- The `Obj` stand-in is gone. The opaque PMF holder `C` and the local `ModelCache`
  view remain, along with other raw offsets and ABI bridges.
- Destructor, vtable/RTTI, initializer and ROM-data ownership are unchanged.
  The TU still claims text only; factories, PMF descriptors and dispatch storage
  retain their existing ownership.
- Attribution is untouched. Integration commit
  `e3d7f6e6633078ace023d8478002c36526996933` already repaired the eight
  previously reported reassignments. The preceding handoff's old owed-work
  statements are explicitly marked historical.

The return issue is **not fixed**: `func_ov002_020af684` still has an `int`
definition without a return, inherited callers forward its supposed result, and
`dActor_c::OnTurnIntoEgg(Player&)` is also declared `int` while its base veneer
forwards a void operation. Documentation now states that unresolved contract.
No arbitrary return value was introduced.

The bounded investigation supports testing a shared `void` contract: 21 of the
25 actor-hook definitions are nonempty fallthrough bodies, one is empty, and
three only forward helper calls (OneUp also has fallthrough paths). None explicitly
returns a constant or computed non-call value. Their ROM exit sequences perform
no result-producing write after the terminal calls. Key and PowerStar forward
helpers defined as void, or an incorrectly int-declared void destruction method.

An ARM immediate-slot-load/BLX scan found five Player actor call sites, all of which
overwrite `r0` before consuming an outgoing result. It also found 14 ov004
minigame calls that consume a result; those belong to the separate
`dScMgBase_c::OnTurnIntoEgg(int)` contract and must remain int. The scan does not
claim coverage of Thumb or computed slot arithmetic. The shared hypothesis has
not yet been compiled: the proposed edit spans 25 actor headers, 25 definitions
and one Key helper, with 1,730 `dActor_c.h` source consumers. Those shared resources
are not yet reserved by this checkpoint.

## Proof

Measurements below use the assigned worktree and pinned mwccarm 2004/b56. The
counter and Model edits passed independently, then together in the final source.

- Worktree canary `build_pin.verify` for `func_ov006_020cb030`, ov006
  `0x020cb030 / 0x104`: `(True, '2004/b56')`. ROM/compiler/bin inputs are wired;
  the worktree has its own build directory.
- `python tools/tubuild.py verify ov002/da1up_c`: exit 0, **36/36 MATCH**, clean
  relocation type/addend isolation and relocation destinations, all 36 functions
  in expected emission order. Final log `build/humanizer-final-wip-tu.log`.
- `python tools/linkcheck.py --module ov002 --name <symbol> --addr <address>
  --size <size> --c src/actors/da1up_c.cpp`, once for each manifest function:
  **36 VERIFIED, empty diffs, zero blind relocations**, every invocation exit 0.
  Results: `build/humanizer-oneup-linkcheck.json`.
- `python tools/romdata_check.py --files src/actors/da1up_c.cpp --json
  build/humanizer-oneup-romdata.json`: exit 0, 11 emitted data symbols,
  **6 VERIFIED (180 bytes), 5 PARTIAL (44 bytes), 0 DIFFERS, 0 UNNAMED**.
  Partial string-extent coverage is not exact-data proof.
- `python tools/affected_src.py include/da1up_c.h --json`: exactly
  `src/actors/da1up_c.cpp`, covered by the checks above.
- `python tools/check_header_offsets.py include/da1up_c.h`: exit 0,
  14 commented fields, 0 mismatched, 0 unparsed, struct span 0x398.
- The five unchanged actor caller functions were also checked with their
  `srcpath.py`-resolved source paths: **5 VERIFIED, empty diffs, zero blind
  relocations**. Results: `build/humanizer-actor-callers-canonical-linkcheck.json`.
  Earlier attempts passed the Player TU for four functions housed in separate
  shards and returned NO-REPRO; those invocation errors are not source failures
  or verification evidence.
- `python tools/check_dead_references.py`: exit 0, no new dead references
  and no broken Markdown links. `git diff --check` and both changed JSON
  documents also pass. Log `build/humanizer-wip-reference-check.log`.
- Full-ROM build, changed-range header-consumer gate and terminal private
  validation: **not run for this WIP checkpoint**. They remain required after
  the shared-contract continuation is composed and before publication as ready.
  The prior PR's green checks are not assigned to this changed source.

The separate engine-contract draft and reservation inventory are preserved in
ignored coordinator build artifacts. No shared engine header or override outside
OneUp was edited, and no speculative original return type is claimed as proved.
