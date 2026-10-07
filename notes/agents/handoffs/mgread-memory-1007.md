# Memory Match readability pass

This commit is the local producer candidate for task `mgread-memory-1007`.
Producer session: `codex-mgread-memory-1007`. Source/workflow input:
`f1333dd9a366eac154d404cad8d552f0270a0ccc`.

Scope: `src/minigames/d_s_mg_memory.cpp`, class `dScMgMemory_c`, production
ov006 `[0x020f3834, 0x020f5564)`, 47 functions / 7,472 bytes. No header,
manifest, enrollment, attribution, or symbol identity changes.

## Changes

- Name card states with a local C++98 enum and replace state literals throughout
  the card and round routines. Names describe reconstructed behavior; they are
  not recovered original identifiers.
- Express flip-up animation, card selection counters, easy/hard dealing, and
  card dispatch directly through the existing fields, removing pointer aliases
  and redundant receiver/local variables.
- Write the reward as `6 * (3 - mMisses)`, removing the all-ones 64-bit mask,
  unnecessary temporaries and nested blocks while preserving the timer checks.
- Name movement targets, angle/trigonometry values and touch distances; remove
  CardMove's unused byte-stride temporary.

## State evidence

`__sinit_ov006_021311c8` copies seven ROM PMF records into
`data_ov006_02142334`. Reading the ov006 records gives these exact mappings;
all seven PMFs have a zero adjustment word:

| Index | PMF record | Handler address | Handler / new constant |
| --- | --- | --- | --- |
| 0 | 0x0213d018 | 0x020f43c4 | CardMove / CARD_MOVING |
| 1 | 0x0213cff0 | 0x020f43c0 | CardIdle / CARD_IDLE |
| 2 | 0x0213cfe8 | 0x020f4248 | CardSelect / CARD_SELECTABLE |
| 3 | 0x0213cfc8 | 0x020f41b0 | CardFlipUp / CARD_FLIPPING_UP |
| 4 | 0x0213d010 | 0x020f41ac | CardWait / CARD_WAITING |
| 5 | 0x0213d008 | 0x020f411c | CardFlipDown / CARD_FLIPPING_DOWN |
| 6 | 0x0213d070 | 0x020f3f84 | CardFlyAway / CARD_FLYING_AWAY |

## Producer proof

- `python tools/tubuild.py compile ov006/dScMgMemory_c`: the complete baseline
  and candidate ELF objects are identical, 24,488 bytes, SHA256
  `0fad0d6f5f37c8ed938707f7047ce60888d91d04d3c1d1178bdce7516b7618f6`.
  This includes emitted metadata and relocations, not just function bytes.
- All 47 manifest functions: `objisolate.plan` succeeds, followed by
  `reloc_audit._as_the_build_links_it` preparation and `linkcheck.linkcheck`:
  **47 VERIFIED**, every `diffs: []`, every `blind: 0`. The factory passes too;
  old manifest text describing an isolation limitation does not describe this run.
- `python tools/check_decl_agreement.py --changed
  f1333dd9a366eac154d404cad8d552f0270a0ccc`: no new disagreements or local
  redeclarations. Fourteen inherited disagreements remain in the scoped report.
- `git diff --check`: clean.

Private worktree evidence is under `build/mgread-memory/`: `baseline.cpp`,
`baseline.o`, `compile-final.log`, `linkcheck.json`, `verify.py`, and
`experiments.json` with exact attempted patches and complete objects.
The initial mask-only edit changed ResultReward; the final direct reward body
above reproduces it. A direct signed-field UpdateCursor increment also differed
and was rejected. Its unchanged original alias remains; the experiment does not
claim that all clearer cursor forms are impossible.

## Remaining reconstruction and handoff

This is a bounded readability pass, not completed class reconstruction. The
scene-relative card walks, ShuffleCards histogram addressing, CheckFinished
WordAt alias, source order/local compiler pragmas, raw factory construction,
unnamed shared helpers/data, inherited declaration disagreements and unknown
fields remain. No new shared declarations were introduced.

The coordinator owns the composed full-ROM verification and independent source
review against this exact candidate and its base. Those checks are pending at
producer handoff. The candidate is committed locally; no PR, source-artifact
publication or merge is claimed or authorized by this document.
