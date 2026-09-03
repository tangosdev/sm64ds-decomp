# Cross-reference: the N64 SM64 decomp as an oracle for this repo

**Status:** investigation notes, incomplete. Parked 2026-08-06 for later revisit.
**Reference repo:** `n64decomp/sm64` (hand-written, byte-matched, years of human work — treat as ground truth).
**Method:** four parallel agents. Findings below were produced by disassembling
`extracted/overlays/overlay_0002.bin`, `extracted/dsd/arm9/arm9.bin`, and ITCM with capstone,
and by reading `config/arm9/**/symbols.txt` and `relocs.txt` — **not** by reading this repo's
own `.c`/`.cpp`, which is largely AI-authored and unverified.

---

## 0. Why the trust model matters here

The N64 decomp is in every model's training data. If a DS source file *looks like* N64 source,
the null hypothesis is **AI projection**, not Nintendo reusing engine code. So naive similarity is
evidence of contamination, not of reuse.

Every claim below is graded:

- **T1** — read from the ROM: instruction immediates, rodata words, relocation targets, or
  mangled symbol names the binary itself spells.
- **T2** — exists only in AI-authored source here. Proves nothing on its own.

Provenance coverage when this was written: `config/match_provenance.jsonl` had 301 records for
~11,089 `src/` files (288 `kind:"ai"`, 12 `kind:"human"`). **Absence of a record means unknown.**

Note the distinction that caused an error mid-investigation: provenance measures *attribution*,
not *verification*. The real verification gate is the `complete` delink flag plus the full-ROM
rebuild — roughly 10,669 of 11,252 files were source-built and link-verified at the time. Those
are byte-proven. What a byte gate cannot prove is that the *source* is real decompilation; see §5.

---

## 1. Headline result

**The tuning numbers were carried over. The code was not.**

SM64DS reads as a from-scratch reimplementation that deliberately preserved SM64's tuning table.
Every architectural subsystem checked came back divergent; a large fraction of the physics
constants came back exact.

---

## 2. T1 evidence of shared tuning data

### 2.1 Jump velocity table — the cleanest artifact

`data_ov002_0210a59c`, file offset `0x5CF3C` in `overlay_0002.bin`, raw words:

```text
0x5cf3c: 0002a000  ->  42.0
0x5cf40: 00034000  ->  52.0
0x5cf44: 00045000  ->  69.0
```

N64 `src/game/mario.c:824 / :786 / :797` — `ACT_JUMP` 42.0f, `ACT_DOUBLE_JUMP` 52.0f,
`ACT_TRIPLE_JUMP` 69.0f. Indexed by `mJumpComboStage`
(`ldrb r2,[r4,#0x6e1]; ldr r1,[r1,r2,lsl #2]` @ 0x020e23fc).

This is a data table in the ROM image. It cannot be a hallucination.

### 2.2 Gravity / terminal velocity

`Player::ChangeState` (0x020e30a0) resets the pair on every transition:

```arm
020e31c0  mov r2, #0x4000     ; 4.0
020e31cc  mov r3, #0x4b000    ; 75.0
020e3208  rsb r4, r2, #0      ; -4.0
020e320c  rsb r3, r3, #0      ; -75.0
020e3234  str r4, [r5, #0x9c] ; mVertAccel        = -4.0
020e3238  str r3, [r5, #0xa0] ; mTerminalVelocity = -75.0
```

N64 `src/game/mario_step.c:576-578` — `vel[1] -= 4.0f`, clamp `-75.0f`. `-75.0` is arbitrary
tuning, not a natural constant. Re-emitted at 8 other Player sites.

### 2.3 Wing-cap flutter — same gate, same numbers

DS `St_Shell_Main` 0x020cc5cc-0x020cc5ec: `mHasWings` AND descending AND A-held →
`mVertAccel = -2.0`, `mTerminalVelocity = -37.5`.

N64 `mario_step.c:566-574` — `MARIO_WING_CAP && vel[1] < 0.0f && (input & INPUT_A_DOWN)`
→ `-2.0f`, clamp `-37.5f`. Same three-way gate, same two non-round constants.

### 2.4 Other exact hits (fx12, ÷0x1000)

| DS site | Value | N64 |
|---|---|---|
| `St_LongJump_Init` 0x020e1314 | 30.0 | `mario.c:865` |
| `St_LongJump_Init` 0x020e1324 | -2.0 accel | `mario_step.c:545` |
| `St_LongJump_Init` 0x020e1334 | ×1.5 | `mario.c:870` |
| `St_BackFlip_Init` 0x020e1940 | 62.0 | `mario.c:793` |
| `St_BackFlip_Init` 0x020e1960/74 | 16.0 + yaw ^0x8000 | `mario.c:792` (`-16.0f`) |
| `St_SideFlip_Init` 0x020e1a90/9c | 8.0, 62.0 | `mario.c:838-839` |
| `St_WaterJump_Init` 0x020e166c | 42.0 | `mario.c:808` |
| `St_Dive_Init` 0x020dd1ec | +15.0 | `mario.c:857` |
| `St_Jump_Init` 0x020e2524 | ×0.7998 (0xccc) | `mario.c:787/798/825` (`*= 0.8f`) |
| `St_Jump_Init` 0x020e23a8 | 20.0 triple-jump gate | `mario.c:1051` |
| air helper 0x020e2a8c-0x020e2aac | backwards drag 16.0 / 2.0 | `mario_actions_airborne.c:207` |

Retuned rather than copied: speed caps (DS 60.0/100.0/40.0 vs N64 48.0), slide kick,
Mario's collider (DS 50/50/40 vs N64 30/24, hitbox 37/160).

### 2.5 Arctangent table — byte-identical

N64 `include/trig_tables.inc.c:1296` `s16 gArctanTable[0x401]` vs DS table at **0x020994e0**:
**1025/1025 entries identical, zero mismatches.** Same 8-way octant fold with the same
`0x4000`/`0x8000`/`0xC000` constants (`cstd::atan2` @ 0x0203b4dc vs N64 `math_util.c:713-750`).

Honest caveat: all 1025 N64 entries also match `round(atan(i/1024) · 0x10000/2π)` exactly —
no typo, no off-by-one, no hand-tweak. The table is fully determined by three design choices,
so this is *shared Nintendo house recipe, re-derivable*, not provable copying.

### 2.6 World unit scale

Actor collider radii in the ROM: 60.0, 150.0, 100.0, 80.0, 50.0, 40.0, 30.0, 20.0, 10.0.
60 and 150 are exactly N64's two `mario_step.c` wall radii. Consistent with
**1 DS fx12 unit == 1 N64 unit**, which makes N64's documented magic numbers a legitimate
cross-check. Low confidence on its own (round numbers), but useful.

---

## 3. Architecture: divergent everywhere checked (T1)

| System | N64 | SM64DS |
|---|---|---|
| Action state | `u32` ID packing group + 22 `ACT_FLAG_*` bits, `switch (action & ACT_GROUP_MASK)` | `State*` at `Player+0x370`/`+0x374`/`+0x378`, Metrowerks pointer-to-member dispatch; 0x18-byte `{Init, Main, Cleanup}` descriptors in ov002 **.bss** (runtime-constructed, not statically readable); flags in a separate `u16` at `+0x6ce` |
| Collision broadphase | 16×16 XZ grid, ±0x2000, pre-sorted floor/ceil/wall lists | **KCL octree** (Mario Kart DS lineage), 1 bit/axis descent, one triangle list per leaf, classified at query time |
| Floor threshold | `normal.y > 0.01` | `normal.y > 0` (`cmp r0,#0; ble` @ 0x01ffd5cc) |
| Point-in-triangle | 3× 2D XZ cross products, no tolerance | KCL 3D edge-plane dots + prism `length`, ±0x20000 tolerance |
| Surface types | `SURFACE_*` 0x0000–0x00FD | 8-byte CLPS bitfield, **5-bit** type field (bits 19-23) — physically cannot hold N64's ID space |
| Warps / camera triggers | static per-level C structs, `WARP_NODE_*` 0xF0–0xFA | instanced `Actor` objects with collision cylinders (`daWarpkun_c_Spawn` = 264-byte actor, `daCamTag_c`) |
| Matrices | 4×4 f32 | 4×3 Fix12 (NitroSDK G3) |
| sqrt / divide / sin / cos | software tables | DS hardware DIV/SQRT MMIO, BIOS `swi 0x0d`, `FX_SinCosTable_` |

`find_floor` equivalent is `MeshCollider::DetectClsn(RaycastGround&)` @ **0x01ffd3f8** (ITCM, 0x498 bytes).

---

## 4. Contamination audit — came back clean

Repo-wide grep over `src/` + `include/`, **zero hits** for all of:

```sh
SURFACE_  TERRAIN  CAMERA_MODE_  LEVEL_  COURSE_  WARP_NODE_
find_floor  findFloor  find_wall  gMarioState  marioObj
atan2s  sins(  coss(  mtxf_  vec3f_  approach_s32
```

No AI invented an N64-shaped enum anywhere in the camera / level / save / collision / math scope.
Naming in those areas is ROM-derived (`BgCh`, `CLPS`, `KCL_File`, `SurfaceInfo`, `MeshCollider`,
`SphereClsn`, `RaycastGround`, `Matrix4x3`) — correct provenance. These subsystems are simply
**under-decompiled**, which is itself a clean, useful negative result.

**Unverified claim, flagged not confirmed:** one agent reported that `notes/actor-naming.md`
instructs contributors to open a clone of `n64decomp/sm64` while drafting matches. If true that is
a documented contamination vector worth a policy note. *This was not verified — check it.*

---

## 5. Defects found along the way (actionable)

1. **`src/_ZN6Player16St_WallJump_InitEv.cpp` is misattributed.** Its own banner admits it is
   *"NOT a Player method"*; it dereferences `this+0x4eb0`, `+0x4eb4`, `+0x4ee5` — ~0x4700 bytes
   past the end of a 0x768-byte `Player`. The real `St_WallJump_Main` is in ov002; this one is in
   ov006. **The bad assumption has already leaked into `include/Player.h:536-540`** as a footnote
   about "offsets far outside the object." Anything trusting that note is building on a bad symbol.

2. **Binary angles being read as fx12.** `0x4000` is 90°, `0x8000` is 180° — *not* 4.0 and 8.0.
   Live mis-annotation risk at 0x020e2560, 0x020e2a8c, 0x020cb9a4 and ~35 other sites. Any tool or
   agent that pattern-matches N64 float constants against DS immediates will get these wrong.

3. **The `0x14` false lead.** `BgCh::ShouldPassThroughImpl` (0x02039488) really does contain
   `cmp r0, #0x14` @ 0x020395a0, and N64 `SURFACE_SLIPPERY` really is 0x14. **Coincidence.** The DS
   comparison gates pass-through, not slipperiness; N64's pass-through analogue is
   `SURFACE_VANISH_CAP_WALLS = 0x7B`, unrepresentable in 5 bits. With a 5-bit field and ~9 observed
   values, collision with some low N64 ID is near-certain.

4. **`src/AngleDiff.c`** has no header comment, no `@symbol` line, no matching claim — unlike every
   neighbour in the math set. Lowest-confidence file in that group.

5. **`src/_ZN6Player11ChangeStateERNS_5StateE.cpp`** declares a local `struct State` and a
   `struct C3;` inside the .cpp, and writes fields through `*(State**)((char*)&self->unk_378)`
   casts. The ROM confirms the 0x18/3-PMF layout, so the *shape* is right — but this is an
   un-recovered type re-declared per-file instead of living in a header.

---

## 6. Oracle list — free, T1-backed symbol names

| Address | Name | Basis |
|---|---|---|
| `data_020994e0` | `gArctanTable` — 1025 `s16` (the +1 guard entry matters), 0x802 bytes | byte-identical to N64 |
| `data_02082214` | `FX_SinCosTable_` — 0x1000 interleaved fx16 sin/cos pairs, 0x4000 bytes, ends 0x02086214 | NitroSDK standard |
| `data_ov002_0210a59c` | `sJumpVelocities` / `sPlayerJumpComboVelY` | rodata {42.0, 52.0, 69.0} |
| `data_ov002_0210a5a8` | jump-combo animation IDs | rodata {0x53, 0x4f, 0x4b} |
| `func_ov002_020e28d4` | `Player::UpdateAirMovement` — cf. N64 `update_air_with_turn` (`mario_actions_airborne.c:186`) | long-jump drag special case + 16.0/2.0 backwards drag + `ApproachLinear` on `mHorzSpeed` |
| `func_ov002_020bf2d8` | `Player_SetVertSpeedByCharFactor` (sibling of `Player_ScaleByCharFactor` @0x020bf30c) | scales arg by per-char factor → `+0xa8` |
| `func_02037e38` | `CLPS::GetType` | the only CLPS getter used in a jump-table dispatch (ov098 @0x02139264) |
| 0x02037e14–0x02037e90 | eight more CLPS bitfield getters | 3-instruction accessors, bit extents recovered |

Camera `State` descriptors (ov002, `data_ov002_0211xxxx`, 41 unnamed), disambiguated by
`ChangeState` caller analysis:

| Address | Role |
|---|---|
| `0x02110424` | Land — from Fall/Jump/LongJump/SideFlip/WallJump/Spin `_Main` |
| `0x0211055c` | LongJump — from `St_Land_Main`, and target of the raised-drag `IsState` check |
| `0x02110214` | Fly / wing-jump — from `St_Jump_Init` when `mHasWings && stage==2` |
| `0x0211022c` | NoControl — from `SetNoControlState` |
| `0x02110364` | HoldLight — substituted for `0x0211013c` when `mHeldObj != 0` |
| `0x02110154` | Wait / Stand — from `St_Walk_Main` on stop |

Naming these and putting the 0x18-byte `{Init, Main, Cleanup}` PMF layout in a shared header would
let `ChangeState` and every `St_*_Main` drop their per-file local `struct State` re-declarations.

---

## 7. Correction on record

An earlier pass claimed the camera behaviour table at `0x0209b008` had **exactly 15** entries at
0x10 stride, "matching N64's 15 populated camera modes." **This is wrong.** The relocs continue:

```text
0x0209b000 0x0209b004 0x0209b008 0x0209b018 ... 0x0209b0e8 0x0209b0f8
0x0209b108 0x0209b118 0x0209b128 0x0209b138
```

The table extends well past 0x0209b0e8, so the slot-count coincidence evaporates. What survives is
the useful part: **there is a real table of 0x10-byte camera `State` structs based at 0x0209b008**,
corroborated by literal-pool targets inside the byte-matched `_ZN6Camera14GoBehindPlayerEj`.
Do not cite the N64 parallel.

---

## 8. Actor architecture and the naming oracle

### 8.1 Architecture — divergent to the bone (T1, vtable-backed)

This is the best-evidenced scope of the investigation, because **vtables are real binary
structures**: a wrong class name breaks the relocation link, so it cannot be hallucinated.

`notes/actor-vtables.md` records `_ZTV9ActorBase` (18 slots), `_ZTV12ActorDerived` (18),
`_ZTV5Actor` (31), `_ZTV6Player` (31, ov002) recovered byte-for-byte from `symbols.txt`/`relocs.txt`
— every slot resolves to a named function, none is a thunk. Hierarchy is
`ActorBase → ActorDerived → Actor → Player` (`Actor` is *not* a direct child of `ActorBase`,
confirmed from the double-vptr-store in `_ZN5ActorC2Ev`).

| | N64 | SM64DS |
|---|---|---|
| Object representation | one `struct Object` for every actor; past `0x088` it degenerates into a raw `union rawData[0x50]` reinterpreted by opcode (`include/types.h:154-171`, `object_fields.h`) | class-specific C++ types with named, typed members; per-class heap allocation via `_ZN9ActorBasenwEj` (BobOmb 1024 bytes, Bookend actor 1108) |
| Behavior | **interpreted bytecode VM** — `object->behavior` points into `data/behavior_data.c`; `bhvCmdProc = BehaviorCmdTable[*gCurBhvCommand >> 24]` (`behavior_script.c:937`) | **virtual dispatch** — vtable slot 6 `virtual s32 Behavior()`, a real compiled function per class (`_ZN6Goomba8BehaviorEv` size 0x544). **No interpreter, no opcode table, no `CALL_NATIVE` equivalent anywhere.** |
| Allocation | fixed `gObjectPool[240]`, all slots identical size | per-class heap sizes |
| Identity | **raw C pointer** (`create_object(const BehaviorScript*)`) — N64 has *no numeric ID system at all* | dense `ActorIDs` table, 391 entries, indexing `ACTOR_SPAWN_TABLE` @ `data_02090864` (T1: `symbols.txt:4138`, relocs `:2810,12558`) |
| Update order | 10 fixed named buckets, `sObjectListUpdateOrder[]` (`object_list_processor.c:172-181`) | continuous per-type `s16 behavPriority` + `s16 renderPriority` in `SpawnInfo`, threaded through two *separate* intrusive lists (`behavNode`/`renderNode`, 0x10 bytes each, at `ActorBase+0x28`/`+0x38`) — DS splits behavior and render ordering; N64 does not |

**The bytecode-VM concept did not survive the port.** And because N64 identifies objects by pointer
while DS *must* use a numeric table (overlay-relocated code may not be resident when level
placement data is read), there is no shared axis on which to compare numbering. DS's `ActorIDs`
clusters related enemies (Goomba 200-202, Koopa 203-205, Bob-omb 206-208, Boo 209-214, Bully
215-218); N64's `behavior_data.c` definition order shows no such clustering (Bully at line 4050
sits between Amp at 3852 and Koopa at 5079, nowhere near Boo at 2984). **Convergent at best.**

### 8.2 Per-actor constants

Only Goomba and BobOmb `InitResources` are confirmed byte-identical in `progress/matched.jsonl`;
everything else below is explicitly unverified.

**Goomba** (`ov084:0x0212bc30`, size 0x390, verified; tables read from raw `ov084.bin` bytes):

| | N64 (`goomba.inc.c`) | DS tiny / regular / huge |
|---|---|---|
| Visual scale | 0.5 / 1.5 / 3.5 | 0.333 / 1.0 / 2.333 |
| Hitbox radius | **72 fixed, all sizes** (`:17`) | 20.0 / 60.0 / 140.0 (scales!) |
| Hitbox height | **50 fixed, all sizes** (`:18`) | 40.0 / 100.0 / 224.0 (scales!) |
| Gravity | -1.333 / **-4.0** / -9.333 (`:119`) | -0.667 / **-2.0** / -4.667 |

The size-scaling **ratios** match exactly (0.333 : 1 : 2.333 on both sides, for both scale and
gravity) — data reuse at the formula level. But N64 uses one fixed 72×50 hitbox for every Goomba
size (a known quirk: a huge Goomba looks 2.3× bigger but hits like a regular one) while **DS scales
the hitbox with size**. A real behavioral difference — DS fixed what reads as an N64 oversight.

**BobOmb** (`ov102:0x0214c510`, size 0x1d4, verified): DS radius 60.0 / height 80.0 vs N64 65 / 113
(`bobomb.inc.c:3-13`). Close on radius, ~29% off on height. Convergent, not shared data.

**daWanwan_c** — DS side **[T2, unmatched]**: claimed 150.0 / 300.0 vs N64 80 / 160
(`chain_chomp.inc.c:15-25`). That is a suspiciously uniform 1.875× on both axes; worth re-checking
against a confirmed match before anyone relies on it.

**Whomp** — structural convergence: both games special-case Whomp with **mesh** collision rather
than a cylinder (N64 `LOAD_COLLISION_DATA(whomp_seg6_collision_...)`; DS `MeshCollider::LoadFile` /
`MovingMeshCollider::SetFile`), and both encode regular-vs-King as **one class with a flag**, not
two behaviors (N64 `oBhvParams2ndByte != WHOMP_BP_SMALL`; DS `mIsKing`).

**Bully** — not recoverable; DS logic is behind unmatched `func_ov064_02116ec0`. N64 ground truth
for future comparison (`bully.inc.c:3-52`): small 73/123/63/113, gravity 4.0, friction 0.91;
big 115/235/105/225, gravity 5.0, friction 0.93.

### 8.3 Naming oracle — N64 behavior symbol → DS class

All DS names below confirmed as live mangled symbols (`_ZTV*`, `_ZN*D1Ev`) in
`config/arm9/**/symbols.txt`. These are linker-level facts about what Nintendo's compiler emitted.

| N64 behavior | → | DS class | overlay |
|---|---|---|---|
| `bhvGoomba` | → | `Goomba` | ov084 |
| `bhvBobomb` / `bhvKingBobomb` / `bhvBobombBuddy` | → | `BobOmb` / `KingBobOmb` / `BobOmbBuddy` | ov102 / ov078 / ov084 |
| `bhvChainChomp` | → | `daWanwan_c` (+`daObjWanwanShutter_c`) | ov014 |
| `bhvSmallBully` / `bhvBigBully` | → | `Bully` / `BigBully` | ov064 |
| `bhvSmallWhomp` (+King) | → | `Whomp` (`mIsKing` flag) | ov079 |
| `bhvThwomp` | → | `Thwomp` | ov091 |
| `bhvBoo` | → | `Boo` / `BigBoo` | ov063 |
| `bhvMrI` | → | `MrI` (+`MrI_Projectile`) | ov071 |
| **`bhvFlyingBookend`** | → | **`BookShot`** — *not* "Bookend" | ov020 |
| `bhvHomingAmp` / `bhvCirclingAmp` | → | `Amp` | ov070 |
| `bhvKoopa` | → | `Koopa` (+`KoopaTheQuick`, `daRFlag_c`, `KoopaShell`) | ov062 |
| `bhvKlepto` / `bhvChuckya` | → | `Klepto` / `Chuckya` | ov062 |
| `bhvHeaveHo` | → | `HeaveHo` | ov077 |
| `bhvFlyGuy` | → | `FlyGuy` | ov070 |
| `bhvSnufit` / `bhvSwoop` | → | `Snufit` / `Swoop` | ov065 |
| `bhvScuttlebug` | → | `Scuttlebug` | ov071 |
| `bhvSpindrift` | → | `Spindrift` | ov081 |
| `bhvWigglerHead`/`Body` | → | `Wiggler` | ov034 |
| `bhvPiranhaPlant` | → | `PiranhaPlant` (+`FirePiranhaPlantBig/Small`) | ov084 |
| `bhvEnemyLakitu` / camera Lakitu | → | `Lakitu` / `LakituBro` | ov077 / ov085 |
| `bhvBowser` | → | `Bowser` (+`BowserTail`, `BowserFire`) | ov060 |
| `bhvUkikiCage` | → | `daObjHmBskt_c` — **but see trap below** | ov030 |
| (Eyerok boss) | → | `Eyerok` | ov066 |
| — | | `CheepCheep` ov090, `Skeeter` ov090 | |

**Two traps in this table:**

1. **`Bookend` → `BookShot`.** The community actor-ID label and the ROM-embedded mangled class name
   disagree. `src/Bookend_Spawn.c` has `VT0=_ZTV8BookShot`. Trust the mangled name.
2. **`UkikiThief` is not a class.** `UkikiThief_Spawn` (`ov030/symbols.txt:60`) instantiates
   `_ZTV13RollingLogTtm` — identical-code folding. `notes/actor-naming.md:31-33` already warns
   that shared functions are "claimed by the lowest actor id." Re-derive its real behavior; do not
   take `RollingLogTtm` as its semantic identity.

Combined with N64's field vocabulary (`oPosX`/`oAction`/…) and the hitbox constants in
`sm64/src/game/behaviors/*.inc.c`, this is directly usable when drafting DS `Behavior()` /
`InitResources()` or renaming `func_ov###_########` placeholders for these actors.

DS-only / EAD additions, not chased: `KoopaTheQuick`, `daIDonketu_c` (then called `ChillBully` here)/`ChiefChilly` (Snowman's Land
reskins — N64 doesn't reuse Bully there), `Toad`, `Dorrie`.

---

## 9. Ground physics — settled (follow-up investigation)

Two agents disassembled the ground-movement chain. Result: **DS kept N64's terrain/surface
physics almost verbatim and replaced N64's acceleration curve.** Both halves are well-evidenced.

### 9.1 The slipperiness class system — STRUCTURAL REUSE, T1

N64 derives a 4-value floor class from surface ID + area terrain type (`mario_get_floor_class`).
**DS stores the class directly in the collision data**: CLPS **bits 12-14** (getter `func_02037e58`,
3 bits) *is* the slipperiness class. It is **not** a terrain/material type — the 3-bit/mask-7
resemblance to N64's `TERRAIN_MASK 0x0007` is a coincidence of arity. DS artists author
slipperiness per-triangle instead of deriving it.

`Player::SetFloorSurfaceInfo` @ `0x020c16ec` unpacks CLPS into the Player; the class lands at
`+0x658`. `Player::GetFloorClass` @ `0x020c031c` then applies **N64's crawl override, exactly**:

```c
if (IsState(0x02110514 /* St_Crawl */) && floorNormalY > 0x800 /* 0.5f */ && class == 0) class = 2;
```

N64 `src/game/mario.c:427` — same three conditions, same `0.5f`, **including the redundant
`== SURFACE_CLASS_DEFAULT` guard**. A redundant guard reproduced identically is a fingerprint.

DS has 6 class values to N64's 4:

| DS class | slope threshold | slope accel | slide accel | slide loss | slope decel | N64 |
|---|---|---|---|---|---|---|
| 0 / default | cos15° `0xF74` | 1.7 | 7.0 | 0.92 | 1.0 | `CLASS_DEFAULT` |
| 1 | cos20° `0xF08` | 0.0 | 5.0 | 0.92 | 1.0 | `CLASS_NOT_SLIPPERY` (+ DS latch role) |
| 2 | cos20° `0xF08` | 0.0 | 5.0 | 0.92 | 1.0 | `CLASS_NOT_SLIPPERY` (crawl-derived) |
| 3 | cos10° `0xFC1` | 2.7 | 8.0 | 0.96 | 0.35 | `CLASS_SLIPPERY` |
| 4 | cos5° `0xFF0` | 5.3 | 10.0 | 0.98 | 0.1 | `CLASS_VERY_SLIPPERY` |
| 5 | cos1° `0xFFF` | 5.3 | 10.0 | 0.98 | 0.1 | **`TERRAIN_SLIDE`, promoted to a class** |

Class 5 is not a DS invention. N64's slide rule is `normal.y < 0.9998477f` → force VERY_SLIPPERY;
`0.9998477 × 4096 = 4095.4` → truncates to **4095 = 0xFFF**, and the accel is 5.3 on both sides.
DS turned N64's special case in front of the switch into a first-class enum value.

### 9.2 The three class tables (ov002, adjacent leaf functions)

| Function | Role | Values | vs N64 |
|---|---|---|---|
| `0x020f035c` | `FloorIsSlope(class, normalY)` | cos 1/5/10/15/20° | **all five exact**, same `<=` predicate |
| `0x020f02c8` | `GetSlideAccelForClass` | 10.0 / 8.0 / 7.0 / 5.0 | **all four exact** (`update_sliding`) |
| `0x020f030c` | `GetSlideLossFactorForClass` | 0.98 / 0.96 / 0.92 | **all three exact** |

Verified independently: loss-factor pool is three consecutive words at `0x20f0350`/`0x54`/`0x58`
= `0xFAE`/`0xF5C`/`0xEB8`, and **`0xEB8` (0.92) occurs exactly once in the entire ROM** —
all 103 overlays plus arm9. Slope accel pool at `0x20c067c`/`0x80`/`0x84` = `0x54CC`/`0x2B33`/`0x1B33`.

### 9.3 Slope accel and sliding

`Player::ApplySlopeAccel` @ `0x020c04e0` is `apply_slope_accel` line for line — steepness via
`Vec3_HorzLen(floorNormal)`, `AngleDiff(floorAngle, moveYaw)`, the slope gate, the 4-way switch,
the `±0x4000` sign flip. One DS addition: accel is scaled by `clamp(forwardVel − 19.0, 1.0, 1.5)`,
a no-op at speeds ≤ 20.

`Player::UpdateSliding` @ `0x020c06fc` fuses N64's `update_sliding` + `update_sliding_angle`.
Exact carries: the `0x52` lossFactor term (= N64's `0.02f`), the `100.0` forward-vel cap, the
`!FloorIsSlope && |vel| < stopSpeed → vel = 0` stop test. Same caller set as N64
(butt slide, stomach slide, slide kick, crouch, thrown, land).

**Two N64 quirks were deliberately removed:**
1. **The 10k-glitch guard is gone.** No `if (forward < 0.0f && forwardVel >= 0.0f) forward *= 0.5f + 0.5f*forwardVel/100.0f`. DS uses `forward` raw.
2. **The trig-derivative rotation hack is gone** — N64's asymmetric `slideVelX/Z` cross-update plus
   renormalize is replaced by a direct angular delta at `+0x69c` (`sideward/16`, or bucketed by
   stick magnitude up to ≈4.2°/frame). N64's effective ≈2.86°/frame sits inside DS's range.

### 9.4 Deceleration — the table is N64's, halved

`Player::GetSlopeDecelCoef` @ `0x020bf56c` returns `baseDecel × factor[class]`:

| class | DS | N64 | |
|---|---|---|---|
| VERY_SLIPPERY | 0.1 | 0.2 | |
| SLIPPERY | 0.35 | 0.7 | |
| default | 1.0 | 2.0 | |
| NOT_SLIPPERY | 1.0 | 3.0 | retuned |

DS halved the table so `default == 1.0` and the coefficient argument becomes the literal decel
rate. **The 1 : 3.5 : 10 ratio is preserved exactly.** A rescale, not a redesign. Callers pass
`2.0` from `UpdateWalkingSpeed` — the same number N64 passes to `apply_slope_decel`.

### 9.5 What was genuinely replaced

N64's `update_walking_speed` core is **absent**. Verified: `1.1` (`0x119A`/`0x1199`) and `43.0`
(`0x2B000`) have **zero occurrences in all 394 KB of ov002** — no literal, no ARM immediate, no
reciprocal multiply, no magic-number division. Also absent from the walk chain: the `48.0` cap,
the `32.0`/`24.0` targets, the `0.95` `normal.y` gate.

DS substitutes a constant-rate approach toward a banded stick-magnitude target:

```c
walk band (stickMag 0.28–0.58) -> quantized ramp 0 .. 8.0
run band  (stickMag > 0.58)    -> max(40.0 * charFactor * stickMag, 10.0 * charFactor)
integrate: ApproachLinear(forwardVel, target, rate)  // rate 1.0, or 30.0 on the dash boost
                                                     // forced to 1.0 when FloorIsSlope()
clamps: forwardVel >= -10.0, no upper bound
then:   ApplySlopeAccel()
```

N64 reaches 32.0 in ~45 frames on a decaying curve; DS reaches 36.0 in 36 frames on a straight
ramp. DS-only machinery with no N64 counterpart: the 1.2×/rate-30 dash boost, a per-character
speed factor table @ `0x020FF170`, a 30-frame character-1 ramp, a 24-frame tightening turn radius.

Quicksand is the exception — **carried over verbatim**: `if (depth > 10.0) target *= 6.25/depth`
at `0x020d3dcc`–`0x020d3de0`, threshold and factor both exact.

### 9.6 The ov098 table is quicksand depth, not speed

Earlier notes described the ov098 jump table (30/45/60/100 → `+0x5f0`) as a surface-type→speed
table. **It is quicksand sink depth.** ov098 is the Shifting Sand Land object overlay;
`func_ov098_02139228` = `Crate::UpdateQuicksand`. `+0x5f0` is target sink depth, `+0x5f4` current,
approached at 0.5/frame. Render position is built as `pos.y − this[0x5f4]` — verbatim N64
`mario.c:1552` `o->header.gfx.pos[1] -= m->quicksandDepth`.

| tier | N64 max | DS max (CLPS type) |
|---|---|---|
| shallow | 10.0 | 30.0 (type 6) |
| shallow-moving | 25.0 | 45.0 (type 7) |
| normal | **60.0** | **60.0** (type 8) |
| deep → terminal | 160.0 → `ACT_QUICKSAND_DEATH` | 100.0 (type 9) → `Crate_SetState(6)` |

So **CLPS surface-type values 6-9 are the four quicksand types.** Corroborated on the Player side:
`0x020c2ab8` clears the quicksand flag when type is outside `6..9`.

### 9.7 Search traps confirmed the hard way

- **`cos(5°) = 0xFF0` has zero literal-pool hits ROM-wide** — it is `0xFF ror 28`, a valid ARM
  rotated immediate, so it is emitted as a `mov`. Same for slide accels `0x5000`/`0x7000`/`0xA000`/
  `0x8000`. A pool-word-only scan reports "absent" and is wrong. Scan `mov`/`orr` immediates too.
- An earlier pass reported `0x1B33` existed only in `Player::Render` and concluded slope accel was
  absent. **Refuted** — it is at `0x020c0684`, inside `ApplySlopeAccel` (`0x020c04e0`–`0x020c0688`).
  The error was resolving an address to the nearest *grepped* symbol instead of bisecting the
  sorted symbol table.
- Two live fx12-vs-angle traps inside one function: at `0x020d3f68` `0x1000` is 22.5°, at
  `0x020d4080` `0x4000` is 4.0 fx12 — same hex values, opposite readings, ~40 instructions apart.

### 9.8 Names earned by this pass

`0x02037e58` `CLPS::GetSlipperiness` · `0x02037e38` `CLPS::GetSurfaceType` ·
`ov002:0x020f02c8` `GetSlideAccelForClass` · `0x020f030c` `GetSlideLossFactorForClass` ·
`0x020f035c` `FloorIsSlope` · `0x020c031c` `Player::GetFloorClass` ·
`0x020c04e0` `Player::ApplySlopeAccel` · `0x020c06fc` `Player::UpdateSliding` ·
`0x020bf56c` `Player::GetSlopeDecelCoef` · `0x020c16ec` `Player::SetFloorSurfaceInfo` ·
`0x020d4d88` `Player::UpdateGroundSpeed` · `0x020d3b9c` `Player::UpdateWalkingSpeed` ·
`0x020d45c0` `Player::Walk_UpdateRunDustFx` · `0x020d413c` `Player::Walk_UpdateBodyLean` ·
`0x020bf30c` `Player::ScaleSpeedByCharacter` · `0x020c29d4` `Player::UpdateQuicksandFlag` ·
`ov098:0x02139228` `Crate::UpdateQuicksand` · `ov002:0x02110514` `St_Crawl` state descriptor

**Player offsets recovered:** `+0x8e` visual yaw · `+0x94` moveYaw · `+0x98` forwardVel ·
`+0xa8` velY · `+0x554/558/55c` floorNormal xyz (fx12, already rescaled from KCL's 0x400) ·
`+0x644` floorHeight · `+0x658` floorClass · `+0x65c/660/664/66c/670` CLPS fields ·
`+0x690` quicksand Y offset · `+0x69a` floorAngle · `+0x69c` slide-yaw delta ·
`+0x6ba` quicksandDepth · `+0x6d2` intendedYaw

---

## 10. Not done

- ~~Walking-speed model~~ / ~~slope acceleration~~ — **both settled in §9.** Walking curve is
  genuinely absent; slope/slide/friction/quicksand are near-verbatim ports.
- Level / course / star IDs: DS has no recovered enum at all, so no comparison was possible.
- **Where does the CLPS slipperiness class come from in the level data?** §9.1 establishes that
  bits 12-14 hold an authored class 0-5, but not how the DS toolchain assigned it per triangle,
  nor what distinguishes class 1 from class 2 in the source assets (class 1 carries extra DS-only
  roles: a latch in `SetFloorSurfaceInfo`, a KCL raycast height-tolerance switch in ITCM at
  `0x01ffc208`/`0x1ffc378`/`0x1ffc4e8`, a position snap, and a no-turn-around rule in `St_Shell_Main`).
- Air physics used the same method but was never re-swept with the corrected search technique
  (§9.7 — ARM rotated immediates, not just pool words). The airborne "absent" results in §2 may
  understate what is actually there.
- The `notes/actor-naming.md` contamination question in §4.

---

## 10. On structure (the original question)

**Do not restructure `src/` to mirror the N64 tree.** The flat one-function-per-file layout is
*forced*, not conventional: `tools/eligible.py` rule 1 rejects any object with more than one
`.text` section (mwccarm emits one per function), `tools/enroll.py` can only emit one address range
per file, and `delinks.txt` carves exactly one contiguous range per entry. Merged TUs fail
**silently** — the extra functions fall back to ROM bytes and the build still verifies green
(`tools/layout_check.py`). `notes/rom-build.md:326` already records this.

N64's transferable asset is not its directory layout. It is the discipline that the whole artifact
must reproduce from a clean clone, plus an honest marker for code that does not match. Subsystem
grouping here should be *logical* (a generated index, never committed while `src/` is flat — see
`tools/srcpath.py`), and the grouping axis is module/overlay, not N64 subsystem.
