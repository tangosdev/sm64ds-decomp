# fold/arm9-actor-1010 -- arm9/Actor promotion (dActor_c)

## Target

`arm9/Actor` -- the `dActor_c` translation unit, `0x0200f658..0x02011654`,
97 functions. A link-verified staged source already sat at
`src_tu/actors/Actor.cpp` (the pilot TU from
`notes/tu-reconstruction-pilot-report.md`); this run promoted it to
`src/actors/dActor_c.cpp`.

Engine target -- allowed under the current sweep (Player remains
excluded). Nothing else claims it: no live worktree, branch, or open PR.

## Partition (daObjKuruma_c precedent, AGENTS.md destructor row)

The ROM orders the destructor triple `D2, D0, D1` and interleaves
`_ZN8Vector3sD1Ev` between `C1` and `C2` -- orders no single TU can emit.
`~dActor_c` is also the class key function, so its real member definition
is what emits `_ZTV8dActor_c`/`_ZTI8dActor_c` for `romdata_check`. The six
tail functions therefore stay as the original per-function shards:

- `src/_ZN8dActor_cD2Ev.cpp` `0x020112c8..0x02011314`
- `src/_ZN8dActor_cD0Ev.cpp` `0x02011314..0x02011374`
- `src/_ZN8dActor_cD1Ev.cpp` `0x02011374..0x020113c0`
- `src/_ZN8dActor_cC1Ev.cpp` `0x020113c0..0x02011508`
- `src/_ZN8Vector3sD1Ev.cpp` `0x02011508..0x0201150c`
- `src/_ZN8dActor_cC2Ev.cpp` `0x0201150c..0x02011654`

The promoted TU licenses `0x0200f658..0x020112c8` (91 functions). An
earlier revision absorbed all 97 as mangled `extern "C"` definitions;
that matched text but emitted no vtable, and the validator's ROM-data
ratchet lost `arm9:_ZTV8dActor_c` (verified bytes 47952 -> 47828). The
restored shards re-emit it: `romdata_check` verifies `_ZTV8dActor_c`
(124 bytes) and `_ZTI8dActor_c` again.

## What landed

- `src/actors/dActor_c.cpp`: the staged TU promoted in place, deslopped
  per current conventions -- `dActor_c::Spawn(...)` call sites now use the
  static member spelling instead of a local `extern "C"` alias; the three
  `dBgCh_LinPad`/`RaycastGroundPod` POD dodge declarations carry
  `local extern:` reasons; `// @symbol <mangled>` markers on the 91
  definitions so the converted ratchet scores members, not the whole file.
- Readability ports recovered from the retired shards (they were built
  from a newer snapshot than the staged file): `BumpedUnderneathByPlayer`
  uses `Player` members, `HorzAngleToCPlayer`/`HorzAngleToFPlayer` use
  named position fields, and `DetectRaycastClsn` takes the real
  `dBgCh_Lin` member form (ctor/dtor are out-of-line, so the member
  spelling emits identical calls) -- all re-verified byte-exact.
- Config: `config/arm9/delinks.txt` consolidates the 91-function span and
  keeps the six shard blocks at their original ranges;
  `config/tu_manifest.d/arm9/Actor.json` records the promoted source, the
  `_ZN7Vector3D1Ev` `deadstrip-duplicate` policy
  (canonical home arm9:0x020072c0), and the verify results;
  `attribution.json` carries `path#symbol` overrides for the consolidated
  functions (tangosdev, matching shard credit); `port/slice_gate9.txt`
  and the live comments in `include/daHanachan_c.h`,
  `src/actors/daObjSwdoor_c.cpp`, `src/game/actors/d_a_sound_obj.cpp`,
  `src/game/actors/daObjPushblock_c.cpp` point at the promoted file.
- `config/converted-baseline.json`: banked member identities migrated to
  `src/actors/dActor_c.cpp#symbol` keys; one member sits in
  `converted-backslide-exceptions.jsonl` with a reason (below).
- `config/decl-agreement-baseline.json`: rows banked for every
  declaration another file (or this file's own reconciled externs) makes
  that disagrees with the moved definitions, e.g. `decl_Actor.h`'s stub
  `int` prototypes and sibling callers' loose `Spawn` spellings.

## Deliberate exceptions (backslide rows)

- `_ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c` keeps `unk_0a4`/`unk_0ac`:
  `notes/actor-core-provenance.md` records those fields as deliberately
  unnamed -- existence proved, meaning not.
- The six shard functions need no exceptions: they returned to their
  original member-definition sources and keep their natural shard-path
  attribution.

## Verification

- `tools/tubuild.py verify arm9/Actor`: 91/91 MATCH, objisolate clean,
  reloc-destinations clean, emission order ROM-ascending,
  `_ZN7Vector3D1Ev` deadstrips exactly.
- `tools/romdata_check.py`: `_ZTV8dActor_c` verified (124 bytes),
  `_ZTI8dActor_c` verified, 4 verified / 3 partial / 0 differs across the
  six enrolled sources' 7 emitted data symbols.
- `check_decl_agreement --changed origin/main`: no new disagreements
  (`data_0209b468` conformed to the shards' plurality `extern void*`
  spelling; callers pass `&name`, byte-exact).
- `tiers_ratchet --check`: PASS.
- `check_src_tu_compiles`: 347/347.
- `queue_audit --check-promoted`, `port_refcheck`,
  `check_dead_references`, attribution check: clean.
- `dsd check symbols` reports the same nine pre-existing ARM9/ITCM errors
  as the baseline control (overlay_100/102, data_020ad524/60, ITCM
  0x01ff98f4..0x01ff9e2c); nothing new.
- `tubuild.py linkcheck arm9/Actor` does not apply: the per-TU linkcheck
  routes through `prepare_intact_object`, which requires a non-text
  claim; this manifest is text-only (`production_mode` absent), produced
  through the ordinary `isolate_many` path like every other text-only
  promoted TU. Production byte-identity is proven by `rombuild.py` and
  `prepush_linkcheck.py` instead.

## Known environmental note

This machine's `extracted/` inputs produce whole-ROM sha256 `ddab9300...`
while the admitted intact proofs record `d1506e90...`. The main checkout's
baseline produces the same `ddab9300` with the identical
`moduleSetSha256` (`f9852ffaf80ef196...`) and `romInputsSha256`
(`2d36439e...`) -- so the difference is outside compared module bytes and
predates this change. `rombuild`'s strict intact-TU control needs either
the admitted sha or a same-worker `build/sm64ds.nds` oracle; CI has both.
