# Translation-unit and filename reconstruction pilot

Status: filename hypotheses only.  No file was moved or renamed.

## Result

Nintendo-style class-family names such as `d_a_obj_waterfall.cpp` are useful
Tier B candidates, but the pilot does **not** find enough evidence to treat
mechanical class-to-filename conversion as recovery.  Recovered TU grouping is
useful; alphabetical link order is not a general predictor in this sample.

The strongest sampled filename candidate is `d_a_obj_waterfall.cpp`: its factory
sits in a compact, high/high-boundary TU containing only the Waterfall class, the
RTTI-derived stem is unique in that TU, and later EAD source organization uses
the same `d_a_obj_*` family.  Even here, there is no surviving SM64DS `__FILE__`
string or linker symbol that proves the spelling.

Several negative results are equally important:

- the current arm9 TU map is under-segmented into one very large interval, so it
  cannot select `d_s_boot.cpp` or `d_s_stage.cpp`;
- the recovered Boo TU has multiple RTTI filename stems;
- the recovered Book TU contains both `daBook_c` and `daBookGen_c`, so the two
  mechanical candidates cannot both be the TU filename;
- several minigame factories occupy tiny factory-only intervals adjacent to the
  class-method interval, showing that current TU boundary recovery is not yet
  production proof;
- local ordering around Waterfall is `lava -> waterfall -> mario_cap ->
  pushblock`, which is not lexical order.

Accordingly, `probable_filename` is blank in the dataset whenever the recovered
TU presents multiple stems.  `class_filename_candidate` retains the mechanical
hypothesis so it can be studied without being promoted.

## Evidence model

The extractor combines:

1. text intervals and boundary confidence from `tools/tu_map.py`;
2. RTTI class-family stems from `tools/tu_names.py`;
3. profiles and factories from the registry extractor;
4. later EAD `d_a*` / `d_s*` source organization as lineage evidence.

`tools/tu_names.py` explicitly emits hypotheses.  No game-code `__FILE__`
strings were found that independently name these SM64DS files.  A unique stem
inside a well-bounded TU raises confidence, but it does not change Tier B to
Tier A.

Static initializers are joined through `tools/sinit_owners.py`, which follows
initializer-touched globals to their consuming TU.  The pilot does not use
`.init` address adjacency: the linker separates `.init` from the owning
object's `.text`, so proximity would be false evidence.

The current `tools/tu_map.py --check` also has a pre-existing `ov063` validation
failure: the current heuristic finds more classed intervals than the check's
expected count.  The generated map remains useful evidence, but the Boo result
is therefore reported conservatively.

## Static-initializer evidence

The machine-readable `static_initializers` field stores symbol, address, size,
classification, causes, evidence, and blockers.  The non-empty sampled
assignments are:

| Recovered TU / sampled profiles | Initializer | Ownership confidence |
|---|---|---|
| under-segmented arm9 / `BOOT`, `STAGE` | 16 initializers from `0x02073a24` through `0x02075154` | medium; cannot distinguish the two sampled classes |
| ov002 Cannon Shutter | `__sinit_ov002_02101968` | high |
| ov010 C1 Trap | `__sinit_ov010_0211203c` | high |
| ov084 Kuribo | `__sinit_ov084_0213035c` | medium |
| ov063 Teresa / Boss Teresa | `__sinit_ov063_0211e29c` | high; shared multi-profile TU |
| ov014 Wanwan | `__sinit_ov014_02113118` | high |
| ov085 Mip Key | `__sinit_ov085_0212f9bc` | high |
| ov070 Propeller Heyho | `__sinit_ov070_02122afc` | high |
| ov020 Book / Book Generator | `__sinit_ov020_02113674` | high; shared multi-class TU |
| ov006 MG Curling | `__sinit_ov006_021304ac` | medium |

No initializer is assigned to the other sampled intervals.  That absence is
recorded as unknown, not proof that the historical source performed no static
initialization.

## Sample results

“Order” describes only the nearby evidence actually available; it is not a
claim about the whole link.

| Profile | Current intake file | Recovered TU | Class-derived candidate | Selected probable filename | Order / lineage evidence | Confidence | Contradiction or limitation |
|---|---|---|---|---|---|---|---|
| `BOOT` | `src/d_s_boot.c` | arm9 `0x020049f0..0x020707a4` | `d_s_boot.cpp` | — | later `d_s_*` family only | — | arm9 interval contains many unrelated families |
| `STAGE` | `src/d_s_stage.cpp` | arm9 `0x020049f0..0x020707a4` | `d_s_stage.cpp` | — | later `d_s_*` family only | — | same under-segmented arm9 interval as `BOOT` |
| `MINIGAME` | `src/func_ov005_020c21ec.c` | ov005 `0x020c21ec..0x020c2250` | `d_s_mini_gm.cpp` | `d_s_mini_gm.cpp` | later `d_s_*`; no RTTI stem in interval | B- | one-function, factory-only interval |
| `CANNON_SHUTTER` | `src/game/actors/d_a_obj_cannon_shutter.cpp` | ov002 `0x020bc8f4..0x020bccfc` | `d_a_obj_cannon_shutter.cpp` | same | unique `d_a_obj_*` stem | B- | right boundary is medium |
| `C1_TRAP` | `src/game/actors/d_a_obj_c1_trap.cpp` | ov010 `0x021111a0..0x021119d0` | `d_a_obj_c1_trap.cpp` | same | unique `d_a_obj_*` stem | B- | left edge lacks independent confidence; registry address is overlay-multiplexed |
| `STAR_CAMERA` | `src/StarCamera_Spawn.cpp` | ov002 `0x020e6c40..0x020ebf8c` | — | — | TU contains a broader Star family | — | no most-derived RTTI class for the factory |
| `PLAYER` | `src/d_a_ply.cpp` | ov002 `0x020bd828..0x020e6c40` | `d_a_ply.cpp` | same | unique `d_a_*` stem | B- | left boundary is medium; very large TU |
| `WATERFALL` | `src/game/actors/d_a_obj_waterfall.cpp` | ov002 `0x020b6e08..0x020b6f18` | `d_a_obj_waterfall.cpp` | same | unique stem and high/high boundaries | B | nearby stems are not alphabetically ordered |
| `KURIBO` | `src/d_a_krb_kuribo.c` | ov084 `0x02129020..0x0212c10c` | `d_a_krb.cpp` | same | one distinct stem | B- | duplicate class entries; left edge lacks confidence |
| `TERESA` | `src/game/actors/daTrs_c/d_a_trs_teresa.cpp` | ov063 `0x02115ee0..0x0211c600` | `d_a_trs.cpp` | — | later `d_a_*` family | — | same TU also yields `d_a_t_basket.cpp`; TU-map check has baseline failure |
| `BOSS_TERESA` | `src/game/actors/daTrs_c/d_a_trs_boss_teresa.cpp` | same ov063 TU | `d_a_trs.cpp` | — | same class as `TERESA` | — | multi-stem TU and two same-class profiles |
| `WANWAN` | `src/ChainChomp_Spawn.cpp` | ov014 `0x02111308..0x02112e0c` | `d_a_wanwan.cpp` | same | unique stem and high/high boundaries | B | nearby `d_a_obj_*` units are not globally lexical |
| `OBJ_MIP_KEY` | `src/d_a_obj_mip_key.c` | ov085 `0x0212cc88..0x0212d528` | `d_a_obj_mip_key.cpp` | same | class family plus high/high boundaries | B | TU-name tool has no surviving stem for this interval |
| `PROPELLER_HEYHO` | `src/game/actors/d_a_propeller_heyho.cpp` | ov070 `0x0211f000..0x02120570` | `d_a_propeller_heyho.cpp` | same | class family only | B- | no recovered interval stem; left edge lacks confidence |
| `KINOKO_CREATE_TAG` | `src/game/actors/d_a_obj_kinoko_tag.cpp` | ov002 `0x020b46a0..0x020b4a70` | `d_a_obj_kinoko_tag.cpp` | same | unique stem and high/high boundaries | B | two same-class factories weaken source organization inference |
| `SHOOT_BOOK` | `src/d_a_book_shoot_book.c` | ov020 `0x021111a0..0x02112938` | `d_a_book.cpp` | — | class-family candidate | — | TU also yields `d_a_book_gen.cpp` |
| `BOOK_GENERATOR` | `src/BookShotSpawner_Spawn.c` | same ov020 TU | `d_a_book_gen.cpp` | — | class-family candidate | — | one TU cannot have both mechanical filenames |
| `MG_CUP` | `src/func_ov006_020e0574.cpp` | ov006 `0x020e0574..0x020e0638` | `d_s_mg_cup.cpp` | same | adjacent earlier class interval has `d_s_mg_cup` | B- | factory is split into its own tiny interval |
| `MG_CURLING` | `src/d_s_mg_curling.c` | ov006 `0x020e0638..0x020e3854` | `d_s_mg_curling.cpp` | same | local `cup -> curling -> curling2` sequence is compatible with related-family order | B- | boundaries are medium/high, not source proof |
| `MG_CURLING_J` | `src/d_s_mg_curling2.c` | ov006 `0x020e6bf4..0x020e6c28` | `d_s_mg_curling2.cpp` | same | immediately follows the `d_s_mg_curling2` method interval | B- | factory-only interval; class and profile suffixes differ |

## WATERFALL ordering trace

The recovered local ov002 sequence is:

```text
0x020b6d28  d_a_obj_lava
0x020b6e08  d_a_obj_waterfall
0x020b6f18  d_a_obj_mario_cap
0x020b8bf0  d_a_obj_pushblock
```

The central unit is compact and coherent, which supports its class/TU ownership.
The surrounding order is not alphabetical, so it does not independently prove
the filename.  The candidate remains Tier B based on unique RTTI family, TU
coherence, semantics, and lineage—not on a claimed lexical link order.

## Answer to the filename hypothesis

Nintendo-style stems are useful for forming candidates and can agree with
coherent TUs. Alphabetical linking did not provide general predictive power in
this pilot. Adopting the lineage convention for repository filenames does not
turn those names into recovered historical facts.
