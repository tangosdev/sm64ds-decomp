# TU merge queue

Combining one-function files back into the translation units the linker erased.
Ground truth and counts: `notes/tu-cpp-census-2026-08.md`. Data: `notes/data/tu-merge-candidates.json`.
Execution loop: the `decomp-tu-build` skill. Map-reading limits: `decomp-tu-slicing`.

Planned 2026-08-25 against a freshly regenerated `tu_map` chain. No builds were run
during planning; every wall-clock figure below is an estimate that B0 and the pilots
exist to replace with a measurement.

## 0. Corrections to the census, measured against the tree

**0.1 — `create`'s `extern "C"` refusal is 64/173 TUs, not 155/173. Tier 1 is 16/100.**
`externc_block_files` counts files that *contain* an `extern "C" { }` block.
`split_legacy_source` handles those correctly — `_EXTERN_C_BLOCK_RE` + `consume_block`
absorb a declaration-only block into `externs` and keep going. The refusal fires only
when the function *definition* is inside the block, so `body_start` never gets set.

| | TUs affected | files |
|---|---|---|
| Tier 1 | **16 / 100** | 25 |
| Tier 2 | **48 / 73** | 106 |
| total | 64 / 173 | 131 |

**84 of the 100 Tier-1 TUs run `tubuild.py create` unmodified.** Hand-assembly is a
16-TU special case, not the default path — and §2.2 reduces those 16 to zero.

**0.2 — Only 2 safe-pool TUs are already in the manifest, not 5.** `already_in_manifest`
is `False` for all 173 because the census joined on the wrong key. The real key is
`entries[*].functions[*].legacy_source`. Joining on it:

* `ov045/PoleLift` (7 files, text-verified + linkcheck record) — **in the safe pool**
* `ov045/FallBlockBfs` (5 files, text-verified) — **in the safe pool**
* `ov002/Enemy` → `ov002 @0x20ad838 dEnemyBase_c` (30 files) — blocked pool
* `ov002/Platform` → `ov002 @0x20ee42c dBgActor_c` (11) — blocked pool
* `ov002/LevelObjects` → `ov002 @0x20fe3cc` (17) — blocked pool, **and it disagrees with the map**

**0.3 — A link-verified TU falsifies a fresh-map boundary.** `ov002/LevelObjects` is
`link-verified` over `.text 0x020fe190..0x020fea4c`. The regenerated map cuts that range
into `@0x20fe190` (2 files, class `Stage`) + `@0x20fe3cc` (17 files, no class,
medium/medium), and extends the second *past* the verified end to `0x20fea84`, sweeping
in `func_ov002_020fea4c.c` and `func_ov002_020fea68.c`.

In the one place where ground truth exists to check it, a **medium/medium** boundary is
wrong in both directions. That is direct empirical support for the high/high +
module-edge gate, and the reason ov002's medium-boundary TUs stay out of this campaign.

**0.4 — `#pragma long_calls on` is in the safe pool and the census gate did not catch it.**
The census gated `opt_*` and `optimize_for_size`. `long_calls` is equally file-global,
equally last-wins, and changes call codegen (BL vs literal-pool + BX). **12 Tier-1 TUs
and 3 Tier-2 TUs** carry it, in every case on only some members (1–3 of N). Merging
silently applies it to the rest. Worse, `assemble_shadow_source` does **not** carry
pragmas into the output at all — it emits a warning comment — so members that *needed*
it lose it. Both directions are live. Affected Tier 1:

```
ov029/ArrowLift(1/9)  ov029/CageLift(1/9)  ov029/RotatingPlatformWdw(1/5)
ov014/ShutterBob(1/6) ov030/RollingLogTtm(3/6) ov063/FallBlockBbh(2/5)
ov015/FallBlockWf(2/5) ov015/RotatingPlatformWf(1/5) ov022/RotatingPlatformLll(1/5)
ov036/RotatingPlatformRr(1/5) ov064/TiltingPlatformLll(2/5) ov016/FloatOnWaterPlatformJrb(1/4)
```

**0.5 — The census's `n_incomplete` / `n_missing_files` under-report**, because its
`files` list silently drops functions that have no legacy source. Re-derived from
`tubuild.unit_functions` + `tubuild.is_complete`:

* **All 100 Tier-1 TUs: zero missing, zero incomplete**, map function count == census
  file count exactly. The Tier-1 pool is genuinely clean.
* 3 Tier-2 TUs hide a blocker: `ov063/Bookshelf+MansionSteps+MerryGoRound+TrapDoor`
  (`_ZN12MansionSteps13InitResourcesEv` has no source), `ov009/Bird`, `ov055/MirrorLuigi`.
* Same failure inflates §6 below.

Two clean confirmations: cross-joining the C++ census's blocker tags against the safe
pool gives **0** hits for `by-value-class-param` (37 files), `not-in-delinks` (113),
`extra-sections:init` (301), and `NONMATCHING-draft` (55). Caveat:
`c-cpp-classification.tsv` covers only the 7,108 `.c` files, so the 1,194 `.cpp` members
are unvetted by that census — which is why preflight P6 re-derives from delinks directly.

**0.6 — The Tier-1 pool is one template repeated.** `<Class>D1Ev`, `<Class>D0Ev`,
`CleanupResources`, `InitResources`, `Behavior`, `Render`, `OnPendingDestroy`,
`<Class>_Spawn`, plus 0–3 `func_ovNNN_*` helpers. Every TU inspected is a **key-function
TU** (D1+D0 present, `_ZTV<Class>` in range). The reconcile cost for the second TU of a
shape family is a fraction of the first — and that, not TU count, is what sizes a batch.

## 1. The batched queue over Tier 1 (100 TUs / 667 files / net −567)

### 1.1 Why ~8–10 TUs / ~1,300 lines per batch

* **The size cliff is per-TU, not per-batch.** 80% of 1–4-member TUs compiled; 0 of 159
  with ≥10 did. Every Tier-1 TU is 2–9 members, i.e. below the cliff. Batch size does
  *not* trade against compile risk — only against review cost.
* **Review cost scales with merged lines and conflicts, not TU count.** Tier-1 median is
  152 lines/TU, total 15,375. A reviewer is not re-reading function bodies (byte-identical
  moved text) — they read the reconciled declaration set, each `TUBUILD CONFLICT`
  resolution, and the verify transcript.
* **Blast radius is bounded by the manifest.** `tubuild` writes only `src_tu/`,
  `config/tu_manifest.d/`, `build/tu/`. It never touches `src/` or
  `config/**/delinks.txt`, and `promote` is `--dry-run`-only in this tree. A merge batch
  is **purely additive and cannot break the ROM build.** Going to 20 TUs would make the
  PR unreviewable, not risky.

Batches cut on: same risk class, cumulative merged lines ≤ 1,500, TU count ≤ 10.

### 1.2 Risk classes

| class | definition | Tier-1 TUs |
|---|---|---|
| **R0** | no reconcile conflict, no `create` refusal, no pragma | 37 |
| **R1** | reconcile conflicts only (`CONFn` = same-name/different-text collisions) | 37 |
| **R2** | needs the `create` normalizer (`NORMn` = files refused) | 16 |
| **R3** | carries `#pragma long_calls on` | 12 |
| control | already text-verified in the manifest | 2 |

### 1.3 The queue

`WHOLEMOD` = the module has exactly one TU, so the boundary is the module and is not
inferred. `uncorrob` = module-level `corroborated:false` (sinit corroboration
unavailable, not failed). `pcov` = how many of the TU's `.c` members the C++ census
independently proved C++.

**B0 — calibration / regression control (no new source).**
`ov045/PoleLift` (7 files), `ov045/FallBlockBfs` (5 files), both already `text-verified`.
Re-run `compile` + `verify` only. Proves the pinned toolchain, the ROM dump and the
serial-build assumption reproduce a *known-green* transcript before a real pilot is
spent. **Run first; 10 minutes, and it de-risks every later "is it me or is it the TU?"**

**B1 — R0, ov002 + ov006 · 10 TUs / 59 files / net −49 / 1,011 lines**
```
ov002 0x20b05d0  n=8  L=126  InvisiblePole            uncorrob
ov002 0x20b0748  n=8  L=111  CameraTag                uncorrob
ov002 0x20b07f8  n=8  L=121  VirtualDoor              uncorrob
ov002 0x20b5734  n=7  L=164  BlueFlame                uncorrob
ov002 0x20b6e08  n=5  L=67   WaterfallMist            uncorrob
ov002 0x20ec388  n=8  L=148  Warp                     uncorrob
ov002 0x20f0dd0  n=8  L=176  OneUpLogo                uncorrob
ov006 0x20dbd54  n=3  L=36   MgPicturePoker           uncorrob
ov006 0x20fa6ac  n=2  L=30   MgPairAGoneAndOn         uncorrob
ov006 0x210a400  n=2  L=32   MgMushroomRoulette       uncorrob
```
All seven ov002 entries are high/high, which after §0.3 is the confidence floor to trust
in that module. The three ov006 minigame TUs are 2–3 files with **zero includes** — the
cheapest possible probe of post-regeneration ov006.

**B2 — R0, one TU per module · 10 TUs / 69 files / net −59 / 1,356 lines**
```
ov009 0x2112078  n=7  L=116  Flag                     pcov=2
ov012 0x21111a0  n=8  L=175  SwitchPillar
ov013 0x21113bc  n=8  L=153  ClockPaintingHandShort   uncorrob
ov014 0x2112e0c  n=8  L=197  ChainChompFence
ov015 0x21111a0  n=7  L=106  PoleBillboard            uncorrob
ov018 0x21126d4  n=4  L=60   PowerStarCreate          uncorrob
ov019 0x211261c  n=5  L=93   IceSlideManager
ov021 0x2112db4  n=6  L=105  ShutterHmc
ov022 0x2111980  n=8  L=181  LavaBridge               uncorrob
ov022 0x2111cac  n=8  L=170  LavaSeesaw               uncorrob
```

**B3 — R0 · 10 TUs / 53 files / net −43 / 1,076 lines**
```
ov022 0x2111f6c  n=5  L=86   FloatingFloorLllSmall            pcov=3
ov022 0x2112380  n=4  L=60   daObjFl_Fall_Block_c             pcov=2
ov022 0x2112498  n=6  L=100  RollingLogLll
ov026 0x21118b8  n=6  L=115  Submarine
ov029 0x21111a0  n=4  L=135  FloatOnWaterPlatformWdwSquare
ov029 0x2111ea4  n=4  L=124  FloatOnWaterPlatformWdwRectangle
ov036 0x2111580  n=8  L=146  ShipWing
ov036 0x2111d14  n=7  L=155  ArmedRotatingPlatform
ov036 0x2111f8c  n=4  L=75   daObjRc_Dorifu_c
ov043 0x2111518  n=5  L=80   RickshawPlatformBdw              pcov=3
```
`FloatOnWaterPlatformWdwSquare`/`Rectangle` are adjacent and near-identical — this batch
is where shape-family amortization first pays.

**B4 — R0 tail · 7 TUs / 37 files / net −30 / 726 lines**
```
ov043 0x2111630  n=4  L=75   daObjKm1_Dorifu_c
ov044 0x21111a0  n=7  L=88   OrangeBallBillboard   WHOLEMOD (module-edge both ends, corroborated)
ov045 0x2111b14  n=5  L=99   FloatingFloorBfs
ov045 0x2111c30  n=5  L=74   TiltingPlatformBfs    pcov=3
ov047 0x21111a0  n=4  L=68   daObjKm3_Kurumajiku_c pcov=2
ov047 0x21114d4  n=5  L=96   RickshawPlatformBs
ov085 0x212edac  n=7  L=226  WallSign
```
B4 completes ov045: with B0's two, all four ov045 safe TUs are done, making ov045 the
first module where a `config_tu/` conversion becomes conceivable.

**B5 — R1, ov002/ov006 conflicts · 6 TUs / 44 files / net −38 / 1,361 lines**
```
ov002 0x20b09b0  n=9  L=338  Exit                   CONF1
ov002 0x20b46a0  n=8  L=189  MegaMushroomCreateTag  CONF1 pcov=5
ov002 0x20bc5e0  n=8  L=175  HealingHeart           CONF1 pcov=4
ov002 0x20f0894  n=8  L=258  Number                 CONF1
ov002 0x20f11b0  n=9  L=322  BlueCoinSwitch         CONF2
ov006 0x212471c  n=2  L=79   MgTrampolineTerror     CONF1
```

**B6 — R1 · 7 TUs / 54 files / net −47 / 1,283 lines**
```
ov009 0x2111a70  n=8  L=247  CastleWater      CONF1
ov009 0x2111dc4  n=8  L=163  MetalNet         CONF2 pcov=2
ov010 0x21119d0  n=8  L=233  LightBeam        CONF1 pcov=3
ov010 0x2111e10  n=8  L=148  PeachPainting    CONF1 pcov=3
ov012 0x2111450  n=7  L=164  BasementWater    CONF2
ov015 0x2112944  n=7  L=143  RotatingBridge   CONF2
ov016 0x211260c  n=8  L=185  ShipUp           CONF1
```

**B7 — R1 · 8 TUs / 63 files / net −55 / 1,440 lines**
```
ov016 0x2112ff8  n=8  L=233  SlidingBox           CONF2 pcov=3
ov017 0x21111a0  n=7  L=159  ShipWater            CONF2 WHOLEMOD pcov=2
ov022 0x21111a0  n=9  L=240  VolcanoRing          CONF1
ov022 0x21116c4  n=9  L=160  FloatOnLavaPlatform  CONF2
ov022 0x2112130  n=7  L=135  LavaPlank            CONF2
ov025 0x2111d40  n=9  L=203  PyramidStep          CONF1
ov032 0x2112698  n=7  L=155  HugeWater            CONF2
ov033 0x21113d4  n=7  L=155  TinyWater            CONF3 pcov=2
```

**B8 — R1 · 7 TUs / 50 files / net −43 / 1,286 lines**
```
ov036 0x21111a0  n=8  L=163  SwingingPlatform   CONF1
ov043 0x21113fc  n=5  L=107  RickshawBdw        CONF1
ov052 0x21111a0  n=7  L=158  daObjEmmLog_c      CONF3 WHOLEMOD pcov=3 (1 distinct include)
ov052 0x2111440  n=7  L=190  SquarePathLift     CONF2
ov056 0x21111a0  n=7  L=216  BigMovingIceBlock  CONF2 WHOLEMOD pcov=2
ov062 0x211af38  n=8  L=188  KoopaFlag          CONF2 pcov=3
ov064 0x211a930  n=8  L=264  Clam               CONF1
```

**B9 — R1 tail, largest bodies · 5 TUs / 41 files / net −36 / 1,341 lines**
```
ov065 0x211ab60  n=8  L=370  daObjCtMecha05_c   CONF3
ov065 0x211b328  n=8  L=253  TTC_MovingBar      CONF2
ov079 0x2126dbc  n=9  L=280  BillBlaster        CONF2
ov079 0x21271e4  n=9  L=252  FortressWall       CONF1
ov091 0x2132404  n=7  L=186  SlidingPlatformWf  CONF2
```

**B10 — R2, normalizer required · 9 TUs / 60 files / net −51 / 1,416 lines**
```
ov002 0x20b3298  n=7  L=164  daObjAbuku_c           NORM2
ov002 0x20bc414  n=8  L=130  Seaweed                NORM1
ov002 0x20ebf8c  n=8  L=228  Tree                   NORM4
ov006 0x20ede18  n=2  L=30   MgWhichWiggler         NORM1
ov006 0x2119824  n=2  L=29   MgBingoBallSlotsShot   NORM1
ov013 0x21111a0  n=8  L=150  ClockPaintingPendulum  NORM1 pcov=4
ov027 0x21111a0  n=8  L=204  SlidingIce             NORM1
ov029 0x21121a4  n=9  L=245  WDW_Water              NORM1
ov030 0x21111a0  n=8  L=236  daObjHmBskt_c              NORM2 pcov=2
```

**B11 — R2 tail · 4 TUs / 32 files / net −28 / 759 lines**
```
ov033 0x21111a0  n=8  L=151  TinyCover        NORM1
ov043 0x21111a0  n=7  L=145  DiamondLift      NORM1
ov045 0x21111a0  n=9  L=200  FireSeaElevator  NORM1 CONF2
ov064 0x21174a0  n=8  L=263  BigBully         NORM1 pcov=2
```

**B12 — R3, `#pragma long_calls on` · 10 TUs / 59 files / net −49 / 1,265 lines**
```
ov014 0x21111a0  n=6  L=133  ShutterBob               1/6 members
ov015 0x2112bd0  n=5  L=105  RotatingPlatformWf       1/5
ov015 0x2112cf4  n=5  L=80   FallBlockWf              2/5  pcov=3
ov016 0x2112ef4  n=4  L=82   FloatOnWaterPlatformJrb  1/4
ov022 0x21115a8  n=5  L=106  RotatingPlatformLll      1/5
ov029 0x211137c  n=9  L=243  ArrowLift                1/9  CONF2
ov029 0x2111ac4  n=9  L=230  CageLift                 1/9  NORM1
ov029 0x2112080  n=5  L=83   RotatingPlatformWdw      1/5  pcov=3
ov030 0x211155c  n=6  L=96   RollingLogTtm            3/6  pcov=4
ov036 0x2111444  n=5  L=107  RotatingPlatformRr       1/5
```

**B13 — R3 tail · 2 TUs / 10 files / net −8 / 206 lines**
```
ov063 0x211d3a0  n=5  L=94   FallBlockBbh        2/5
ov064 0x2118020  n=5  L=112  TiltingPlatformLll  2/5
```

**Totals: 3 pilots + 13 batches + 2 control = 100 TUs, 667 files, net −567.**

For B12/B13 the pragma decision follows the skill's rule literally — *maximise
cartridge-verified functions, never protect a single pre-existing match*. Run each TU
**twice**: once with `#pragma long_calls on` restored at the top of the merged file, once
without. Keep whichever gives more `MATCH` members; record the losing count in the
manifest `notes`. If neither reaches N/N, admit greedily (§5) rather than shipping a
partial merge silently.

## 2. Per-TU execution recipe

### 2.1 Command sequence

```bash
# 0. GATE — read-only. Do not proceed unless all four lines are clean.
python tools/tubuild.py inspect <ID>
#    require: "functions without `complete`     : none"
#    require: "functions with no legacy source  : none"
#    require: "pragmas in legacy sources        : none"   (or an explicit B12 pragma plan)
#    record : "_ZTV/_ZTI/_ZTS symbols present"  -> this TU is a key-function TU
#    record : "D0/D1/D2 destructor variants"    -> the allowed order exception

# 1. GENERATE the shadow TU + manifest entry
python tools/tu_create.py <ID>      # falls through to `tubuild.py create` when
                                    # nothing needs normalizing

# 2. RECONCILE + COMPILE, iterating
python tools/tubuild.py compile <ID>
#    edit src_tu/actors/<tail>.cpp in place until it compiles clean

# 3. VERIFY — the three-part line, all green
python tools/tubuild.py verify <ID>

# 4. (optional, cheap) one TU compile -> N isolated per-function objects
python tools/tubuild.py partial <ID>

# 5. LINK — agents use --no-rom. NEVER run bare linkcheck concurrently.
python tools/tubuild.py linkcheck <ID> --no-rom
```

`<ID>` is `<module>/<tail>` where `tail` is the single class name, `"+".join(sorted(classes))`
for a multi-class TU (`ov027/ChillBully+daIDonketu_c`), or `@<start>-<end>` for an
unattributed run. If the tail is not unique in the module it becomes `Class@<start8hex>`.
Quote the multi-class ones in the shell.

**`linkcheck` without `--no-rom` runs a ROM build.** That is the shared-`build/` hazard.
Agents always pass `--no-rom`; the human runs one bare `linkcheck` (or `rombuild.py`)
per *batch*, serially, at merge time. `rombuild.py` is the final verdict, not `build_pin`.

### 2.2 The `extern "C"` refusal — the fix

`_EXTERN_C_BLOCK_RE` matches `extern "C" {` on its own line; `consume_block` runs to the
balancing brace and appends every interior line to `externs`. When the definition is
*inside* that block it is consumed as a declaration, `body_start` stays `None`, and
`create` raises `SystemExit`. Example, `src/func_ov010_0211139c.cpp`:

```c
//cpp
extern "C" {
int _ZN7fBase_c18MarkForDestructionEv(void*);
void* _ZN8dActor_c10FindWithIDEj(unsigned int);
int func_ov010_0211139c(char *c){        /* <-- definition, inside the block */
  ...
}
}
```

Drop in `tools/tu_create.py`. It **never writes to `src/`** — it normalizes the text in
memory, monkey-patches `tubuild.split_legacy_source`, and delegates the whole assemble +
`build_manifest_entry` + manifest-upsert path back to `tubuild`, so the shadow file and
the manifest schema stay the tool's own:

```python
#!/usr/bin/env python3
"""tubuild.py create, with an in-memory normalizer for legacy files whose
function DEFINITION sits inside an `extern "C" { }` block (tubuild's splitter
consumes it as a declaration and reports "scanned to end of file without
finding a function body"). Reads src/ only; writes only what tubuild writes."""
import re, sys, argparse, importlib.util, pathlib

REPO = pathlib.Path(__file__).resolve().parent.parent
spec = importlib.util.spec_from_file_location("tubuild", REPO / "tools" / "tubuild.py")
tb = importlib.util.module_from_spec(spec); spec.loader.exec_module(tb)

_BLK = re.compile(r'^\s*extern\s+"C"\s*\{\s*$')

def normalize(text):
    """Close the extern "C" block before the first definition inside it and give
    that definition its own `extern "C"` prefix. Semantics-preserving."""
    lines, out, i = text.splitlines(), [], 0
    n = len(lines)
    while i < n:
        if _BLK.match(lines[i]):
            depth, k, body = 1, i + 1, None
            while k < n and depth > 0:
                d0 = depth; depth += lines[k].count("{") - lines[k].count("}")
                if d0 == 1 and body is None and lines[k].count("{") > lines[k].count("}"):
                    body = k
                k += 1
            close = k - 1
            if body is None:                      # declaration-only block: untouched
                out.extend(lines[i:close + 1]); i = close + 1; continue
            s = body                              # walk back onto the signature line
            while s > i + 1:
                p = lines[s - 1].strip()
                if (not p or p.endswith(";") or p.endswith("}")
                        or p.startswith("//") or p.startswith("/*") or p.endswith("*/")):
                    break
                s -= 1
            out.append(lines[i]); out.extend(lines[i + 1:s]); out.append("}")
            out.append('extern "C" ' + lines[s].lstrip())
            out.extend(lines[s + 1:close])
            i = close + 1; continue
        out.append(lines[i]); i += 1
    return "\n".join(out) + "\n"

_orig = tb.split_legacy_source
def patched(text):
    r = _orig(text)
    if not r["error"]:
        return r
    r2 = _orig(normalize(text))
    if not r2["error"]:
        r2["notes"] = list(r2.get("notes", [])) + [
            "// tu_create.py: definition was inside an extern \"C\" block; "
            "block closed before it and the definition given explicit C linkage"]
    return r2
tb.split_legacy_source = patched

ap = argparse.ArgumentParser()
ap.add_argument("id"); ap.add_argument("--force", action="store_true")
ap.add_argument("--manifest", default=None)
a = ap.parse_args()
a.manifest = pathlib.Path(a.manifest) if a.manifest else None
tb.cmd_create(a)
```

**Validated read-only against the tree**: over all 131 refused safe-pool files this
recovers **130**. The single residual, `src/func_ov018_021118fc.c` (Tier 2,
`ov018/daPgMthr_c`), fails for a different reason worth naming — its definition is
`struct dActor_c* func_ov018_021118fc(char* c) {`, and `split_legacy_source`'s first-word
test sees `struct` in `_DECL_KEYWORDS` and consumes the whole function as a shadow
declaration. **Any definition whose return type is spelled `struct X*` / `enum X` /
`class X*` is misparsed the same way.** It errors loudly here only because the file has
nothing after it; hand-assemble that one.

Interaction to get right: `assemble_shadow_source` already prepends `extern "C" ` to a
`.c` member's body when `cpp_needed`. `normalize` prepends only to definitions that were
*inside* an `extern "C"` block, and in the safe pool those are `.cpp` files, so no double
prefix arises. If you ever hit a `.c` file in this situation, drop the prefix from
`normalize` and let `assemble` add it.

### 2.3 Source order — generated correctly, but verify it

`create` emits `for o, name, addr, size in reversed(ord_rows)`, where `ord_rows` comes
from `unit_functions` sorted by ROM address. The highest-address ROM function is written
first, as required. **Do not reorder the generated file.**

The check lives in `verify`, which builds its ordering from `symbols[*].shndx` — the ELF
**section index** — because each function gets its own `.text` section and `st_value` is
0 for every symbol. It prints:

```
emission order    : N ordinal pair(s) NOT in ROM order: [...]
```

**Pass: N == 0, or the only reported pairs lie within one class's D0/D1/D2 group.** That
exception is precedented — the committed `ov045/PoleLift` entry records
`"functions_occur_in_expected_order": "PARTIAL -- ordinal pair(s) not in ROM order: [(0, 1)]"`
at `text-verified`, and (0,1) is exactly `_ZN8PoleLiftD1Ev`/`_ZN8PoleLiftD0Ev`. Any
cross-group pair is a real order bug and blocks the TU.

### 2.4 Declaration conflicts — reconcile in place

`create` never picks a winner. It keeps the **first** occurrence in ROM-ascending
processing order, comments the loser out with a `TUBUILD CONFLICT` banner, and lists it
under "N item(s) need human review before compiling".

1. **Never build a preamble and never hoist.** Five of five hoisting experiments
   regressed (72→62, 72→46). Files carry real ordering dependencies — some declare
   `typedef int s32;` inline. Keep one declaration **at its first occurrence** and delete
   the later duplicate.
2. **Keep the most complete observation.** `extern Mtx43 data_x` beats `extern char data_x`;
   `void f(void*)` beats implicit `int f()`. Each legacy file declared only what it
   needed; the union beats any single view.
3. Delete the `TUBUILD CONFLICT` banner once resolved. A surviving banner in a PR is an
   unresolved conflict.
4. **`extern "C"` is the diagnostic.** Before believing a clean compile, give every
   definition C linkage and read the errors. C++ silently swallows conflicting
   declarations as *overloads*, so a TU can compile having proven nothing while the wrong
   symbol links. In the merged output every `.c`-origin member already carries
   `extern "C"`; for the `.cpp`-origin members, do the pass by hand once.

### 2.5 Local-struct collisions

Two members' private shadow structs land in one file scope. Loud form: redefinition
error. **Silent form: one member compiles against the other's layout and emits different
bytes with no diagnostic.** `create` detects same-`(kind, name)`-different-text and emits
a `TUBUILD CONFLICT` (included in the `CONFn` counts). The residual risk is same-name,
*same text, different intended type*, which no tool catches. Mitigation is preflight P2
plus reading `verify`'s per-member byte result — a layout collision shows as a byte DIFF
on a member you did not edit.

### 2.6 The three-part verify line

Nothing reaches `text-verified` unless the run printed **all three**:

```
byte comparison   : N/N MATCH
objisolate check  : clean            <- relocation type/addend
reloc-destinations: clean            <- relocation destination identity
```

A byte MATCH alone is **not** proof: `match.compare` wildcards every relocated word, so a
member can reproduce the bytes while calling the wrong function. ov077's
`func_ov077_02124118` called `ApproachLinear` where the ROM calls `ApproachLinear2` —
same signature shape, reported MATCH, cost a day. If any of the three could not run, the
status is "not verified", never "probably fine".

`999 word(s) differ` means **sizes** differ (usually the D0/D1/D2 variant collapse), not
a type error — go to `decomp-cpp-class-form`.

Every TU in this pool is a key-function TU, so vtable/RTTI emission appears as
*unlicensed-but-expected* output. `linkcheck --no-rom` is where a wrong vtable anchor
surfaces. Watch the objisolate addend rule: a `_ZTV*` relocation's addend must **lose 8**
on rebinding; get it wrong and it links clean and corrupts 34 modules.

## 3. Pre-flight static checks — `tu_preflight.py` (proposed, not yet written)

Read-only, no compiles. Takes TU ids or `--batch B3`; exits non-zero on any FAIL.

```
python tu_preflight.py ov023/Squasher
python tu_preflight.py --batch B3
```

| # | check | kills | verdict |
|---|---|---|---|
| **P1** | **any-pragma scan** — `split_legacy_source(text)["pragmas"]`, not just `opt_*` | `long_calls` going file-global (§0.4); `create` dropping a pragma a member needed. Precedent: ov062/001 went 14 match/24 differ from one stray `optimize_for_size on`; removing it gave 36/2 | **FAIL** if non-empty and the TU is not in B12/B13 with an explicit two-run plan |
| **P2** | **local-struct + extern collision scan** — replay `tubuild._merge_field`, print both texts side by side; flag same-key/same-text `struct` decls in ≥3 members as advisory | silent layout divergence (§2.5) | **WARN**; count must equal the batch's `CONFn`, all resolved before `verify` |
| **P3** | **`decl_common.h` usage** — count members including it, print what each actually consumes | it sometimes declares a TU's own functions as **data** → silent mismatch | **WARN**. Policy: drop it and restate the 3–17 lines. Median distinct includes is 6–7; usage 0–6 members/TU (highest `ov016/ShipUp` 6/8) |
| **P4** | **sinit accounting** — module `sinits` / `ctor_entries` / `sinit_vs_tu` / `corroborated`, plus this TU's share | two merged TUs that each carried a sinit must produce **one** | **FAIL** if `sinit_vs_tu != "ok"`. **WARN** on `corroborated:false` (67 of 100 Tier-1 — *unavailable*, not *failed*). Corroboration is module-wide, **not narrowed to this TU** |
| **P5** | **manifest dedupe** — join on `entries[*].functions[*].legacy_source`, **not** the census flag | redoing PoleLift/FallBlockBfs, or fighting an entry that already claims a member (§0.2) | **FAIL** on partial overlap; route whole-TU overlap to B0 as a re-verify |
| **P6** | **completeness re-derived** — `SP.path_for(sym)` not `None`, `is_complete(module, path)`, assert `len(unit_functions) == len(census files)` | the census drops sourceless functions (§0.5). Without `complete`, dsd supplies the range from ROM bytes and **your source is never compiled** | **FAIL** on any missing or incomplete. All 100 Tier-1 pass today; 3 Tier-2 fail |
| **P7** | **`create`-ability probe** — `split_legacy_source`, on error retry `tu_create.normalize` | discovering mid-batch that `create` aborts; the `struct`-return misparse (§2.2) | **INFO**: clean / normalizer-required / HAND-ASSEMBLE. FAIL only on the third |
| **P8** | **inert-marker + blocker cross-check** — `text.startswith("//cpp")` vs `"//cpp" in text[:400]`; assert no member is in the by-value / not-in-delinks / extra-sections / NONMATCHING sets | a file that looks C++ and compiles as C99; a member invisible to the byte gates | **REPORT ONLY, do not tidy.** All four blocker sets currently have 0 safe-pool hits |

Run it over all 100 Tier-1 TUs once before B1 and treat the output as the batch-entry gate.

## 4. Pilots — run serially, one at a time, after B0

### Pilot 1 — `ov023/Squasher`

`.text 0x021111a0..0x02111760` · 9 files · 236 lines · 5 `.cpp` + 4 `.c` · 9 includes.

Chosen over the smaller candidates because:

* **Boundary confidence is not inferred at all.** ov023 has exactly one TU; `inspect`
  reports `left=edge, right=edge`. There is no cut to be wrong about. Contrast the
  4-file `ov018/PowerStarCreate`, smaller but sitting on two inferred `high` boundaries.
* **Sinit accounting is exactly satisfied**: 1 sinit / 1 `.ctor` entry, `sinit_vs_tu=ok`,
  `corroborated=True`, 1 TU. The tightest corroboration available anywhere in the pool.
* **The language variable is pre-settled**: all 4 `.c` members are census-proven C++
  (`pcov=4/4`), so the flip `create` performs is not a guess.
* `create` runs unmodified (0 refusals); one extern conflict to reconcile.
* Zero pragmas, zero incomplete, zero missing.
* **`linkcheck` is maximally meaningful** — the TU is the whole module, so the whole-range
  link *is* the module link, with no gap-owned remainder to confound a vtable-anchor failure.

```bash
python tu_preflight.py ov023/Squasher
python tools/tubuild.py inspect ov023/Squasher
python tools/tu_create.py ov023/Squasher
python tools/tubuild.py compile ov023/Squasher
#   iterate on the Squasher shadow TU until clean
python tools/tubuild.py verify ov023/Squasher
python tools/tubuild.py linkcheck ov023/Squasher --no-rom
```

**PASS**: `verify` prints `byte comparison : 9/9 MATCH`, `objisolate check : clean`,
`reloc-destinations: clean`; `emission order` reports 0 bad pairs **or** only the
`(_ZN8SquasherD1Ev, _ZN8SquasherD0Ev)` pair; `linkcheck --no-rom` reproduces the ov023
range. `_ZTV8Squasher` is emitted as unlicensed-but-expected — record it, do not treat it
as a failure.
**FAIL**: <9/9 → per-member triage, a reconcile bug. Audits dirty at 9/9 → wrong-callee
(the ov077 class); do **not** record `text-verified`. `linkcheck` failing at 9/9 clean →
vtable-anchor / class-form; go to `decomp-cpp-class-form` before touching the merge.

Then run `python tools/rombuild.py` once, serially. It is the final verdict.

### Pilot 2 — `ov027/ChillBully+daIDonketu_c` (multi-class + normalizer)

`.text 0x021115c4..0x021118c8` · 7 files · 193 lines · high/high · `corroborated:false` ·
1 file needs the normalizer.

New mechanisms: (a) **two class labels in one TU** — `inspect` reports `_ZTI12daIDonketu_c`,
`_ZTS12daIDonketu_c`, `_ZTV10ChillBully`, i.e. RTTI for one class and a vtable for the
other, the sharpest available test that the grouping is real; (b) the `+`-joined ID path;
(c) the normalizer in isolation; (d) an *inferred* high/high boundary; (e) `corroborated:false`.

**This is the run that validates the multi-class grouping** — 19 of the 173 safe TUs are
multi-class. If this cannot work as a single file, revisit all 19 before batching them.

### Pilot 3 — `ov036/daObjRcCarpet_c` (normalizer stress, no language flip)

`.text 0x02112158..0x02112538` · 8 files · 191 lines · high/high · **all 8 members are
`.cpp`** · **5 of 8 need the normalizer** · 3 includes.

One of only two all-`.cpp` TUs in the pool, which makes it the clean experiment: **there
is no C→C++ flip**, so any byte difference is attributable to the merge or the normalizer
and nothing else. 5/8 refusals is the heaviest normalizer load in Tier 1.
**A DIFF here indicts `tu_create.normalize`, not the merge** — and the normalizer is used
by B10, B11 and 48 Tier-2 TUs, so that verdict is worth buying early.

If all three pass, B1 can be handed to an agent. If Pilot 3 fails while 1 and 2 pass,
reorder: run B1–B9 (no normalizer needed) and hold B10–B11.

## 5. The Tier 2 campaign — 73 TUs / 1,338 files / net −1,265 / 40,621 lines

Sizes: 31 TUs at 10–14 members, 11 at 15–19, 18 at 20–24, **13 at 25+** (largest:
`ov081/MrBlizzard` 35, `ov102/BobOmb` 35, `ov077/Spiny` 34, `ov077/Lakitu` 32,
`ov085/Rabbit` 32). 18 are multi-class. 48 of 73 need the normalizer.

### 5.1 Greedy admission

The cliff is absolute: **0 of 159 TUs with ≥10 members compiled all-or-nothing.** Never
attempt a full-set compile as the goal; attempt it once as a *probe* and expect failure.

```
1. tu_preflight -> must pass P1/P4/P5/P6; P7 gives the normalizer load
2. tu_create.py <ID>                       # full set, all N members
3. tubuild compile <ID>                    # the probe. If it passes, go to verify.
4. On failure, DO NOT bisect blindly. Seed with the class core:
      D1, D0, InitResources, CleanupResources
   Edit the manifest entry's `functions` list down to the seed, trim src_tu/
   to match (keep ROM-reverse order among survivors), compile, verify.
5. Admit ONE member at a time, in ROM-DESCENDING order (so each new member is
   prepended and existing sections keep their relative order):
      - re-add the member's body + its reconciled declarations
      - re-add its symbol to the manifest `functions` list, ordinal preserved
      - tubuild compile ; tubuild verify
      - KEEP iff the TU still compiles AND no previously-matching member
        regressed to DIFF.  NEVER trade a MATCH for a DIFFER.
      - on reject: revert, record the member + failure in the entry's `notes`,
        move on. Do not retry until the pass is over.
6. After one full pass, retry every rejected member once in the new context.
   Some rejections are ordering artefacts a later admission fixes.
7. Record the final admitted/rejected split in the manifest entry.
```

Cost: a linear pass is **N compiles**, ≈1,338 across Tier 2. Re-probing everything after
every admission is O(N²) — 14,826 compiles — which is why the skill caps at ~200 members.
Every Tier-2 TU is ≤35, so linear plus one retry pass (~2,700 compiles) is affordable.

**Do the language flip inside the admission loop, not before it.** A member that only
matches as C99 should be **left out** rather than sinking the TU — only possible if the
flip and the admission are the same decision.

### 5.2 When a member only matches outside the TU

* **Reading A — the TU is real, the member is a reconcile failure.** The member DIFFs and
  re-admitting it perturbs others, or it DIFFs only once a specific other member is
  present (shared local struct, `decl_common.h` declaration, pragma). Fix the reconcile.
  `tubuild partial <ID>` isolates "this member's codegen changed" from "its context changed."
* **Reading B — the boundary is wrong.** The member is at one **end** of the range, the
  adjacent boundary is `medium` or came from the union-find rather than a class label, and
  it matches perfectly alone. This is the `ov002/LevelObjects` situation (§0.3).

**Decision rule:** if ≥2 rejected members are *contiguous at one end* and that boundary is
not `module-edge`, stop and treat it as a **boundary defect**. Re-cut, record the
evidence, merge the fragments separately. Scattered through the interior means Reading A.

### 5.3 Stopping rules

Bank the partial when any holds:

* **≥80% of members admitted and text-verified.** A 28/35 TU deletes 27 files; the tail
  is not worth it.
* **Two consecutive full passes admit zero new members.** The remainder is structural.
* **A rejected member is contiguous-at-an-end** (Reading B) — stop and re-cut.
* **Wall clock exceeds ~4× the Tier-1 per-TU norm** without crossing 50% admission.

A partial TU **cannot reach `link-verified`** by the whole-range path — the range has
holes. Use `linkcheck <ID> --partial` and record `module_reproduces: NOT RUN`.

### 5.4 Start Tier 2 before Tier 1 completes? No — with one exception

Tier 1 removes 567 files at a *known* per-TU cost with a mechanically generated starting
point and binary pass/fail. Tier 2 removes 1,265 at an unmeasured cost, requires a
hand-built admission loop nobody has run here, and produces *partial* results that are
harder to review. 48 of 73 need the normalizer, whose only validation is Pilot 3; 3 more
have blockers the census concealed; the 13 TUs at 25+ members are exactly where "0 of 159
compiled" was measured.

**Exception — run exactly one Tier-2 TU as a cost probe, after B2:** `ov092/ToxBox`
(`.text 0x2130f00`, 23 files, `module_tus=1`, module-edge both ends, `corroborated:true`,
**all 12 `.c` members census-proven**, `pcov=12/12`). The only Tier-2 TU with a
module-edge boundary on both sides *and* a settled language variable, so an admission
failure there is unambiguously an admission failure. Measure admitted/N, compiles run,
wall clock, then decide whether Tier 2 is a campaign or a backlog. Second-best probe:
`ov094/HootTheOwl` (22 files, whole-module, corroborated) — but `pcov=0`, so it confounds
language with admission.

## 6. Deferred-but-recoverable: ov063 Boo (94) and ov060 Bowser (80)

Neither is "blocked only by pragmas/incomplete members".

**ov063 — `BigBoo+BigBooIcon+Boo+BooCage`, `.text 0x2115ee0..0x211c600`, 94 files.**
Per `tubuild.py inspect` (authoritative over the census): **6 functions without
`complete`**, of which **3 have no legacy source at all** — `func_ov063_021166ac`,
`func_ov063_02117cdc`, `_ZN3Boo6RenderEv`/`_ZN3Boo8BehaviorEv`/`_ZN3Boo13InitResourcesEv`
region. Boo has no legacy source anywhere and no directory of its own; the nearest
named relatives are `src/actors/BigBoo` and `src/actors/BooCage`.
3 members carry `opt_propagation off` / `opt_common_subs off` / `optimize_for_size on`.
**Swallower: 97 functions, 69% of ov063**, carrying 4 class labels, **3 separate vtables**
(`_ZTV3Boo`, `_ZTV7BooCage`, `_ZTV10BigBooIcon`) and 3 destructor pairs. 94 members is
9.4× the cliff.

**ov060 — `Bowser+BowserTail`, `.text 0x2111900..0x2116484`, 80 files.**
**1 function with no legacy source** (`func_ov060_021140c0`; the census said 0). 2 members
carry `opt_common_subs` / `opt_lifetimes`. **Swallower: 81 functions, 53% of ov060.**
3,181 lines, 15 includes.

**What actually unblocks them**, in order:

1. **Write the missing sources** (3 for Boo, 1 for Bowser). Without `complete`, dsd
   supplies the range from ROM bytes and your source is never compiled — a merge over
   them proves nothing. Ordinary matching work, not merge work.
2. **Resolve the swallower.** A 94-function run spanning 69% of a module with 4 class
   labels is far more likely to be *several* TUs the union-find failed to cut than one
   3,500-line file. §0.3 is direct evidence the map over-merges when class labels are sparse.
3. **Then** it is still a 94-member greedy admission, 3–4× the largest Tier-2 TU.

**Worth it? Not yet, and probably not as stated.** 174 files is ~10% of the merge win at
a cost plausibly larger than all of Tier 1, and the "correct grouping" premise is itself
unverified. **The cheap move instead:** run the boundary machinery over ov060 and ov063
specifically, with the RTTI vtable evidence for their classes, and see whether the runs
partition on class-label boundaries. If they do, Boo becomes 3–4 TUs of 20–30 members —
Tier-2-shaped — and 3 of the 4 fragments will not contain the missing sources. A few
hours of analysis with large option value. **Do it after B4, not before.**

## 7. Interface with the C→C++ conversion workstream

See `notes/plan-cpp-conversion-queue.md`. Sequencing rules: §8 of
`notes/tu-cpp-census-2026-08.md`.

**NEEDS from conversion:**

1. **Hands off all 173 safe-pool TUs (2,005 files).** For a mixed TU the merge *is* the
   conversion: `create` sets `cpp_needed` if **any** member has `//cpp`, writes the output
   as `.cpp`, and gives every `.c`-origin member `extern "C"` protection. All 171 mixed
   TUs already have ≥1 `.cpp` member, so the flip is automatic and free inside the merge.
   **241 Tier-1 `.c` files and 811 safe-pool `.c` files are the merge's to flip.**
2. **A D0/D1/D2 form verdict per key-function class, one batch ahead**, before `linkcheck`
   on that class's TU. Every TU here is a key-function TU. `inspect` gives the class→`_ZTV`
   mapping; the *form* has to come from the conversion side.
3. **Live exclusion lists** so preflight P8 stays honest. All four sets are currently
   clean against the `.c` census only — it has 7,108 rows and does not cover the 1,194
   `.cpp` members. If the census extends to `.cpp`, the merge wants the delta.

**OWES conversion:**

1. **Per batch, the list of `.c` files that became C++ inside a merged TU** — strike them
   from the conversion backlog. Tier 1 alone is **241 files**, of which the census had
   independently proven only 75. The merge reaches 166 files the per-file evidence cannot.
2. **The class → key-function-TU mapping** from `inspect`, for every TU touched.
   Notable multi-class case: ov027 carries `_ZTI/_ZTS 12daIDonketu_c` **and**
   `_ZTV10ChillBully` in one TU.
3. **Every RTTI-vs-coined-name contradiction hit.** `ov070/FlameChomp` is Tier-2 rank 1,
   and FlameChomp is already known to sit in `daKrpa_c`'s vtable. The ROM name wins.
4. **Findings §0.1–0.5**, which invalidate three census fields the conversion side may
   also be reading: `already_in_manifest`, `n_incomplete`/`n_missing_files`, and the
   interpretation of `externc_block_files`.

## 8. Shared-resource protocol and cost

`build/` is per-worktree; only `build/objcache` is shared and it is content-keyed, so
parallel *compiles* are safe. **`rombuild.py`, `eligible.py`, `build_pin.py --apply`, and
bare `tubuild.py linkcheck` are not.** Agents use `linkcheck --no-rom`; exactly one bare
ROM build at a time, by hand, at batch-merge boundaries. If the workstreams get their own
worktrees (`decomp-worktree`), this evaporates for everything except the final ROM
verdict — and tearing one down must use `wt-remove.ps1`, never `git worktree remove`.

**Estimates — no builds were run while planning.**

| step | estimate |
|---|---|
| `tu_preflight` | seconds |
| `inspect` | ~10–60 s (runs `build_pin.verify` per function, so it compiles N times) |
| `tu_create.py` | seconds |
| reconcile + `compile` iterations | **10–40 min first-of-shape, 5–15 min thereafter** — the dominant cost |
| `verify` | ~1 min |
| `linkcheck --no-rom` | **minutes** — where vtable-anchor failures surface |

Per batch (8–10 TUs, ~1,300 lines): **4–7 h agent time** for the first batch of a shape
family, **2.5–4.5 h** later; **45–90 min human review** (verify transcripts + conflict
resolutions + merged declaration sets — the bodies are byte-identical moved text and do
not need re-reading); 1 serial `rombuild.py` at merge.

Tier 1 whole: **13 batches + 3 pilots ≈ 45–75 agent-hours, ≈12–18 h review, 15 serial ROM
builds**, for −567 files (5.1% of `src/`).

Tier 2 whole: unmeasurable until the ToxBox probe returns. Lower bound ~1,338 compiles for
one linear pass, ~2,700 with the retry, plus reconcile on 40,621 lines — **plausibly
2.5–4× Tier 1's effort for 2.2× the files**, with partial results that cost more to
review. `ov081/MrBlizzard` (35 members, 1,416 lines, 17 local-struct files, 9 normalizer
files) is the worst single unit; budget it in hours, alone.

**Bounding the commitment:** B0 + Pilot 1 is ~1 hour and settles whether the toolchain and
the approach reproduce. B0 + all 3 pilots + B1 is ~1 working day and settles the
normalizer, the multi-class grouping, and the per-TU reconcile cost — after which the
45–75 h figure becomes a measurement rather than a guess.

---

## 9. PILOT RESULT, 2026-08-25

B0 and the pilot were run serially, then a first batch. **Supersedes nothing above except
where noted.**

### B0 control: one of the two does not reproduce

| control | result |
|---|---|
| `ov045/PoleLift` | **7/7 MATCH, objisolate clean, reloc-destinations clean → TEXT-VERIFIED.** The one out-of-order pair is the D1/D0 group, the precedented exception. |
| `ov045/FallBlockBfs` | **3/5 — NOT verified.** Both destructors `999 word(s) differ` with wrong reloc destinations (`_ZTV10dBgActor_c != 0x021130f4:ov045`). |

`FallBlockBfs` is banked in `config/tu_manifest.d/` as `text-verified` and does not
reproduce today. **A banked status is not evidence.** Re-verify before trusting any entry.

### Pilot 1 `ov023/Squasher`: 9/9 MATCH → TEXT-VERIFIED

`create` produced exactly the one predicted conflict. Three reconcile edits followed, all
of them predicted hazards, and all of them recurring across the queue:

1. **empty-paren prototype** — `extern void func_0200fa04();` called with 3 args. Legal in
   C (unspecified args), illegal in C++. Restate with the signature the call site proves.
2. **destructor redefined** — D1 and D0 each spell `Squasher::~Squasher()`. One definition
   emits both (plus D2). Collapse to one, mirroring `PoleLift.cpp`.
3. **the vptr store** — see below. This is the one worth memorising.

### The vptr store: `+ 2` inside the cast

A merged TU becomes its class's **key-function TU**, so mwcc emits `_ZTV<C>` itself — the
whole vtable, two-word Itanium preamble included — while `decl_common.h` declares
`extern int _ZTV<C>[]` bound to the ROM's **slot array**. The legacy spelling
`p[0] = (int)_ZTV<C>;` then binds to the local definition and the vptr lands **8 bytes low**.
Bytes still "match" (a relocated word is a wildcard to every byte gate); only
`objisolate.plan()` catches it — a *defined* `_ZTV` needs `addend >= 8`, an *undefined* one
needs `addend == 0`.

```c
p[0] = (int)(_ZTV8Squasher + 2);   /* CORRECT -- int-indexed, +8 bytes, folded into the addend */
p[0] = (int)_ZTV8Squasher + 8;     /* WRONG -- mwcc emits an extra ADD; the function's size
                                    * changes (999 word(s) differ) and the addend stays 0 */
```

**The `+ 2` must be inside the cast.** `src_tu/actors/PoleLift.cpp` documents this in a
17-line comment that predates this plan; roughly **88 of the 100 Tier-1 TUs** hand-store a
vtable their own TU owns and will need the same edit. It is the most common reconcile step
in the queue — a known step, not a blocker. *(An earlier revision of this section claimed
those 88 were blocked pending constructor recovery. That was wrong; it was written before
reading the verified sibling.)*

### First batch: 12 small TUs, 4 verified

Run because they were the cheapest, not because the others were blocked.

| TU | files | result |
|---|---|---|
| `ov006/MgPairAGoneAndOn` | 2 | **2/2 TEXT-VERIFIED** — one decl said `(void)`, the definition proved `int*` |
| `ov006/MgPicturePoker` | 3 | **3/3 TEXT-VERIFIED** — same shape, 2 declarations reconciled |
| `ov006/MgMushroomRoulette` | 2 | **2/2 TEXT-VERIFIED** — a local `extern int f[];` declared this TU's own FUNCTION as data |
| `ov006/MgBingoBallSlotsShot` | 2 | **2/2 TEXT-VERIFIED** — needed `tools/tu_create.py`, then clean |
| `ov006/MgWhichWiggler` | 2 | in progress — `this` used as a parameter name (C++ keyword), then a further overload conflict |
| `ov036/daObjRcCarpet_c` | 8 | in progress — one vtable declared twice (`int[]` vs `void*[]`), then `data_ov002_0210af70` redeclared |
| `ov036/daObjRc_Dorifu_c` | 4 | blocked on `mMovingMeshCollider` — the class header does not declare the member. **Header work, not merge work.** |
| `ov043/daObjKm1_Dorifu_c` | 4 | same |
| `ov047/RickshawPlatformBs` | 5 | same, plus an ambiguous overload |
| `ov065/TTC_MovingBar` | 8 | `class 'TTC_MovingBar' redefined` — the loud form of the local-struct collision |
| `ov065/daObjCtMecha05_c` | 8 | `data_ov065_0211c0c8` redeclared, `short` vs other |
| `ov006/MgTrampolineTerror` | 2 | overload conflict at the extern block |

**`tools/tu_create.py` is written and works.** All 3 TUs that `create` refused now generate,
and one verified immediately with no further edits. §2.2's design held up unchanged.

### Recurring reconcile edits, in frequency order

Every failure in this batch was one of five shapes, and none was a boundary error — the map
has been right every time so far:

1. a declaration's parameter list disagrees with the definition in the same TU (`(void)`, or
   empty parens) → restate from the definition;
2. the vptr store needs `+ 2` (≈88 Tier-1 TUs);
3. a symbol restated locally with a different type than the project header gives it
   (`int*` vs `void*` for the game-heap pointer) → drop the local restatement;
4. a function declared as data (`extern int f[];`) in a TU that defines `f` → drop it;
5. a C++ keyword used as an identifier (`this`) → rename to `self`.

Shapes 1, 3 and 4 are mechanical and scriptable. Shape 2 is a one-token edit. Only the
class-header gaps (`mMovingMeshCollider`) and the local-class collisions are real judgment.
