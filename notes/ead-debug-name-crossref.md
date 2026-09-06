# EAD debug-name cross-reference

**Status: partially applied.** Confirmed current main's ROM-proven
`CannonHatch` -> `daObjCannonShutter_c` rename and updated the actor gloss from
`CANNON_HATCH` to `CANNON_SHUTTER`. Bumped: `Kumo`
medium -> high in [config/rom-name-glossary.json](../config/rom-name-glossary.json). Added: new `Mip` term
(high confidence). Added to [overlay_actors.md](../symbols/overlay_actors.md): the RTTI-resolved
new-ground entries below (BOOT, DSMT, TITLE, MINIGAME, ENTRY, RESULT,
GAME_OVER, CT_MECHA03, BIG_SNOWMAN, METER, MULTIBOOT, MG_CUP, MG_SLOT1,
MG_SLOT3, MG_AMIDA, MG_MCARLO, MG_FLOWER).

----
**Not applied, on purpose:** the `Guragura` -> `Guruguru` suggestion below (§2, idx 132) — checked and rejected. `Guragura` is already a real, independently RTTI-verified class ([daObjGuragura_c.h](../include/daObjGuragura_c.h), [ov002](../config/arm9/overlays/ov002/symbols.txt):`0x0210905c` `_ZTI15daObjGuragura_c`) with its own header and matched methods; it is a *different actor* from index 132's `RC_GURUGURU` ([ov036](../config/arm9/overlays/ov036/symbols.txt)). The two just happen to romanize similarly. Left as-is rather than risk conflating two unrelated classes. The remaining §2 divergences (STARBASE, HANSWITCH, HS_MOON cluster, PILE/STUMP, SWITCHDOOR, WANWAN_SHUTTER) are unapplied — flagged for a closer look, not acted on.

---
[actor_debug_names.tsv](../symbols/actor_debug_names.tsv) is a literal ROM string table (Tier A, per
[symbol-name-provenance.md](../notes/symbol-name-provenance.md)) keyed on the same index as
`ACTOR_SPAWN_TABLE`. This note joins it, index by index, against the two
existing community-authored sources for the same indices:

- [overlay_actors.md](../symbols/overlay_actors.md) — English actor glosses keyed by index (Tier B:
  the *wiring* is ROM-proven, the English word is a community reading).
- [rom-name-glossary.json](../config/rom-name-glossary.json) — hand-authored romaji-fragment glosses of
  RTTI class names, with a confidence level per entry.

## Method and coverage

All **391** entries (indices 0–390; index 391 is the `END_OF` sentinel) were
parsed out of both files and auto-bucketed by token overlap between the ROM
debug string and the index-matched [overlay_actors.md](../symbols/overlay_actors.md) gloss:

| bucket | count | meaning |
|---|---|---|
| `EXACT` | 30 | byte-identical after case/underscore folding |
| `STRONG_OVERLAP` | 37 | ≥50% token overlap — abbreviation, reordering, or a dropped qualifier |
| `PARTIAL_OVERLAP` | 94 | some shared token, rest is localization |
| `NO_OVERLAP` | 213 | no shared token — **expected** for a terse EAD code vs. a localized English name; only a fraction of these are genuine mismatches |
| `NEW_GROUND` | 17 | [overlay_actors.md](../symbols/overlay_actors.md) has no entry at all for that index |

Of these, **~80 entries were individually reviewed by hand** below (30 exact
matches, a representative ~20 of the 37 strong-overlap matches, the full 17
new-ground indices, and ~12 hand-picked candidates out of the 213+94
no/partial-overlap indices that looked like they might be genuine identity
questions rather than ordinary translation). A further **13 indices** were
cross-checked against `_ZTS…` RTTI class names and `*_Spawn` function names in
`config/arm9/**/symbols.txt` to establish the index → class → glossary join
the task asked for (the same technique used previously for [daKpFr_c](../src/game/actors/d_a_kp_fr.cpp)). This
is a sample, not an exhaustive pass over all 391 — see the bucket counts above
for what wasn't hand-reviewed.

**Important asymmetry check performed and worth flagging:** file order inside
a `symbols.txt` overlay does **not** reliably follow `ACTOR_SPAWN_TABLE`
index order. `StarSwitch_Spawn` (index 12) appears *before*
`ExclamationSwitch_Spawn` (index 11) in [ov002](../config/arm9/overlays/ov002/symbols.txt). Any future join
that infers index order from symbol-file order (as I initially tried for the
slide-decoration case below) needs a real table walk, not a file-order
assumption.

---

## 1. Confirmations

Exact or near-exact matches between the ROM string and the existing gloss.
None of these are currently below `high` confidence in
[config/rom-name-glossary.json](../config/rom-name-glossary.json) themselves, but two matches below strengthen
a `terms` entry that *is* below high — see "Confidence-bump candidates."

### Exact matches (30 of 30 listed)

| idx | ROM string | gloss | source (overlay) |
|---|---|---|---|
| 3 | STAGE | STAGE | [arm9](../config/arm9/symbols.txt) |
| 4 | STAR_SELECT | STAR_SELECT | [ov003](../config/arm9/overlays/ov003/symbols.txt) |
| 9 | YOSHI_EGG | YOSHI_EGG | [ov002](../config/arm9/overlays/ov002/symbols.txt) |
| 12 | STAR_SWITCH | STAR_SWITCH | [ov002](../config/arm9/overlays/ov002/symbols.txt) |
| 31 | PATH_LIFT | PATH_LIFT | [ov100](../config/arm9/overlays/ov100/symbols.txt) |
| 158 | BOOKSHELF | BOOKSHELF | [ov063](../config/arm9/overlays/ov063/symbols.txt) |
| 159 | MERRYGOROUND | MERRY_GO_ROUND | [ov063](../config/arm9/overlays/ov063/symbols.txt) |
| 177 | STAR_CAMERA | STAR_CAMERA | [ov002](../config/arm9/overlays/ov002/symbols.txt) |
| 179 | SILVER_STAR | SILVER_STAR | [ov002](../config/arm9/overlays/ov002/symbols.txt) |
| 191 | PLAYER | PLAYER | [ov002](../config/arm9/overlays/ov002/symbols.txt) |
| 192 | PUSHBLOCK | PUSH_BLOCK | [ov002](../config/arm9/overlays/ov002/symbols.txt) |
| 207 | CANNON | CANNON | [ov098](../config/arm9/overlays/ov098/symbols.txt) |
| 208 | WATERBOMB | WATER_BOMB | [ov098](../config/arm9/overlays/ov098/symbols.txt) |
| 213 | BOOK_SWITCH | BOOK_SWITCH | [ov016](../config/arm9/overlays/ov016/symbols.txt) |
| 224 | SNOWBALL | SNOWBALL | [ov081](../config/arm9/overlays/ov081/symbols.txt) |
| 225 | SHARK | SHARK | [ov090](../config/arm9/overlays/ov090/symbols.txt) |
| 244 | WATER_RING | WATER_RING | [ov064](../config/arm9/overlays/ov064/symbols.txt) |
| 275 | SNOWMAN_BREATH | SNOWMAN_BREATH | [ov027](../config/arm9/overlays/ov027/symbols.txt) |
| 283 | LAST_STAR | LAST_STAR | [ov089](../config/arm9/overlays/ov089/symbols.txt) |
| 286 | TREE | TREE | [ov002](../config/arm9/overlays/ov002/symbols.txt) |
| 288 | COIN | COIN | [ov002](../config/arm9/overlays/ov002/symbols.txt) |
| 289 | RED_COIN | RED_COIN | [ov002](../config/arm9/overlays/ov002/symbols.txt) |
| 290 | BLUE_COIN | BLUE_COIN | [ov002](../config/arm9/overlays/ov002/symbols.txt) |
| 308 | TORNADO | TORNADO | [ov096](../config/arm9/overlays/ov096/symbols.txt) |
| 332 | CAMERA | CAMERA | [arm9](../config/arm9/symbols.txt) |
| 333 | CAMERA_TAG | CAMERA_TAG | [ov002](../config/arm9/overlays/ov002/symbols.txt) |
| 336 | BUTTERFLY | BUTTERFLY | [ov100](../config/arm9/overlays/ov100/symbols.txt) |
| 344 | FISH | FISH | [ov100](../config/arm9/overlays/ov100/symbols.txt) |
| 351 | MUGEN_BGM | MUGEN_BGM | [ov002](../config/arm9/overlays/ov002/symbols.txt) |
| 353 | DOOR | DOOR | [ov100](../config/arm9/overlays/ov100/symbols.txt) |

### Strong-overlap matches (20 of 37 shown — abbreviation/reorder, same referent)

| idx | ROM string | gloss | source | note |
|---|---|---|---|---|
| 18 | ICE_BLOCK_LL | ICE_BLOCK | [ov081](../config/arm9/overlays/ov081/symbols.txt) | `LL` = Lethal Lava Land-adjacent qualifier dropped in the gloss |
| 22–25 | VS_ITEM_BLOCK, CAP_BLOCK_M/W/L | EXCLAMATION_BLOCK_VS, CAP_BLOCK_MARIO/WARIO/LUIGI | [ov102](../config/arm9/overlays/ov102/symbols.txt) | single-letter cap initials expanded to full names |
| 28 | SEESAW | SEESAW_UTM | [ov095](../config/arm9/overlays/ov095/symbols.txt) | course qualifier added in gloss |
| 36 | C1_TRAP | TRAP | [ov010](../config/arm9/overlays/ov010/symbols.txt) | course-code prefix dropped |
| 90 | WL_SUBMARINE | SUBMARINE | [ov026](../config/arm9/overlays/ov026/symbols.txt) | course-code prefix dropped |
| 178 | STAR | POWER_STAR | [ov002](../config/arm9/overlays/ov002/symbols.txt) | qualifier added |
| 186 | PEACH_PRINCESS | PRINCESS_PEACH | [ov085](../config/arm9/overlays/ov085/symbols.txt) | word order flipped, same referent |
| 195 | LUIGI | MIRROR_LUIGI | [ov055](../config/arm9/overlays/ov055/symbols.txt) | qualifier added |
| 197 | WATERFALL | WATERFALL_MIST | [ov002](../config/arm9/overlays/ov002/symbols.txt) | qualifier added |
| 214 | BUBBLE | LAVA_BUBBLE | [ov064](../config/arm9/overlays/ov064/symbols.txt) | qualifier added |
| 226 | MANTA | MANTA_RAY | [ov090](../config/arm9/overlays/ov090/symbols.txt) | suffix expanded |
| 249 | PIANO | MAD_PIANO | [ov063](../config/arm9/overlays/ov063/symbols.txt) | qualifier added |
| 256 / 257 | PENGUIN_BABY / PENGUIN_MOTHER | BABY_PENGUIN / MOTHER_PENGUIN | [ov072](../config/arm9/overlays/ov072/symbols.txt)/[ov018](../config/arm9/overlays/ov018/symbols.txt) | word order flipped |
| 273 / 274 | BIG_SNOWMAN_HEAD / _BODY | SNOWMAN_HEAD / SNOWMAN_BODY | [ov072](../config/arm9/overlays/ov072/symbols.txt) | "BIG" qualifier dropped in gloss (see §3, index 272) |
| 282 | OBJ_KEY | KEY | [ov089](../config/arm9/overlays/ov089/symbols.txt) | `OBJ_` prefix dropped |
| 297 | HEART | HEALING_HEART | [ov002](../config/arm9/overlays/ov002/symbols.txt) | qualifier added |
| 318 | OBJ_FLAMETHROWER | FLAMETHROWER | [ov095](../config/arm9/overlays/ov095/symbols.txt) | `OBJ_` prefix dropped |
| 355 | STAR_CREATE | POWER_STAR_CREATE | [ov018](../config/arm9/overlays/ov018/symbols.txt) | qualifier added |
| 383 / 384 | MG_ROULETTE / MG_TRAMPOLINE | MG_MUSHROOM_ROULETTE / MG_TRAMPOLINE_TIME | [ov006](../config/arm9/overlays/ov006/symbols.txt) | localized minigame subtitle added |

### Enrichment confirmations (RTTI cross-checked, not simple string matches)

| idx | ROM string | existing gloss | what the RTTI/spawn-function join adds |
|---|---|---|---|
| 187 | MIP | RABBIT ([overlay_actors.md](../symbols/overlay_actors.md)) | [ov085/symbols.txt](../config/arm9/overlays/ov085/symbols.txt) has `_ZTS7daMip_c` — the *actual* Nintendo-internal class is literally "Mip," which is the proper name of the rabbit enemy in the JP source (localized as "Rabbit"/"Mips" in EN fan material). The gloss RABBIT is correct as a translation but the ROM hands us the real proper noun for free. Worth adding `Mip` as a `terms` entry in the glossary (high confidence — corroborated by RTTI, the debug string, and `daObj_Mip_Key_c` at index 229). |
| 229 | OBJ_MIP_KEY | RABBIT_KEY ([overlay_actors.md](../symbols/overlay_actors.md)) | `_ZTS15daObj_Mip_Key_c` confirms the same join — `daObj_Mip_Key_c` maps almost character-for-character onto `OBJ_MIP_KEY`. Good demonstration that the debug-name table and the class name agree closely when both exist. |
| 314 | OBJ_KUMO | CLOUD ([overlay_actors.md](../symbols/overlay_actors.md)) | [rom-name-glossary.json](../config/rom-name-glossary.json)'s `Kumo` term is **medium** confidence, noting "could also be spider; cloud fits `daObjKumo_c`'s platform role" as indirect corroboration. The literal ROM debug string `OBJ_KUMO` at the exact index the community already assigned CLOUD to is a second, independent, Tier-A-strength corroboration of the *cloud* reading over the *spider* reading. **Confidence-bump candidate: `Kumo` medium → high.** |

---

## 2. Divergences worth a second look

Cases where the ROM string and the existing gloss plausibly name the same
slot but disagree enough — or where a class-name cross-check surfaced a
conflict already latent in the tree — that someone should look again. These
are *not* the routine terse-code-vs-localized-name gap; each has a specific
reason to doubt the current gloss (or, in one case, doubt the code's own
naming).

| idx | ROM string | existing gloss/name | source(s) | assessment |
|---|---|---|---|---|
| 14 | CANNON_SHUTTER | Former `CANNON_HATCH` gloss; current code uses ROM-proven `daObjCannonShutter_c` | [ov002](../config/arm9/overlays/ov002/symbols.txt) | **Resolved.** [ov002](../config/arm9/overlays/ov002/symbols.txt) has `_ZTS20daObjCannonShutter_c`, and current main already adopted that exact RTTI spelling. The independent ROM debug string confirms "Shutter," so this change updates only the remaining community gloss rather than replacing the stronger class identity. |
| 132 | RC_GURUGURU | ARMED_ROTATING_PLATFORM ([overlay_actors.md](../symbols/overlay_actors.md)); glossary term `Guragura` = "wobbling, unsteady" (high confidence) | [ov036](../config/arm9/overlays/ov036/symbols.txt) | **Likely glossary correction, not just a gloss question.** The ROM debug string is `GURUGURU` (ぐるぐる/ぐるぐる — "round and round," i.e. spinning), not `GURAGURA` (ぐらぐら — "wobbling/unsteady"). These are two different Japanese reduplicated onomatopoeia. "Round and round" fits an *armed rotating platform* far better than "wobbling." Recommend checking whether `config/rom-name-glossary.json`'s `Guragura` term should actually be `Guruguru`. |
| 180 | STARBASE | STAR_MARKER ([overlay_actors.md](../symbols/overlay_actors.md)); codebase uses `StarMarker` as the primary class name (`StarMarker_Spawn`, `_ZN10StarMarker...`) | [ov002](../config/arm9/overlays/ov002/symbols.txt) | **Worth reconciling, not urgent.** [ov002](../config/arm9/overlays/ov002/symbols.txt) shows the class has *two* coexisting names sharing one vtable: `_ZTV10StarMarker` and `_ZTV12daStarBase_c` alias the same address, and the RTTI pair `_ZTI12daStarBase_c`/`_ZTS12daStarBase_c` exists. The RTTI name (Tier A) and the EAD debug string (Tier A) both independently say "StarBase," while the friendlier in-tree name "StarMarker" — which the gloss follows — is the minority-sourced one. Not necessarily wrong (both may be legitimate dev-facing names for the same class), but worth a note that "Base" is the ROM-native term. |
| 11 | HANSWITCH | EXCLAMATION_SWITCH ([overlay_actors.md](../symbols/overlay_actors.md)) | [ov002](../config/arm9/overlays/ov002/symbols.txt) | **Unresolved — flagging for someone with more context.** `ExclamationSwitch_Spawn` exists in [ov002](../config/arm9/overlays/ov002/symbols.txt) right next to `StarSwitch_Spawn`, but has no distinct `_ZTS`/`_ZTI` of its own to check against (it likely shares `StarSwitch`'s class). "Han" doesn't obviously decode to "exclamation" from anything in the glossary's term list. Not confident enough to call it wrong, but it doesn't confirm cleanly either — worth a check of what index 11's actual object model/graphics are. |
| 302–305 | HS_MOON, HS_STAR, HS_Y_STAR, HS_B_STAR | SLIDE_DECORATION_SILVER_STAR / YELLOW_STAR / ORANGE_SMILEY / BLUE_SMILEY ([overlay_actors.md](../symbols/overlay_actors.md)) | [ov031](../config/arm9/overlays/ov031/symbols.txt) | **Possible index-alignment issue, unresolved.** None of the four community labels mention "moon," yet index 302's debug string is literally `HS_MOON`. [ov031](../config/arm9/overlays/ov031/symbols.txt) does have `SlideDecorationSilverStar_Spawn`, `..YellowStar_Spawn`, `..OrangeSmiley_Spawn`, `..BlueSmiley_Spawn`, so the four class names the codebase already uses match the gloss set — but I could not confirm which `_Spawn` binds to which of 302–305 without walking `ACTOR_SPAWN_TABLE` directly (file order is not reliable — see the index-11/12 counterexample above). Either "moon" is a literal fifth decoration shape that got folded into "silver star" by whoever eyeballed the sprite, or the 302→305 assignment in [overlay_actors.md](../symbols/overlay_actors.md) needs re-deriving from the table itself. `HS` is also not currently a recognized `course` prefix in [config/rom-name-glossary.json](../config/rom-name-glossary.json) — worth adding once the course is identified. |
| 27 | PILE | STUMP | [ov091](../config/arm9/overlays/ov091/symbols.txt) | Plausibly the same object (a driven support post vs. a cut tree stump — both a short vertical wooden post), but the words aren't synonyms in English. Worth a quick look at the model to confirm which reading fits. |
| 40 | SWITCHDOOR | SHUTTER_BOB | [ov014](../config/arm9/overlays/ov014/symbols.txt) | The ROM literally says "Door," not "Shutter." Given the CANNON_SHUTTER finding above shows the ROM is careful about Shutter vs. Hatch vs. Door as distinct words, this is worth a second look rather than assuming they're interchangeable. |
| 41 | WANWAN_SHUTTER | CHAIN_CHOMP_FENCE | [ov014](../config/arm9/overlays/ov014/symbols.txt) | `Wanwan` = Chain Chomp is already high-confidence in the glossary, so the actor identity isn't in question — but "Shutter" (ROM) vs. "Fence" (gloss) is the same Shutter/Door/Fence looseness flagged above. Possible rename candidate: "Chain Chomp Shutter." |

---

## 3. New ground

The 17 indices with no entry in [overlay_actors.md](../symbols/overlay_actors.md) at all. 13 of the 17 were
resolved to a real RTTI class via the index → `*_Spawn` → `_ZTS…` join
(the technique the task asked for), giving a solid lead even where no English
gloss exists yet.

| idx | ROM string | RTTI class found | lead |
|---|---|---|---|
| 0 | BOOT | `_ZTS9dScBoot_c` ([arm9](../config/arm9/symbols.txt)) | Boot/startup scene. |
| 1 | DSMT | `_ZTS9dScDSMT_c` ([ov007](../config/arm9/overlays/ov007/symbols.txt)) | Has a nested `graphCallback_c` — draws a graph. Likely a DS system-level check screen (health & safety / multi-card test); gloss uncertain. |
| 2 | TITLE | `_ZTS10dScTitle_c` ([ov003](../config/arm9/overlays/ov003/symbols.txt)) | Title screen. |
| 5 | MINIGAME | `_ZTS11dScMgBase_c` ([ov004](../config/arm9/overlays/ov004/symbols.txt), partial match — base class only) | ov004 hosts the minigame-scene base class; index 5 is plausibly the minigame-select hub scene, but no scene-specific RTTI was found to confirm it's this exact class rather than a sibling. |
| 6 | ENTRY | `_ZTS10dScEntry_c` ([ov075](../config/arm9/overlays/ov075/symbols.txt)) | Has nested `icon_c` and `graphCallback_c` — course-entry / stage-select loading screen with a course icon and a stats graph. |
| 7 | RESULT | *(not found)* | No RTTI class located in this pass. Presumably a results/scoreboard scene; unconfirmed. |
| 8 | GAME_OVER | `_ZTS13dScGameOver_c` ([ov003](../config/arm9/overlays/ov003/symbols.txt)) | Game-over scene. |
| 110 | CT_MECHA03 | `_ZTS16daObjCtMecha03_c` ([ov065](../config/arm9/overlays/ov065/symbols.txt)) | Real, distinct TTC mechanism actor — sits in the gap between "Mecha02" (glossed `TTC_ROTATING_PRISM`, index 109) and "Mecha04" (glossed `TTC_CONVEYOR_BELT_LARGE`, index 111). Nobody has glossed it. |
| 272 | BIG_SNOWMAN | `_ZTS11daBgSnwmn_c` ([ov072](../config/arm9/overlays/ov072/symbols.txt)) | `Bg` + `Snwmn` = "Big Snowman" — the parent/manager actor over `SnowmanHead` (273) and `SnowmanBody` (274), which *are* glossed. This is the missing third member of that family. |
| 334 | METER | `_ZTS8dMeter_c` ([ov002](../config/arm9/overlays/ov002/symbols.txt)) | HUD meter/gauge object — exact match to the debug string. Likely the power-flower or timer meter. |
| 360 | MULTIBOOT | *(not found)* | Presumably related to DS "multiboot" / download-play; no RTTI class located in this pass. |
| 361 | MG_CUP | `_ZTS10dScMgCup_c` ([ov006](../config/arm9/overlays/ov006/symbols.txt)) | Minigame "cup" scene — likely the trophy/category screen. |
| 364 | MG_SLOT1 | `_ZTS12dScMgSlot1_c` ([ov006](../config/arm9/overlays/ov006/symbols.txt)) | Minigame slot-machine scene, variant 1. |
| 365 | MG_SLOT3 | `_ZTS12dScMgSlot3_c` ([ov006](../config/arm9/overlays/ov006/symbols.txt)) | Variant 3. No `MgSlot2` class was found in this pass — may not exist, or may share code with 1 or 3. |
| 371 | MG_AMIDA | `_ZTS12dScMgAmida_c` ([ov006](../config/arm9/overlays/ov006/symbols.txt)) | "Amidakuji" (ghost-leg lottery) minigame scene. |
| 381 | MG_MCARLO | `_ZTS13dScMgMCarlo_c` ([ov006](../config/arm9/overlays/ov006/symbols.txt)) | "Monte Carlo"-style minigame scene — pairs with `MG_MCARLO2` (index 382), which *is* already glossed `MG_PAIR_A_GONE_AND_ON`. This is its missing sibling. |
| 390 | MG_FLOWER | `_ZTS13dScMgFlower_c` ([ov006](../config/arm9/overlays/ov006/symbols.txt)) | Flower-themed minigame scene — last entry before the `END_OF` (391) sentinel. |

---

## Confidence-bump candidates for [config/rom-name-glossary.json](../config/rom-name-glossary.json)

- **`Kumo`: medium → high.** Index 314's debug string `OBJ_KUMO` plus the
  existing `CLOUD(314)` entry in [overlay_actors.md](../symbols/overlay_actors.md) doubly corroborate the
  "cloud" reading over the "spider" reading the note already flagged as the
  ambiguity.
- **`Guragura`: flag for re-derivation, not a simple bump.** Index 132's debug
  string is `GURUGURU`, not `GURAGURA` — a different onomatopoeia with a
  different meaning ("spinning round and round" vs. "wobbling/unsteady").
  Recommend checking whether the term should be renamed/corrected to
  `Guruguru` rather than bumped in place.
- **`Dorifu`: still low, but reinforced.** The literal string `DORIFU`
  appears three times in the debug table (indices 123, 134, 153 — `RC_DORIFU`,
  `KM1_DORIFU`, `KM3_DORIFU`), across three different courses. That's good
  evidence it's a real, deliberate Nintendo term rather than a one-off label,
  but it doesn't resolve *what the word means* (the glossary's actual
  uncertainty), so confidence should stay low pending a real reading.
- **`Mip` (new term, not yet in the glossary):** see §1 enrichment row above —
  high confidence, corroborated by RTTI + debug string + the paired
  `daObj_Mip_Key_c`/`OBJ_MIP_KEY` join.

## What wasn't covered

This pass hand-reviewed roughly 80 of the 391 indices plus 13 RTTI
cross-checks; the bucket counts in "Method and coverage" account for the
remaining ~310. The `NO_OVERLAP` bucket (213 entries) in particular is mostly
unremarkable — terse EAD codes translated into idiomatic English — and a full
manual pass over it would very likely turn up more cases like §2, but at
diminishing returns per entry checked. The `crossref2.tsv` working file this
note was built from (idx / rom_name / addr / gloss / overlay / bucket-status /
token-overlap score for all 391 rows) was scratch output and was not kept in
the repo; regenerate the join in [actor_debug_names.tsv](../symbols/actor_debug_names.tsv) ×
[overlay_actors.md](../symbols/overlay_actors.md) if a fuller pass is wanted.
