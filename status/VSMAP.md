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

---

# THE RUNTIME HALF: what the port actually does, per arena

Build green: `port/build-port.cmd` clean, every pre-configure selftest PASS, and
`alternatename_guard`, `gxband_guard` (layout OK) and `tailjump_guard` (97
assertions over 24 maps, forms OK) all green post-link.

`python port/tools/vs_arena_census.py --frames 420`, one quiet muted boot per
arena, **VERDICT: PASS**. Verbatim, with the SEQARC sound-effect traffic filtered
out of the stream list:

```
========================================================================
VS map 1  level 51   rc 0
  star filter : 2
  sound group : 43 bank 0x36 (table bgm column -1, unread in VS)
  layer-1 seq : 0x4d
  music streams started, in order (plus 57 SEQARC sound effects):
      0x4d NCS_BGM_VSATHRETIC bank 128 (sdat+0x3dcbc, +0x1c into the file)
  ROM places / port spawns / port keeps alive, by class:
       31  rom x1    spawn x1    alive x1
      178  rom x4    spawn x4    alive x4
      180  rom x4    spawn x4    alive x4
      191  rom x0    spawn x1    alive x0     (not placed: HUD, player, minimap, a spawn's child)
      197  rom x7    spawn x7    alive x7
      269  rom x6    spawn x6    alive x6
      286  rom x12   spawn x12   alive x1     (folded: Tree::InitResources, see below)
      332  rom x0    spawn x1    alive x0     (not placed)
      334  rom x0    spawn x1    alive x1     (not placed)
      335  rom x0    spawn x1    alive x1     (not placed)
      337  rom x1    spawn x1    alive x1
      350  rom x5    spawn x5    alive x5

========================================================================
VS map 2  level 43   rc 0
  star filter : 2
  sound group : 45 bank 0x36 (table bgm column 68, unread in VS)
  layer-1 seq : 0x4d
  music streams started, in order (plus 37 SEQARC sound effects):
      0x4d NCS_BGM_VSATHRETIC bank 128 (sdat+0x3dcbc, +0x1c into the file)
  ROM places / port spawns / port keeps alive, by class:
       22  rom x1    spawn x1    alive x1
      178  rom x5    spawn x5    alive x5
      180  rom x5    spawn x5    alive x5
      269  rom x6    spawn x6    alive x6
      287  rom x5    spawn x5    alive x5
      288  rom x8    spawn x8    alive x8

========================================================================
VS map 3  level 29   rc 0
  star filter : 2
  sound group : 46 bank 0x36 (table bgm column 65, unread in VS)
  layer-1 seq : 0x4d
  music streams started, in order (plus 29 SEQARC sound effects):
      0x4d NCS_BGM_VSATHRETIC bank 128 (sdat+0x3dcbc, +0x1c into the file)
  ROM places / port spawns / port keeps alive, by class:
      178  rom x5    spawn x5    alive x5
      180  rom x0    spawn x5    alive x5     (PowerStar's own child, see below)
      269  rom x15   spawn x15   alive x15
      288  rom x17   spawn x17   alive x17
      290  rom x5    spawn x5    alive x5

========================================================================
VS map 4  level 42   rc 0
  star filter : 2
  sound group : 44 bank 0x36 (table bgm column 65, unread in VS)
  layer-1 seq : 0x4d
  music streams started, in order (plus 44 SEQARC sound effects):
      0x4d NCS_BGM_VSATHRETIC bank 128 (sdat+0x3dcbc, +0x1c into the file)
  ROM places / port spawns / port keeps alive, by class:
       22  rom x2    spawn x2    alive x2
      178  rom x5    spawn x5    alive x5
      180  rom x5    spawn x5    alive x5
      220  rom x2    spawn x2    alive x2
      269  rom x6    spawn x6    alive x6
      287  rom x1    spawn x1    alive x1
      288  rom x8    spawn x8    alive x8

VERDICT: PASS
```

**Zero declined spawns on any arena.** Every log carries
`[census] N spawned (M classes), 0 skipped (0 classes)`, so the registry gate in
`port/hal/actor_registry.cpp` declines nothing here, which matches the field
logs, where the only decline anywhere is one unrelated id 0x14a.

### The two rows that are not equalities, both adjudicated

- **TREE 286, map 1: 12 placed, 12 spawned, 1 on the behaviour list.** The ROM's
  own arrangement, not a loss. `src/_ZN4Tree13InitResourcesEv.cpp:34-38`
  allocates a 0x4c-byte node, links it into `data_ov002_02110a48[modelIndex]`,
  and then

      *(int*)(p + 0x48) = *slot;
      *slot = (int)p;
      if (*(int*)(p + 0x48) != 0) return 0; else return 1;

  so the FIRST tree of a model index returns 1 and stays an actor and every later
  one returns 0 and lives as a node on that list, which the surviving tree
  renders. All twelve trees are on the list; one of them is the actor.
- **STAR_MARKER 180, map 3: 0 placed, 5 spawned.** Also the ROM's:
  `src/_ZN9PowerStar13InitResourcesEv.cpp:171` and `:237` spawn actor `0xb4`
  (180) from the star itself. The proof is in the positions: map 3's five
  STAR_MARKERs read back at exactly the five POWER_STAR coordinates
  `(4500,6300,-5500) (1829,-799,2735) (-6209,-4080,5571) (-5185,-4180,6189)
  (-7210,-4180,6189)`. Maps 1, 2 and 4 place their markers in the level data and
  the counts there are exact equalities, so the star takes the other branch on
  those.

## The four-window proof, all four arenas

`python port/tools/vs4_proof.py --map <0..3> --frames 900`, muted and minimised
through `mp2_proof.env_base`. **40 PASS / 0 FAIL on every map**, including all
six pairings of rung 8:

| map | level | verdict | rung 7 scores across windows | rung 8 |
|---|---|---|---|---|
| 1 | 51 | ALL GREEN | `(2,0,0,0)` x4 | 6/6 pairings NO DIVERGENCE, 401 common frames |
| 2 | 43 | ALL GREEN | `(2,0,0,0)` x4 | 6/6 pairings NO DIVERGENCE, 401 common frames |
| 3 | 29 | ALL GREEN | `(2,0,0,0)` x4 | 6/6 pairings NO DIVERGENCE, 401 common frames |
| 4 | 42 | ALL GREEN | `(2,0,0,0)` x4 | 6/6 pairings NO DIVERGENCE, 401 common frames |

The per-window census is identical across all four windows on every arena, and
the sequence is issued in all four:

```
VS map 1   p0..p3  [census] 47 spawned (12 classes), 0 skipped   LoadAndSetMusic_Layer1(0x4d)
     +  31 x1  PATH_LIFT        + 178 x4  POWER_STAR      + 180 x4  STAR_MARKER
     + 191 x4  PLAYER           + 197 x7  WATERFALL_MIST  + 269 x6  CAP
     + 286 x12 TREE             + 332 x1  CAMERA          + 334 x1  HUD
     + 335 x1  MINIMAP          + 337 x1  UNCHAINED_CHOMP + 350 x5  AMBIENT_SOUND

VS map 2   p0..p3  [census] 37 spawned (10 classes), 0 skipped   LoadAndSetMusic_Layer1(0x4d)
     +  22 x1  EXCLAMATION_BLOCK_VS  + 178 x5  POWER_STAR   + 180 x5  STAR_MARKER
     + 191 x4  PLAYER                + 269 x6  CAP          + 287 x5  INVISIBLE_POLE
     + 288 x8  COIN                  + 332 x1  CAMERA       + 334 x1  HUD
     + 335 x1  MINIMAP

VS map 3   p0..p3  [census] 54 spawned (9 classes), 0 skipped    LoadAndSetMusic_Layer1(0x4d)
     + 178 x5  POWER_STAR       + 180 x5  STAR_MARKER     + 191 x4  PLAYER
     + 269 x15 CAP              + 288 x17 COIN            + 290 x5  BLUE_COIN
     + 332 x1  CAMERA           + 334 x1  HUD             + 335 x1  MINIMAP

VS map 4   p0..p3  [census] 36 spawned (11 classes), 0 skipped   LoadAndSetMusic_Layer1(0x4d)
     +  22 x2  EXCLAMATION_BLOCK_VS  + 178 x5  POWER_STAR + 180 x5  STAR_MARKER
     + 191 x4  PLAYER                + 220 x2  ROLLING_IRON_BALL   + 269 x6  CAP
     + 287 x1  INVISIBLE_POLE        + 288 x8  COIN       + 332 x1  CAMERA
     + 334 x1  HUD                   + 335 x1  MINIMAP
```

The four-player census is larger than the solo one by the three extra PLAYERs;
the CAMERA/HUD/MINIMAP trio the boot always makes is in both.

---

# THE VERDICT, per arena

| arena | ROM says | the port did | the port now does |
|---|---|---|---|
| map 1, level 51 | seq 0x4d at GO, silence before; group 0x2B bank 0x36; 40 objects in 8 classes, one enemy (UNCHAINED_CHOMP), 6 CAPs, no VS block | the same | unchanged, nothing to fix |
| map 2, level 43 | seq 0x4d at GO; group 0x2D bank 0x36; 30 objects in 6 classes, no enemies, 6 CAPs, 1 EXCLAMATION_BLOCK_VS | the same | unchanged |
| map 3, level 29 | seq 0x4d at GO; group 0x2E bank 0x36; 42 objects in 4 classes, no enemies, 15 CAPs, no VS block | the same | unchanged |
| map 4, level 42 | seq 0x4d at GO; group 0x2C bank 0x36; 29 objects in 7 classes, ROLLING_IRON_BALL x2, 6 CAPs, 2 EXCLAMATION_BLOCK_VS | the same | unchanged |

**No source change was needed and none was made.** The three reports resolve as:

- **WRONG MUSIC** - already fixed in this base by 20621ecf0 and 4d251d9af, and
  now proved at the sequencer rather than at the call site: the arena starts SSEQ
  0x4d `NCS_BGM_VSATHRETIC`, bank 128, on all four maps. Worth knowing why it can
  still SOUND wrong to a player: 0x4d and 0x41 `NCS_BGM_ATHRETIC` are the SAME
  SSEQ FILE byte for byte (md5 `eaf021baab2f51612f325ef5cdea7cb0`, 8324 bytes,
  file ids 73 and 62 pointing at different FAT entries holding identical
  content), differing only in the bank their INFO records name (128 against 101).
  The VS arena music IS the athletic theme on a different instrument set. That is
  the cartridge.
- **NO ENEMIES** - the cartridge has none. Every enemy in these four levels sits
  in a star-group-1 sub-table that `LoadObjects` skips at filter 2.
- **NO CAPS** - the CAPs are there, all of them, on every arena: 6 / 6 / 15 / 6
  alive, at the ROM's own coordinates, in every one of the four windows. So are
  the VS `!` blocks where the ROM puts them: 1 on map 2, 2 on map 4, and NONE on
  maps 1 and 3. If the reported match was on map 1 or map 3, "no cap blocks" is
  the cartridge's own answer.

## Gaps and debts, stated rather than closed

1. **The CAPs and blocks are alive but this lane did not LOOK at them.** Every
   number here is a count and a coordinate off the behaviour list. Whether a CAP
   renders, and whether picking one up turns a Yoshi into Mario, is a visual and a
   gameplay question this lane is not allowed to judge for itself. That is the
   next thing to check and it wants the owner's eyes on a capture.
2. **The countdown's two missing ROM preconditions** are still open, already
   recorded in `port/hal/star_flow.cpp` and `port/stage_lifecycle_map.txt`: the
   `data_0209fc9c` pause arm and the `data_0209fc68 == 6` match-end arm are not
   hosted, so `port_vs_countdown_tick` ticks where the ROM would suppress. Not
   observable today; it becomes observable the day either piece of state goes
   live.
3. **`src/_Z26LoadOrUnloadObjectOverlaysPFviEi.cpp:12-14` is unreproduced, on
   purpose.** In a match the cartridge loads NO object overlay at all and relies
   on residency from the previous scene for ov100 and ov102, which is where
   UNCHAINED_CHOMP, PATH_LIFT, ROLLING_IRON_BALL and the two `!` block classes
   live. The port mounts its overlays statically and so is immune. Faking the
   fragility would make the port worse, so nothing was done; it is recorded
   because a reader who finds ov102 classes spawning in a match will otherwise
   wonder how.
4. **Seven players, not four.** The field report is from a seven-player match and
   every proof here is four windows, because `vs4_proof.py` is the four-window
   harness. Nothing in the object load or the music path is player-count
   dependent, both run once at the boot ahead of any seat past four, but that is
   an argument rather than a measurement, and the wide-session harness in
   `status/VS16HOST.md` is where it would be measured.
5. **420 and 900 frames.** Long enough to reach the countdown and a MATCH OVER,
   not long enough to say anything about a full-length match.
