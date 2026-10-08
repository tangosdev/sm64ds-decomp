# Monte Carlo card and board readability

This document describes this commit; the queue records its immutable SHA.

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/3171
- Task: `mgread-mcarlo-1007`, producer stage `produce`.
- Producer session: `codex-mgread-mcarlo-producer-1007` (Codex).
- Branch/worktree: `readability/minigames-next-1007`, `C:/tmp/sm64ds-mgread-next-1007`.
- Source input and workflow pin: `c7db694da50d5651fd105d5fb20e4079e25ca6ce`.
- Status: locally verified source candidate; independent review and publication gates follow.

## Scope and changes

Only `src/minigames/d_s_mg_m_carlo.cpp` and this handoff change. The already
promoted `ov006/dScMgMCarlo_c` TU owns 25 functions, 6,336 text bytes in
`ov006:[0x020f7634,0x020f8ef4)`, covering the scene and `dMgMCarloCardObj_c`.
No header, symbol identity, layout, manifest, enrollment or attribution changes.

The card's numeric states now have local C++98 enum constants. These are coined
behavioral names, not recovered original identifiers:

| Value | Name | Evidence |
|---|---|---|
| 0 | `CARD_WAITING` | Init sets it; Update counts down the deal delay before DealIn. |
| 1 | `CARD_ENTERING` | DealIn sets it for cards previously outside the first 20 slots; UpdateBoard continues these cards while a pair is selected. |
| 2 | `CARD_IDLE` | Update sets it when motion/lift finish; this state accepts a new selection or a changed target slot. |
| 3 | `CARD_SELECTED` | A successful HitTest selects the first or matching second card; another HitTest deselects it. |
| 4 | `CARD_MOVING` | DealIn sets it for waiting or repositioned board cards; Update advances position and lift. |
| 5 | `CARD_LEAVING` | FlipAway sets it; Update waits out the column delay, then approaches the left offscreen x coordinate. |

The stored state remains the existing byte member. Scene-state numbers retain
separate meanings and are not replaced by the card constants.

Card slots, target deltas, row/column distances, relative touch coordinates,
list insertion/removal pointers, and weighted-draw locals now say what they
represent. Statement order, narrowing casts and branch structure stay intact.

Comments now describe the actual code: reserve cards continue through common
movement setup; selection cancellation requires a successful HitTest; leaving
cards move left after their delay; one held pick permits input, while a complete
pair is busy; UpdateBoard changes the fixed-point visible-card count, not a
camera coordinate. Render's first pass checks positive vertical step, and its
card blink condition is described by the actual global bit instead of claiming
a touch-state meaning. The Picture Poker source reference uses its current path.

## Proof

All local checks below used the pinned `2004/b56` compiler and production flags.
Evidence is retained in ignored `build/mgread-mcarlo/`.

- `python tools/tubuild.py compile ov006/dScMgMCarlo_c`: exit 0 before and after.
  Both complete raw ELF objects are identical: 21,824 bytes, SHA256
  `c9fe6223fa33cf7bca0b129c01664550972e362b34682f0d3ccc79d097f45e9d`.
  This compares all bytes, including text, emitted metadata, symbols and relocations;
  it is byte neutrality, not a new data-ownership claim.
- `python build/mgread-mcarlo/verify.py`: exit 0. Fresh compile, full-object
  equality, explicit fail-closed objisolate planning/application for each manifest
  function, then linkcheck against ov006 ROM: **25/25 VERIFIED**, every
  `diffs: []`, every `blind: 0`. Report: `strict.json`; objects: `baseline.o`
  and `candidate.o`; baseline source: `baseline.cpp`.
- `git diff --check`: exit 0. Original source line endings are preserved.
- Coordinator's unchanged-base `rombuild.py -j16 --no-rom`: 106/106 modules,
  11,273 source functions, zero mismatches, report
  `build/mgread-next-baseline-rom.json`. Candidate full-ROM, range declaration,
  attribution, port and prepush checks are the integration lane's next action;
  their exact-commit results are recorded separately after this commit.
- No shared header changed, so no new consumer fanout is introduced.
- Independent source acceptance, private validation and merge remain pending.

## Remaining reconstruction

Completion remains partial. Existing scene factory allocation/vptr writes,
address-named helpers/globals, unknown scene/shared-state fields, and ABI callback
spellings are unchanged. Card constructor/destructor and scene destruction retain
the existing compiler-owned lifecycle output; this pass makes no new lifecycle,
RTTI, vtable, static-initializer or data ownership claim. Legacy declaration
conflict notes remain under the existing manifest contract. No compiler barrier
was newly asserted and no failed matching experiment was discarded.

The minigame reconstruction lane retains these follow-ups under issue #3171.
Next: independently review and verify this immutable candidate, run the full
candidate gates, then let the integrator publish and merge the accepted slice.
