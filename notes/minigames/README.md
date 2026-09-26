# How the minigames work

SM64DS has 30 touch-screen minigames, plus the menu that launches them. This page maps
how the code fits together, so the source can use names that the page explains. Two
other notes go deeper:
- [notes/minigame-provenance.md](../minigame-provenance.md) records where the field
  layouts came from.
- [themes.md](themes.md) lists the readability rewrites that recur across the suite, with
  whether each is byte-proven.

Claims are tagged where the source matters:
- **[ROM]**: read from RTTI, symbols or the cartridge bytes.
- **[CODE]**: read from matched source in this tree.
- **[GUESS]**: an inference that nothing has checked yet.

## Where the code lives

| Module | What it holds |
|---|---|
| arm9 | Entry and exit (`StartMinigameMenu`, `ExitMinigameMenu`), the scene-ID test `IsMinigameActorID`, `GetSceneOverlayID`, and the saved records (`SaveData::SaveMinigames`, `ReadMinigameData`, `SetDefaultValuesMg`). |
| ov005 | `dScMiniGm_c`, the select menu (scene 5, profile `MINIGAME`). |
| ov004 | `dScMgBase_c` and the runtime every game shares: the shared state machine, the pause menu and its `dMgPsOpt_c` options panel, the HUD and sprite records, and the score functions. |
| ov006 | The 30 game scenes, the two intermediate bases, and their helper objects (`dMgCardObj_c`, `dMgTrmp*`, `cMgSmartball_*`, ...). |

- ov005 and ov006 both load at 0x020bfec0, so the menu and a game are never resident at the same time. [ROM: both overlays' delinks]
- Loading ov006 also loads ov004. [CODE: `func_0201a798`]

## The class tree [ROM]

```
fBase_c > dBase_c > dScene_c
  |- dScMiniGm_c                 the select menu
  '- dScMgBase_c                 13 games derive from it directly
       |- dScMgSingle3DBase_c    13 games
       '- dScMgD3DBase_c         4 games: Jump, Jump2, Trampoline, Trampoline2
```

- **Every game is a scene, not an actor.**
  - Scene IDs 0x169..0x186 share the profile-ID space with actors, hence the name `IsMinigameActorID`.
  - The `dScMg` prefix is d + Sc(ene) + Mg (minigame).
- **Each base answers vtable slot 26 with its own constant**: 0, 1 or 2. `dScMgBase_c::BeforeInitResources` uses the answer to choose the display engines.
  - [GUESS] 0 means a 2D game, 1 means one 3D screen, and 2 means both screens are 3D, which would explain why D3DBase owns two cameras.

## A game, from menu to menu [CODE]

1. **Open the menu.** `StartMinigameMenu(flag)` stores `flag` and fades to scene 5, the menu.
   - [GUESS] The flag means "came from the castle rec room": the rec-room objects pass 1, and the title menu passes 0.
2. **Pick a game.** `dScMiniGm_c` copies the chosen 0x34-byte row of its table into `data_0209b308`, then fades to that row's scene ID.
   - The table has 36 rows for 30 scene IDs [ROM]. Six games appear twice, with different parameters.
3. **Load the game.** An ID in 0x169..0x186 loads ov006 (and with it ov004), and the profile's factory builds the scene.
4. **Play.** The scene runs until a quit from the pause menu, or an end-of-game state, fades back to scene 5.
5. **Leave the menu.** When the flag from step 1 is set, `ExitMinigameMenu` loads level 0x32 at entrance 4 and fades to scene 3, the stage. Otherwise it fades to scene 1.
   - [GUESS] Level 0x32 is the rec room.

## One frame [CODE]

`fBase_c::Process` runs `Before*`, then the method itself, then `After*`. If `Before*` returns 0, the method is skipped for that frame.

**Behavior.** `dScMgBase_c::BeforeBehavior` does the shared work, in this order:
1. Check the buttons. This runs only while the screen fader `data_0209f5bc` answers slot 5, `IsAtStart`, as true ([GUESS] no fade is running). If A, B, Select or Start was just pressed:
   - with the pause menu closed and the stylus up, open it (slot 27)
   - with it open, no item highlighted and the options panel idle, close it (slot 28)
2. While the pause menu is open, run it and return 0, so the game's own `Behavior` does not run. Its items are picked by touch.
3. Step the shared state machine, then the three UI records and the 0x40 sprite records.
4. Count `mFrameCounter` from 0 to 39, then wrap to 0.

**Render.** `BeforeRender` draws the menu instead of the game while it is open. Otherwise it draws the shared records and the state machine's overlay.

**What each game implements.** Every game overrides the same core set [ROM: vtable walk]:
- `InitResources`, `Behavior`, `Render` and its destructors
- slot 18, which games call with -1, 3, 4 or 5, for example `OnYoshiTryEat(-1)`
  - [GUESS] It resets or starts a round.
  - The name is a placeholder; see Names below. The base's slot-18 body is empty.

## Shared services [CODE]

- **Shared state machine.**
  - `mStateController` sits at `dScMgBase_c+0xcc`. Its type has no RTTI, so the name `dMgState_c` is the tree's own.
  - It owns 20 states, entered through `func_ov004_020b0a54(n)`.
  - [GUESS] The states run the common banners and jingles: ready, go, time up, results.
  - A game's own rules are a separate, game-local state table.
- **Score.** `func_ov004_020adb1c(score)` submits a score:
  - It writes the live score to `+0x464c`.
  - The menu row's scoring mode decides the direction: mode 5 is lower-is-better, and every other mode is capped and higher-is-better.
  - A new session best is stored at `+0x4654`. If it also beats the record copy at `+0x4650`, that copy is updated and the new-record flag at `+0x465c` is set.
- **Saved records.**
  - The scene's constructor loads the record copy from the save.
  - At cleanup, `func_ov004_020ad940` writes it back into the save table, but only when the menu row's word `+0x10` is 0.
  - Seven games (BSC, Card, Memory, Memory2, MCarlo, Roulette, Slot1) have that word set to 1. They save two values each through `func_ov004_020ad79c` instead. All but Roulette still submit through `func_ov004_020adb1c`.
- **Input.**
  - `data_0209f5bc` is the installed screen fader.
  - Each of the four input slots (index `data_020a0e40`) has a four-byte touch sample `{down, changed, x, y}` in `data_020a0de8`. A new touch is `down && changed`.
  - The per-slot button state is in `data_020a0e58`; its second halfword holds the buttons pressed this frame.
- **Pause menu.** Fields `+0x4628..+0x4648` of `dScMgBase_c` hold a three-item menu. The items are:
  - resume (slot 28)
  - quit to the menu (scene 5)
  - options: the `dMgPsOpt_c mTouchOptions` panel at `+0xf4`, with eight touch icons and a close timer

## Game file map

- **ID, profile and class** are all [ROM]; **Base** is the RTTI parent.
- **"+ N shards"** means N methods are still one-function files in `src/`.
- **Four games exist only as shards in `src/`:** Slot3, Pachinko2, Smartball and 3DEsp. Slot3 and Smartball also have a `src_tu` unit.
- **Five games have a promoted TU that holds their free helpers but few of their methods:**
  - Pachinko, Curling and Teresa: it also holds the destructors D1 and D0.
  - Bomroom and Coin: it holds no method.
  - Their other methods are still shards.

| ID | Profile | Class | Base | Source |
|---|---|---|---|---|
| 0x169 | MG_CUP | dScMgCup_c | Single3D | `src/actors/dScMgCup_c.cpp` |
| 0x16a | MG_MEMORY | dScMgMemory_c | Single3D | `src/minigames/d_s_mg_memory.cpp` |
| 0x16b | MG_MEMORY_J | dScMgMemory2_c | Single3D | `src/actors/dScMgMemory2_c.cpp` |
| 0x16c | MG_SLOT1 | dScMgSlot1_c | Base | `src/actors/dScMgSlot1_c.cpp` + 5 shards |
| 0x16d | MG_SLOT3 | dScMgSlot3_c | Single3D | 9 shards (a merged Slot1 and Slot3 unit exists only as a `src_tu` shadow) |
| 0x16e | MG_LUIGI | dScMgLuigi_c | Base | `src/actors/dScMgLuigi_c.cpp` |
| 0x16f | MG_SOUND | dScMgSound_c | Single3D | `src/actors/dScMgSound_c.cpp` |
| 0x170 | MG_PACHINKO | dScMgPachinko_c | Base | `src/actors/dScMgPachinko_c.cpp` + 4 shards |
| 0x171 | MG_TAMAIRE | dScMgPachinko2_c | Base | 6 shards |
| 0x172 | MG_BOMROOM | dScMgBomroom_c | Base | `src/actors/dScMgBomroom_c.cpp` + 6 shards |
| 0x173 | MG_AMIDA | dScMgAmida_c | Base | `src/actors/dScMgAmida_c.cpp` + 6 shards |
| 0x174 | MG_JUMP | dScMgJump_c | D3D | `src/actors/dScMgJump_c.cpp` + 1 shard |
| 0x175 | MG_JUMP2 | dScMgJump2_c | D3D | `src/minigames/d_s_mg_jump2.cpp` |
| 0x176 | MG_CURLING | dScMgCurling_c | Base | `src/actors/dScMgCurling_c.cpp` + 4 shards |
| 0x177 | MG_CURLING_J | dScMgCurling2_c | Base | `src/actors/dScMgCurling2_c.cpp` + 4 shards |
| 0x178 | MG_SMARTBALL | dScMgSmartball_c | Base | 9 shards (shadow: `src_tu/actors/dScMgSmartball_c.cpp`) |
| 0x179 | MG_SNOWBALL | dScMgSnowball_c | Single3D | `src/actors/dScMgSnowball_c.cpp` |
| 0x17a | MG_COIN | dScMgCoin_c | Base | `src/actors/dScMgCoin_c.cpp` + 6 shards |
| 0x17b | MG_CARD | dScMgCard_c | Single3D | `src/minigames/d_s_mg_card.cpp` |
| 0x17c | MG_PANEL | dScMgPanel_c | Base | `src/actors/dScMgPanel_c.cpp` |
| 0x17d | MG_MCARLO | dScMgMCarlo_c | Single3D | `src/minigames/d_s_mg_m_carlo.cpp` |
| 0x17e | MG_MCARLO2 | dScMgMCarlo2_c | Single3D | `src/minigames/d_s_mg_m_carlo2.cpp` |
| 0x17f | MG_ROULETTE | dScMgRoulette_c | Single3D | `src/actors/dScMgRoulette_c.cpp` |
| 0x180 | MG_TRAMPOLINE | dScMgTrampoline_c | D3D | `src/minigames/d_s_mg_trampoline.cpp` |
| 0x181 | MG_TRAMPOLINE2 | dScMgTrampoline2_c | D3D | `src/minigames/d_s_mg_trampoline2.cpp` |
| 0x182 | MG_HANACHAN | dScMgHanachan_c | Base | `src/actors/dScMgHanachan_c.cpp` + 2 shards |
| 0x183 | MG_TERESA | dScMgTeresa_c | Base | `src/actors/dScMgTeresa_c.cpp` + 6 shards |
| 0x184 | MG_BS_CARD | dScMgBSC_c | Single3D | `src/minigames/d_s_mg_bsc.cpp` |
| 0x185 | MG_3DESP | dScMg3DEsp_c | Single3D | 8 shards |
| 0x186 | MG_FLOWER | dScMgFlower_c | Single3D | `src/actors/dScMgFlower_c.cpp` |

**The bases:**
- `src/minigames/d_s_mg_base.cpp` (plus 26 `dScMgBase_c` shards)
- `src/minigames/d_s_mg_single3_d_base.cpp`
- `src/actors/dScMgD3DBase_c.cpp`

**The shared ov004 runtime:**
- `src/actors/dMgPsOpt_c.cpp`
- the text-verified shadows `src_tu/actors/dMgState_c.cpp` and `src_tu/actors/unit_ov004_020b0a38.cpp`

## Names: what is known

- **Certain [ROM]:**
  - every class name above, and the helper classes' names
  - profile strings, scene IDs and class sizes
  - which vtable slots each class overrides
- **Placeholders:**
  - `dScMgBase_c` slots 18, 19 and 21-30 borrow `dActor_c` method names by slot index only, so `OnYoshiTryEat` does not mean Yoshi. See [notes/dScMgBase_c-slots-18-35.md](../dScMgBase_c-slots-18-35.md).
  - Slots 20 and 31-35 are named after their vtable offset, for example `Virtual7C`.
- **Coined or inferred in this tree [CODE]:**
  - `dMgState_c`, the three shared-state block types, and many method and field names.
  - The markers are not uniform yet: some say "coined", some "inferred" or "reconstructed", and the `dScMgBase_c.h` field names carry none.
- **Candidate only:** the `d_s_mg_*.cpp` filenames.

**Naming rule for the readability passes.** A new name needs evidence from its uses and is marked `coined`. Without evidence, the placeholder stays, with a one-line note on what is known.

## Open questions

- **Shared states:** what do the 20 shared states show? Callers most often enter 0x12, 5, 4, 0 and 0x10.
- **Scoring modes:** what separate modes 0, 1 and 3? Mode 5 is Snowball's lower-is-better time.
- **Duplicate rows:** are the six duplicated menu rows one game offered in two variants?
- **Pause gate:** why does `BeforeBehavior` stop a game while `unk_462c` is set, or while `unk_0a4` is 0 and the fader is not at its start?
- **Local mislabels:** `d_s_mg_base.cpp` and `dScMiniGm_c.cpp` still call the fader the "stylus owner", and `dScMiniGm_c.cpp` names the touch bytes `{pressed, held}`. A later pass should fix these comments.
- **Slot 18:** what does it really do? Callers pass -1, 3, 4 and 5.
