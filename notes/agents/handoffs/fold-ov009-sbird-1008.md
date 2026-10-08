# fold/ov009-sbird-1008 -- daSBird_c head run + PMF member conversion

## Target

ov009 `daSBird_c` head run. The class linker unit is 0x021111a0..0x02111a70,
split by the `func_ov009_0211145c` hatch (proven mwccarm 1.2/2004/b56 register-
allocation wall; stays `src/unnamed/ov009/func_ov009_0211145c.c`, unenrolled, not in delinks).
The upper run was already promoted (`src/game/actors/d_a_s_bird.cpp`,
manifest `ov009/daSBird_c`, 0x021115d8..0x02111a70). This change folds the
two scattered shards below the hatch and finishes the class's member
conversion.

## What landed

- New `src/game/actors/d_a_s_bird_head.cpp` (manifest `ov009/daSBird_c_head`,
  .text 0x02111224..0x0211145c, 2 functions, TEXT-VERIFIED 2/2):
  `daSBird_c::func_ov009_02111224(int)` (the follower-attach the spawn loops
  call on the child) and `daSBird_c::func_ov009_02111234()` (PMF ordinal 3,
  the fly/steer state). Both fully memberized onto the existing
  `daSBird_c.h` field names -- no raw-offset bodies needed.
- `d_a_s_bird.cpp`: `func_ov009_021116ec` / `func_ov009_021115d8` converted
  from `extern "C"` helpers to `daSBird_c` members (PMF ordinals 0/1 of the
  same `data_ov009_02113c48` table), fully memberized; 8/8 TEXT-VERIFIED.
  The player's position stays an opaque `p2 + 0x5c` read -- `Player` is
  incomplete in this TU and pulling the header in is a different campaign.
- `include/daSBird_c.h`: four member declarations under the virtuals.
  Helper names are not recovered, so members keep their addresses (S33):
  `_ZN9daSBird_c19func_ov009_0211xxxx*`.
- `src/unnamed/ov009/func_ov009_0211145c.c` (the hatch): the `func_ov009_02111224` extern
  respelled to the mangled member name -- it is .c and cannot member-call.
- `delinks.txt`: the two shard blocks consolidated into one
  `d_a_s_bird_head.cpp` block. D1/D0 shards untouched -- they emit the
  class vtable/RTTI and must not be synthesised into a class TU.
- `symbols.txt`: four rows renamed to the member mangles.
- `attribution.json`: shard-path keys moved to `path#symbol` on the new file
  (tangosdev); the two converted helpers' keys respelled to the member
  mangles (021115d8 stays lunavyqo, 021116ec tangosdev).
- `config/decl-agreement-baseline.json`: one stale row removed
  (the deleted 02111234 shard's `FindWithID` `void*` decl).
- `config/tu_manifest.d/ov009/daSBird_c.json`: boundary/notes updated for
  the head fold; function symbols respelled to member mangles.

## Verification

- `tools/tubuild.py verify ov009/daSBird_c_head`: 2/2 MATCH, objisolate
  clean, reloc-destinations clean, `_ZN7Vector3D1Ev` deadstrips exactly.
- `tools/tubuild.py verify ov009/daSBird_c`: 8/8 MATCH, all criteria PASS.
- `tubuild linkcheck` does not apply to these text-only promoted TUs; the
  link proof is rombuild + `prepush_linkcheck --range`.

## Leftovers (in file banners)

- `func_ov009_0211145c` hatch between the head file and the promoted run:
  permanent NONMATCHING, unenrolled.
- D1/D0 stay their own enrolled files (vtable/RTTI ownership).
- `d_a_s_bird.cpp` keeps its existing deslop leftovers (6az Fix12-by-value
  externs, opaque player read, `&mPosX` Vector3 pun).
- `d_a_s_bird_head.cpp`: atan2/Vec3 externs (same walls as the upper half),
  `*(u16 *)&mPrevAngleX` sign pun for the sine-table index.
