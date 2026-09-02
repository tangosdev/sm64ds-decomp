# VSMAP - the VS arenas against the cartridge

Lane worker. Branch `port/vsmap`, worktree `C:\tmp\vsmap`, private build dir
`C:\tmp\vsmap\build\port`. Base: `port-mount-noseat-cluster` @ ad09cd9f5.

The field report from a seven-player 0.3.2 match: the VS arenas play the WRONG
MUSIC, have NO ENEMIES and NO CAPS. This lane answers each of the three against
the ROM's own code and level data rather than against a memory of the match.

Two instruments were built first, because all three questions are counting
questions and the lane had no counter:

- `port/tools/vs_objcensus.py` - what the CARTRIDGE places. Walks each arena's
  LVL_Overlay the way `src/_Z11LoadObjectsRN11LVL_Overlay8ObjTableEij.c` walks
  it, applies the ROM's own star-group filter, resolves each record's raw index
  through `data_ov002_0210cbf4`, and attributes every actor id to the overlay
  its SpawnInfo lives in.
- `port/tools/vs_arena_census.py` - what the PORT gets. One quiet muted boot per
  arena through `mp2_proof.env_base`, differenced against the roll-up above, with
  the star filter, the sound group and bank, and every layer-1 sequence the run
  issued.

---

## 1. THE FILTER, and why "star filter 2" is the whole of VS object loading

`src/_Z11LoadObjectsRN11LVL_Overlay8ObjTableEij.c:4-17` is the entire function:

```c
unsigned char* e = *(unsigned char**)(t + 4);
for (i = 0; i < *(unsigned short*)t; i++) {
    unsigned char b = *e;
    int type = (b >> 5) & 7;
    if (type == 0 || type == data_0209f220) {
        void (*h)(void*, int, unsigned int) = data_ov002_0210cbb8[b & 0x1f];
        if (h != 0) h(e, a1, a2);
    }
    e += 8;
}
```

The descriptor byte at the head of each 8-byte sub-table entry carries the
sub-loader index in bits 0..4 and the STAR GROUP in bits 5..7. A whole sub-table
either runs or does not; there is no per-record mask. So a VS arena is not an
adventure level with things hidden - it is a level carrying a SECOND object list
that only star group 2 reaches.

`data_0209f220` is 2 in a match, and every step of that is a read:

| step | file:line | what it does |
|---|---|---|
| the VS start | `src/func_ov075_02116c8c.c:11` | `LoadLevelNoReturn(data_ov075_0211c6ec[idx], 0, 2, 0)` - the literal 2 is `starID` |
| stage it | `src/LoadLevel.c:30-34` | `data_0209f1f0 = starID` |
| survive the clamp | `src/SetNextStar.c:11-13` | would force it back to 1, but `data_0209f2d8 == 1` returns first |
| latch it | `src/_ZN5Stage13InitResourcesEv.cpp:210` | `data_0209f220 = data_0209f1f0` - the only write to it in the tree |

The same 2 arms the VS sound-group branch at `InitResources:316`, so one value
decides both the arena's object list and its sound bank.

**The port already seats it.** `port/tests/walk_window.cpp:6702` writes
`SM64DS_STAR_FILTER=2` with `_putenv_s` (not `SetEnvironmentVariableA`, and the
comment there records why), and `port/hal/level_boot.cpp:3527-3529` reads it into
`data_0209f220`. Every field log carries `star filter 2` on the `[vs] ENTERING
VS` line.

## 2. What the arenas actually place

`python port/tools/vs_objcensus.py`, filter 2, the ROM's own bytes. The last
column is the overlay each class's SpawnInfo lives in.

### VS map 1 - level 51, ov059 (VS-only; every sub-table is group 0)

|  id | class | x | home |
|---|---|---|---|
|  31 | PATH_LIFT | 1 | ov100 |
| 178 | POWER_STAR | 4 | ov002 |
| 180 | STAR_MARKER | 4 | ov002 |
| 197 | WATERFALL_MIST | 7 | ov002 |
| 269 | CAP | 6 | ov002 |
| 286 | TREE | 12 | ov002 |
| 337 | UNCHAINED_CHOMP | 1 | ov100 |
| 350 | AMBIENT_SOUND | 5 | ov002 |

### VS map 2 - level 43, ov051

|  id | class | x | home |
|---|---|---|---|
|  22 | EXCLAMATION_BLOCK_VS | 1 | ov102 |
| 178 | POWER_STAR | 5 | ov002 |
| 180 | STAR_MARKER | 5 | ov002 |
| 269 | CAP | 6 | ov002 |
| 287 | INVISIBLE_POLE | 5 | ov002 |
| 288 | COIN | 8 | ov002 |

### VS map 3 - level 29, ov037

|  id | class | x | home |
|---|---|---|---|
| 178 | POWER_STAR | 5 | ov002 |
| 269 | CAP | 15 | ov002 |
| 288 | COIN | 17 | ov002 |
| 290 | BLUE_COIN | 5 | ov002 |

### VS map 4 - level 42, ov050

|  id | class | x | home |
|---|---|---|---|
|  22 | EXCLAMATION_BLOCK_VS | 2 | ov102 |
| 178 | POWER_STAR | 5 | ov002 |
| 180 | STAR_MARKER | 5 | ov002 |
| 220 | ROLLING_IRON_BALL | 2 | ov100 |
| 269 | CAP | 6 | ov002 |
| 287 | INVISIBLE_POLE | 1 | ov002 |
| 288 | COIN | 8 | ov002 |

### The control that makes those tables trustworthy

The same reader at `--star 1` - the adventure filter - turns level 43 back into
an adventure course, and it is a different world:

```
  PLACED under filter 1:
       21 0x015  x1   EXCLAMATION_BLOCK            ov102
      179 0x0b3  x4   SILVER_STAR                  ov002
      184 0x0b8  x1   SIGN_POST                    ov002
      200 0x0c8  x3   GOOMBA                       ov006
      250 0x0fa  x1   PIRANHA_PLANT                ov006
      265 0x109  x2   LAKITU                       ov006
      276 0x114  x1   ONE_UP_MUSHROOM              ov002
      288 0x120  x10  COIN                         ov002
  FILTERED OUT (another star group):
       22 0x016  x1   EXCLAMATION_BLOCK_VS         ov102
      178 0x0b2  x5   POWER_STAR                   ov002
      180 0x0b4  x5   STAR_MARKER                  ov002
      269 0x10d  x6   CAP                          ov002
      288 0x120  x8   COIN                         ov002
```

The two lists are complementary and the pairing is structural: under filter 2
the five POWER_STARs sit at the same five coordinates as the five STAR_MARKERs,
which is how a placed star is spelled. So the reader is decoding real records,
not noise.

## 3. NO ENEMIES: the cartridge has none either

Every enemy on maps 2, 3 and 4 - GOOMBA, PIRANHA_PLANT, LAKITU, HEAVE_HO, and
the two unnamed ov006 ids - is in a star-group-1 sub-table. `LoadObjects` skips
it under filter 2, so a DS VS match has never spawned any of them. What the ROM
does place that MOVES is one UNCHAINED_CHOMP and one PATH_LIFT on map 1 and two
ROLLING_IRON_BALLs on map 4, and that is the whole cast.

The VS mode is a star battle. The ROM's own `LoadEntranceObjects` says so on the
same page: `src/_Z19LoadEntranceObjects...c:71-76` forces every slot's character
to 3 (Yoshi) when `data_0209f2d8 == 1`, and the CAPs listed above are what turn
a Yoshi into Mario, Luigi or Wario.

## 4. THE MUSIC: the port already plays the ROM's track, on every arena

The arena's music is not chosen per level. `src/_ZN5Stage8BehaviorEv.cpp:166`
is a literal:

```c
164:  } else {
165:      func_02012790(0x2a);                       // Play2D(2, 0x2a), the GO
166:      Sound::LoadAndSetMusic_Layer1(0x4d);
167:  }
```

`0x4d` is the only music id in that block and nothing there reads the level
number. `Sound::LoadAndSetMusic_Layer1` passes its argument straight through
(`src/_ZN5Sound22LoadAndSetMusic_Layer1Ei.c:13-21`), so the sound GROUP cannot
redirect it either. Out of the ROM's own SDAT symbol block
(`extracted/dsd/files/data/sound_data.sdat`, SYMB/SEQ):

```
0x4c  NCS_BGM_VSSHIRO       the VS menu
0x4d  NCS_BGM_VSATHRETIC    the arena
0x4e  NCS_BGM_VSMUTEKI      the VS power-up variation
0x51  NCS_BGM_VSWIN
0x52  NCS_BGM_VSLOSE
```

So one arena track, four arenas, and the port issues exactly it: every field log
carries `[vsgo] countdown finished ... LoadAndSetMusic_Layer1(0x4d)`.

Three further pieces of the same statement, all of which the port already has:

- **There is no music during the 3-2-1.** `InitResources:365-366` gates the whole
  per-level music load out when `data_0209f2d8 == 1`, so the arena comes up with
  layer 1 stopped (the VS start's own `StopLoadedMusic_Layer1(0x3c)`,
  `src/func_ov075_02116c8c.c:12`) and stays silent until GO.
  `port/hal/star_flow.cpp:331-345` reproduces that guard and says why; the port
  used to call the table row and got level 51's `-1` (silence) and level 42's 65
  (the Battle Fort ADVENTURE track). That was the wrong-music bug and it is
  already fixed in this base.
- **The sound GROUP is per-arena and the bank is not.**
  `InitResources:316-327`: group 0x2B/0x2E/0x2C/0x2D by level, bank forced to
  0x36. `port/hal/star_flow.cpp:249-258` is the same four constants.
- **The wing cap changes the music in VS and only in VS.**
  `src/_ZN6Player16InitWingFeathersEb.cpp:24-27` sets variation 0x4e when
  `data_0209f2d8 == 1` and 0x33 otherwise. Matched src, in the port's link.

## 5. What is NOT true, and was believed going in

- *"the VS-specific object sub-table is not being loaded"* - it is. The filter is
  seated at 2 and every field log says so; map 1 has no group-2 table at all
  (every one of its sub-tables is group 0), so on that arena the filter is inert.
- *"levels 29 and 42 have data_02092208 rows that do not decode as a
  LVL_Overlay"* - that note in `port/hal/actor_registry.cpp` is about a static
  sweep over UNMOUNTED levels. Both arenas decode cleanly from their own mounted
  overlays (level 29 -> ov037 0x021111ec, level 42 -> ov050 0x02111204), and
  `port_level_mounts_install` asserts those addresses against the ROM at boot.
- *"the ROM plays a different track per arena"* - it plays 0x4d on all four.

One thing that IS surprising and is recorded rather than acted on:
`src/_Z26LoadOrUnloadObjectOverlaysPFviEi.cpp:12-14` returns before loading any
object overlay when `data_0209f2d8 == 1`, and the ARM at 0x0202df70 confirms it
(`ldrb r0,[r2]; cmp r0,#1; ... bxne lr`). On the cartridge a match therefore
mounts NO object overlay and depends on whatever ov100/ov102 residency the
previous scene left behind. The port mounts its overlays statically and so is
immune; nothing is faked to reproduce a fragility.

<!-- RUNTIME SECTIONS APPENDED BELOW ONCE THE PROOF RUNS -->
