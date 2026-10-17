# fold/ov002-floatboard-1009 -- daObjFloatBoard_c tail fold

## Target

ov002 `daObjFloatBoard_c` tail. The class's promoted TU
(`src/game/actors/d_a_obj_float_board.cpp`, manifest `ov002/daObjFloatBoard_c`)
licensed 0x020b5ab4..0x020b5e58; the next three functions in the overlay
were scattered shards owned by the same class:

- `0x020b5e58` -- the shared Init helper all three leaf `InitResources`
  (`daObjKi_Ita_c` ov016, `daObjWcObj01_c`/`daObjWcObj06_c` ov029) call on
  `this`. The body writes only this class's named fields, calls member
  `func_ov002_020b5b98`, loads model + KCL and registers the mesh
  callbacks.
- `0x020b5f9c` -- the mesh-collision callback: if the touching actor's
  `actorID` is 0xbf (Player) it is stored in `mRider` and `mRiderTimeout`
  reloads to 5.
- `0x020b5fc4` -- the registration thunk `0x020b5e58` stores into the mesh
  collider's callback slot. The callback ABI hands it an extra leading
  argument; it discards it and forwards (board, actor) to the member
  callback.

The run is bounded below by `_ZN15daObjGuragura_cD0Ev` at 0x020b5fd8, so
the licensed span is now 0x020b5ab4..0x020b5fd8.

## What landed

- `src/game/actors/d_a_obj_float_board.cpp`: the three shards folded in as
  `daObjFloatBoard_c::func_ov002_020b5e58(daObjFloatBoard_c_Resources *)`,
  `daObjFloatBoard_c::func_ov002_020b5f9c(dActor_c *)`, and free
  `extern "C" func_ov002_020b5fc4` (its address is stored as a raw
  callback word, so it cannot be a member). Defined at the top of the
  file -- mwccarm emits this TU's .text in reverse source order, so the
  new functions lead the file for the object's sections to come out in
  ROM-ascending order.
- `include/daObjFloatBoard_c.h`: the two member declarations. Address
  names kept -- nothing in the ROM names either helper.
- The three leaf `InitResources`: local `ResourceDescriptor` copies and
  the `extern "C" func_ov002_020b5e58` decls deleted; the call is now the
  inherited member on `this` and the table externs spell the shared
  `daObjFloatBoard_c_Resources` type.
- `include/decl_common.h`: the `func_ov002_020b5e58` junk-drawer row
  removed (symbol renamed to the member mangle); the three leaf file
  tables' rows re-typed `struct daObjFloatBoard_c_Resources` (the same
  `struct T` convention as `extern struct Vector3 data_ov...`).
- `config/arm9/overlays/ov002/symbols.txt`: e58/f9c renamed to the member
  mangles; fc4 keeps its C name.
- `config/arm9/overlays/ov002/delinks.txt`: the three shard blocks
  consolidated into the TU block; span widened to 0x020b5fd8.
- `config/tu_manifest.d/ov002/daObjFloatBoard_c.json`: three function
  entries (ordinals 5-7, `legacy_source` set to the deleted shard paths),
  span widened, boundary evidence and notes updated.
- `attribution.json`: `path#symbol` keys added for the three folded
  functions (tangosdev, matching the shard credit).
- `config/decl-agreement-baseline.json`: one row added under
  `_ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block`
  -- the same `param:#3 const Matrix4x3 &` / `param:#6 CLPS_Block &`
  mangle-accurate spellings every sibling decl already banks against the
  definition file's loose `*`/`void*` spelling. The decl is byte-proved
  (the TU verifies 8/8 with it).

## Verification

- `tools/tubuild.py verify ov002/daObjFloatBoard_c`: 8/8 MATCH,
  objisolate clean, reloc-destinations clean, emission order
  ROM-ascending; `_ZN17daObjFloatBoard_cD0Ev`/`D1Ev`/`_ZN7Vector3D1Ev`
  deadstrip exactly.
- Leaf TUs re-verified after the call-site change:
  `ov016/daObjKi_Ita_c` 4/4, `ov029/daObjWcObj01_c` 4/4,
  `ov029/daObjWcObj06_c` 4/4 -- all objisolate clean, emission order
  correct.
- `check_decl_agreement --changed origin/main`: clean (one banked row,
  above).
- `port_refcheck`, `check_dead_references`, `queue_audit`: clean.
- `tiers_ratchet --check`: PASS.
- `check_src_tu_compiles`: 347/347.

## Leftovers (in file banners)

- `func_ov002_020b5f9c`: the `enum Bool` staging on the `actorID == 0xbf`
  test is load-bearing -- the plain `if` folds to a 0x1c body; the ROM's
  0x28 keeps the widened bool.
- `func_ov002_020b5fc4` stays `extern "C"`: the Init helper stores its
  address as a raw callback word.
- `dBgW_KcMbg::SetFile` stays a mangled free call -- the
  `Fix12<int>`-by-value wall, same as the file's two existing externs.
- Destructor pair D0/D1 stays shard-enrolled (vtable/RTTI ownership --
  the manifest's destructor note carries the full measurement).
