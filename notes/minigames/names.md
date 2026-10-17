# Minigame names: evidence and confidence

The cartridge carries class names (RTTI) but no method, field or helper names
for the minigame code. Every name on this page is coined in this tree. Each one
records:
- the evidence it rests on
- a confidence level
- for anything below **high**, what to watch at runtime to settle it (see
  [runtime-checks.md](runtime-checks.md))

Confidence levels:
- **high**: the body and every caller agree, and no other reading fits.
- **medium**: the role is clear from the code, but the exact meaning rests on
  something not yet observed, such as which banner graphic a file index shows.
- **low**: a best guess from one caller or one body. Rename freely when better
  evidence turns up.

A rename is checked by compiling every minigame TU before and after. The objects
must be identical apart from the renamed symbols: same section bytes, same
relocation offsets and types, same targets.

## dScMgBase_c virtual slots 18-35 (renamed 2026-10-14)

Until this rename, slots 18-30 carried `dActor_c` method names, copied across by
vtable slot index alone. `dActor_c` and `dScMgBase_c` share only `dBase_c`,
which declares no virtual, so a slot number means nothing between them.
- **Where they came from.** The actor names are themselves imports from an ASM-hack
  symbol file. An untooled "vtable-identity" pass (#877) wrote them onto the scene
  classes as `recovered from vtable slot identity` comments.
- **How they spread.** `tools/reconcile_names.py` (#944) turned those comments into
  `// recovered name:` lines. The slot campaign of 2026-08-31 (#2088-#2112) then made
  them symbols. Slots 31 and 32 got `Kill` and `AfterClsn` the same way, from
  `dBgActor_c` and `dPathLiftActor_c`.
- **Reach.** No other non-actor class has a vtable longer than 18 slots, so the
  transplant is confined to these 33 classes.

| Slot | Old name | New name | Confidence | Evidence |
|---|---|---|---|---|
| 18 | `OnYoshiTryEat(int)` | `StartRound(int)` | high (seen) | `dMgState_c` calls it with the finished state's number when a banner ends. Games call it with -1 to retry after a lost life (Cup's `StateFinish`). The overrides reset their round, and Memory's calls `ResetGame`. |
| 19 | `OnTurnIntoEgg(int)` | `CanEndState(int)` | high | Every `dMgState_c` behaviour handler polls it with `mState` and waits for nonzero. The overrides return 0 while their own reaction animation plays. |
| 20 | `Virtual50` | `OnNewRecord` | high, seen | Its one caller is state 17's behaviour, the results path taken when the new-record flag is set, 120 frames in. Seen in play: a record-beating Teresa run and a Snowball finish both ran it, with banner 5 and jingle 1 (runtime-checks.md). Cup and Sound play a win animation; 3DEsp and Teresa free banner slot 8. |
| 21 | `OnGroundPounded` | `OnDealDone` | high (seen) | Seen in Memory and Card: it fires as Luigi's opening shuffle-and-deal plays, from `Dealer_StateShuffle`. Memory's override goes straight to `MgCoins_PlaceBet(3)`. Its one caller is `Dealer_StateShuffle` (unit020bfec0, was `Dealer_StateDeal`): the shuffle loop calls it when its repeat count reaches the mark set at the start (`+0x1d8 == +0x1da`). Memory and Memory2 then place their bet coins (`MgCoins_PlaceBet(mMaxMisses)`); BSC raises its bet from the score tier. |
| 22 | `OnAttacked1` | `OnGraphCallback1` | high (mapping) | `graphCallback_c::GraphCallback1` forwards to it. Runs in the main loop after the scene step, before the frame wait (`func_02019404`). |
| 23 | `OnAttacked2` | `OnGraphCallback0` | high (mapping) | `GraphCallback0` forwards to it. Runs at the main loop's frame start; the arm9 caller `func_02019390` skips its OAM reset and 3D flush when the callback returns 0. |
| 24 | `OnKicked` | `OnGraphCallback2` | high (mapping) | `GraphCallback2` returns its result. Runs in the VBlank IRQ before the OAM flush and display-register commit (`func_02019144`), which a 0 return skips. The base body saves or restores the screen on an edge of `mMenuOpen`. |
| 25 | `OnPushed` | `OnGraphCallback3` | high (mapping) | `GraphCallback3` forwards to it. Runs at the end of the VBlank IRQ (`func_02019100`). The base returns `mMenuOpen == 0`. |
| 26 | `OnHitByCannonBlastedChar` | `GetDisplayMode` | high | It returns 0, 1 or 2 from Base, Single3DBase and D3DBase. `BeforeInitResources` enables the 3D engines for any nonzero value, and the sprite wrappers choose a screen on 2. |
| 27 | `OnHitByMegaChar` | `OpenMenu` | high | It sets the three menu items to their slide-in positions and sets `mMenuOpen = 1`. `BeforeBehavior` calls it on A, B, Select or Start with the stylus up. |
| 28 | `OnHitFromUnderneath` | `CloseMenu` | high | It clears `mMenuOpen` and resets the options panel. It runs from the resume item, or from a button press with nothing highlighted. |
| 29 | `OnAimedAtWithEgg` | `SaveScreenForMenu` | high | It saves POWCNT1, both DISPCNT BG bits, 0x400 bytes of palette and 0x2000 bytes of OBJ VRAM, then draws the menu graphics. It runs on the rising edge of `mMenuOpen`. |
| 30 | `OnAimedAtWithEggReturnVec` | `RestoreScreenFromMenu` | high | It is slot 29's exact mirror and runs on the falling edge. It returns nothing (the bytes settle `void`). |
| 31 | `Virtual7C` (was `Kill`) | `InitSubScreenBg` | high | It sets up sub-engine BG1 and loads the language's character file plus screen map 0x5b. |
| 32 | `Virtual80` (was `AfterClsn`) | `InitMainScreenBg` | high | It does the same for main-engine BG1 with screen map 0x67. |
| 33 | `Virtual84` | `InitEngines` | high | It sets graphics modes, VRAM banks, the BG/OBJ character files and palettes, and registers the graph callback. (`InitDisplay` was already taken by Trampoline's own method.) |
| 34 | `Virtual88(int,int,int,int)` | `PaintSquare` | high | It paints a size x size square of one 4bpp palette index into the brush's BG layer. The line rasteriser `Mg_DrawLine` calls it once per point. |
| 35 | `Virtual8C` | `IsAltVariant` | high, seen | It returns `(param1 & 0xff) != 0`, and Amida asks `== 1`. Coin, Panel, Sound and Snowball branch on it to pick their second variant's assets. The menu passes `MgEntry::sceneParam` as param1, the menu row's second word: its low byte is 1 on each duplicated game's second row (2 on Amida's), so the name holds. The second entries of Amida, Snowball, Smartball, Coin, Panel and Slot1 set the byte, and the two Snowballs were seen starting with 0x0a00 and 0x2201 (pass 4). Seen on Panel's row 35 (runtime-checks.md). Sound has only one row, so its branch never runs from the menu, and no Amida row is variant 1; Amida's own slot 36, `IsVariant2` (was `Unk36`, pass 6), asks `== 2`: that is its second-row test, which its second Rec Room entry (row 24) passes. |

The graph callbacks' frame phases are seen (runtime-checks.md, 2026-10-15). Each
frame runs 0 at the start, before the scene update (`func_02019390`), then the
scene's `BeforeBehavior`, then 1 after the scene update (`func_02019404`). In the
VBlank IRQ, 2 runs before the OAM flush and display-register writes, which it
can veto (`func_02019144`), and 3 runs at the end (`func_02019100`). The callback
names stay as they are until the arm9 dispatchers are renamed to match, which
needs the usual gates.

## ov004 runtime helpers and globals (renamed 2026-10-14)

These are free functions and data of the shared ov004 runtime
(`src/minigames/d_s_mg_base.cpp`). Each was named from its body, and its body's
comment in that file carries the detail. The `Mg_` prefix marks the minigame
runtime. `Ov004_` marks a thunk that only forwards to an arm9 routine.

**High confidence.** The body says exactly this, and the callers agree.

| Area | Names (old address) |
|---|---|
| Scene singleton | `gMgScene` (data_ov004_020beb68): set by the `dScMgBase_c` constructor, cleared by its destructor. `gMgMenuRow` (arm9 data_0209b308): the 0x34-byte menu row the select menu copies in. Word 2 is the scoring mode, words 5/6 the record defaults, word 11 the music mode. |
| Scores and records | `Mg_SubmitScore` (020adb1c), `Mg_GetSubmittedScore` (020adbc0), `Mg_SetSessionBest` (020ada40), `Mg_GetSessionBest` (020adafc), `Mg_IsNewRecord` (020adbe0), `Mg_ClearNewRecordFlag` (020ada20), `Mg_SetTopRecord`/`Mg_GetTopRecord` (020adc00/020adc1c), `Mg_GetSaveTable` (020adc3c), `Mg_FindRecordRank` (020ad6fc), `Mg_GetLastRecordRank` (020ad6f4), `Mg_RankSessionBest` (020ad940), `Mg_CommitRecords` (020ad90c), `Mg_SaveRecordPair` (020ad79c), `Mg_GetSavedRecord0/1` (020ad8b8/020ad878), `Mg_GetScoringMode` (020ae1a8) |
| Shared state machine | `Mg_SetSharedState` (020b0a54), `Mg_CallStartRound` (020b29a0) |
| Banners | `Mg_ShowBanner` (020b0cac), `gMgBanners` (data_ov004_020bf648, three 0x134-byte records) |
| Effect pool | `sMgFxPool` (0x40 records), `sMgFxUpdateTable`/`sMgFxDrawTable`, `Mg_SpawnFx`, `Mg_InitFx`, `Mg_UpdateFx`, `Mg_DrawFx` |
| Pause menu | `Mg_UpdateMenu` (020aeb24), `Mg_DrawMenu` (020ae858) |
| Drawing | `Mg_DrawSpriteList`, `Mg_DrawSpriteListEntry`, `Mg_CopySpriteList`, `Mg_DrawNumber` (8 px digits), `Mg_DrawBigNumber` (16 px digits), `Mg_DrawNumberRotated`, `Mg_DrawTally`, `Mg_DrawScoreTally`, `Mg_DrawRecordTable`, `Mg_DrawRecordRow`, `Mg_DrawPrompt`, `Mg_DrawLine`, `Mg_InitBrush`, `sMgSpriteTables` (per-language sprite lists) |
| Display | `Mg_ResetBgOffsets`, `Mg_DisableAllBanks`, `Mg_GetTopScreenYOffset`/`Mg_SetTopScreenYOffset` with `sMgTopScreenYOffset`, `Mg_ResetOverlays` |
| Music | `Mg_GetMusicMode`, `Mg_GetMusicTrack`, `Mg_StartMusic`, `Mg_InitMusic`, `Mg_RestoreMusic` |
| Files and memory | `Ov004_LoadFile`, `Ov004_LoadFileByPath`, `sMgWorkBuffer`/`Mg_GetWorkBuffer` (the 0x4000-byte buffer `BeforeInitResources` allocates) |

**Medium confidence.** The mechanics are certain, but the game-facing meaning is not observed yet.

| Name (old) | Why it is not high |
|---|---|
| `Mg_PlayJingle` (020ae274) | It starts sequence `id` on `Sound::PlaySub`'s player and sets the music volume target to 0x40. The callers pass 1, 3 and 4 when a banner goes up. **Seen:** 4 with the round-clear banner (state 0), 3 with GAME OVER (state 18) and 1 with the new HIGH SCORE (state 17). The audio was not heard. |
| `Mg_LoseCredits`, `Mg_GainCredits`, `Mg_SubCredits`, `Mg_AddCredits`, `Mg_DrawCredits`, `Mg_DrawCreditsIcon` | They work on `mCredits`, clamped to 0..9999, which `BeforeBehavior` eases `mCreditsShown` toward for display. Cup gives 2 and loses one per wrong pick before game over; Memory caps the value at 3. A wider "credits" reading is possible in the gambling games. |
| `Mg_ResumeSharedState` (020b0a38) | It clears `mStateController.unk_024`, which lets `BeforeBehavior`/`BeforeRender` step and draw the controller again. What sets the flag (state 6's behaviour) reads as "hand control back to the game". |
| `Mg_CheckBannerButton` (020b0b1c) | It hit-tests a touch against a shown banner whose graphic id is `arg` (0x40 x 0x13 box), or reports the banner's finish. States use ids 0, 1 and 2 as retry/quit buttons. |
| `Mg_DrawBlinkSprite` (020b0d8c) | It alternates sprite words 0x11/0x12 on the 40-frame counter. Which graphic that is has not been seen. |
| `Mg_DrawTopLeftCounter`, `Mg_DrawTopRightCounter`, `Mg_DrawTopCenterCounter`, `Mg_DrawStockCounter` | HUD numbers with a label sprite, named by screen position. The label graphics (sprite words 8, 4, 3 and 0/1) are not identified. |
| `Mg_RenderSub*`, `Mg_RenderMain*`, `Mg_RenderSpriteBoth*`, `Mg_RenderSpriteOn*` | OAM wrappers named by screen and arguments. `OAM::Render`'s first argument is read as main=0 / sub=1 from `RenderOamMainScreen` and `Hud_RenderSprite`; that reading has not been checked on hardware. |

**Low confidence.**
- `Mg_GetLevel` (020b1aec) returns `mLevel`, which the constructor copies from menu-row word 12. The name only describes where the value lives. Jump reads it `% 5`.

## dScMgBase_c fields (renamed 2026-10-14)

The compiler located every use for these: each field was renamed in the header,
and only the lines the compiler then rejected were changed.

| Offset | Old | New | Confidence | Evidence |
|---|---|---|---|---|
| 0x050 | `unk_050` | `mTallyShown` | high | `Mg_DrawTally` keeps the last tally it drew here and compares the new count against it. |
| 0x054 | `unk_054` | `mTallyAnimTimer` | high | It starts at 1 when the tally rises and drives the newest icon's blink and the close-up at five. |
| 0x060 | `unk_060` | `mRecordRevealTimer` | high | `Mg_DrawRecordTable` reveals a row every 0x14 frames as it counts, then loops it over 0x64..0x8b. `Mg_SetSharedState` zeroes it. |
| 0x064 | `unk_064` | `mLastRecordRank` | high | `Mg_RankSessionBest` stores the rank (0..4, or -1); `Mg_GetLastRecordRank` returns it. |
| 0x0a0 | `unk_0a0` | `mSubBgBank` | medium | Only `dScMgD3DBase_c` uses it: it saves `func_02053eb0()` and passes the value to `GX::SetBankForSubBG`. |
| 0x0a4 | `unk_0a4` | `mRunDuringFade` | high | `BeforeBehavior` skips the frame while it is 0 and the fader is busy. 3DEsp, Coin, Curling and Curling2 set it to 1. |
| 0x0a8 | `unk_0a8` | `mCredits` | high (coins seen) | It is clamped to 0..9999 by `Mg_Add/SubCredits`, and 0 sends the shared states to game over. Seen at runtime in Slot3: the coin counter starts at 12, a miss takes 3 coins and a win pays 3 or 6 (runtime-checks.md). Cup uses the same field as lives (2), so "credits" covers both readings. |
| 0x0ac | `unk_0ac` | `mCreditsShown` | high | `BeforeBehavior` eases it toward `mCredits`, and `Mg_DrawCredits` draws it. |
| 0x0b8 | `unk_0b8` | `mBestHudScore` | high | Cup, BSC and Card raise it to `mHudScore` whenever that is higher. `BeforeInitResources` zeroes it. |
| 0x0bc | `unk_0bc` | `mLevel` | medium | The constructor seeds it from menu-row word 12. Wins increment it (capped at 9998) and a loss can reset it. Amida reads it as the level; the level banner (style 12) draws `mLevel + 1`. |
| 0x0c8 | `unk_0c8` | `mMusicTrack` | high | `BeforeInitResources` copies menu-row word 10 into it; `Mg_GetMusicTrack` returns it. |
| 0x462c | `unk_462c` | `mMenuScreenSaved` | high | `OnGraphCallback2` latches `mMenuOpen` into it after saving or restoring the screen. `BeforeBehavior` waits while it is set. |
| 0x4630 | `unk_4630` | `mMenuDisabled` | high | `OpenMenu` does nothing while it is set. `Mg_CheckBannerButton` sets it when a quit button (1 or 2) is touched. |
| 0x4648 | `unk_4648` | `mQuitFadeStarted` | high | The menu's quit item sets it once, after starting the fade and `func_02012dd0(0x3c)`. |
| 0x464c | `unk_464c` | `mSubmittedScore` | high | `Mg_SubmitScore` writes it; `Mg_GetSubmittedScore` returns it. |
| 0x4654 | `unk_4654` | `mSessionBest` | high | `Mg_SetSessionBest` and `Mg_GetSessionBest` write and read it. |
| 0x4658 | `unk_4658` | `mRankedScore` | high | `Mg_RankSessionBest` moves the session best here before clearing it; the record table blinks it. |
| 0x465c | `unk_465c` | `mNewRecord` | high | `Mg_SetSessionBest` sets it on beating the top record; `Mg_IsNewRecord` returns it. |

Left unnamed:
- `unk_0c2`: only the constructor writes it (1).
- The top record at +0x4650 was padding here; pass 2 typed it as `mTopRecord` (below).

## dMgState_c: the shared banner states (renamed 2026-10-14)

Handlers that serve one state are named after that state's `MG_STATE_*` name
(pass 6; they were named by number, `State05_Enter` / `State05_Update`, and
are now `Lose_Enter` / `Lose_Update`). Which handler serves which state is
certain from the enter table in `SetState` and from which behaviour each enter
handler installs; how sure each state's own name is stays with the enum's tags
in `include/dMgState_c.h` (`RoundBanner5`, `LoseResume`, `RoundClearTop`,
`RetryPromptB`, `ScoreThenRound`, `RetryPromptWideB`, `DigitsThenRound` and
`BannerEThenRound` are low). Shared handlers are named by
what they do:
- `UpdateRetryFadeOut`: the retry button's fade-out.
- `UpdateGameOverToMenu`: the Game Over banner, then a fade to the menu (scene 5).
- `UpdateWaitThenResume`
- `UpdateFadeThenResume`
- `UpdateAfterHidden`
- `UpdateRetryPrompt`: states 12/13, which show buttons 0 and 2.
- `DrawRecordTable`, `DrawScore`, `DrawScoreDigits`

What a state means to the player depends on which banner graphic its file index
shows, and that has not been observed. The table records the mechanics (certain)
and a guess (unconfirmed):
- **Banner** is the file index passed to `Mg_ShowBanner`. `*` means a game can
  override it once through `Mg_SetNextBanner`.
- **Jingle** is the `Mg_PlayJingle` id, or an `s` + `Sound::PlayBank2_2D` id.

| State | Enter: banner, jingle, timer | Then | Entered by | Guess |
|---|---|---|---|---|
| 0 | 3\*, 4, 180 | fade, restore music, `StartRound(0)` | a win in Cup, Amida, Teresa, Panel, Sound, Jump | round cleared, play on. **Seen** in Sound, Cup and Teresa: banner 3 is the rainbow "CLEAR" and r1 is the score. A no-miss round first calls `Mg_SetNextBanner(6)`, and banner 6 is "PERFECT" |
| 1 | none, none, 15 | fade, `StartRound(1)` | Hanachan, Luigi | silent next round |
| 2 | 5, 1, 180 | fade, restore music, `StartRound(2)` | none found | |
| 3 | 8\*, 3, 60 | retry (button 0) / quit (1, 2) buttons | Snowball, with banner 0xe | game over, retry? |
| 4 | 9\*, s0x136, 120 | `StartRound(4)` | a win in Memory, Card, BSC, MCarlo, Roulette | you win. **Seen:** clearing Memory's board pays out and deals again |
| 5 | 0xa, s0x137, 120 | game over at 0 credits, else `StartRound(5)` | a loss in the same games | you lose. **Seen** in Memory (bet gone) and Card (Luigi's hand higher), each then `StartRound(5)` |
| 6 | 0xa, s0x137, 120 | hides the controller and hands back to the game | none found | |
| 7 | 0xb, none, 120 | `StartRound(7)` | Card | draw. **Seen:** `CompareHands` = 0 refunds the bet coin, then `StartRound(7)` |
| 8 | none, none, 0 | `StartRound(8)` at once | Roulette's tie | silent tie. **Seen:** a spin with no chip staked (payout 0 = stake 0) returns the rack chips and spins again |
| 9 | 3\* on the top screen, 4, 180 | `StartRound(9)` | none found | |
| 10 | 8 (style 0xb), 3, 180 | retry / quit buttons | none found | |
| 11 | none, 1, 180, draws the score | fade, `StartRound(11)` | none found | |
| 12, 13 | none, none, 10 | retry / quit buttons | Flower (12) | continue? |
| 14 | 5, 1, 180, draws score digits | fade, `StartRound(14)` | none found | |
| 15 | 0xe, 3, 60 | fade, `StartRound(15)` | none found | |
| 16 | 7 at (0x80, 0x14), none, 120 | record table, touch to go on; `Mg_CommitRecords` | the timed games (Bomroom, Coin, Curling, Pachinko, Smartball); Wanted! after game over | results. **Seen:** banner 7 is "HIGH SCORE" |
| 17 | as 16, 300 frames | record table; banner 5, jingle 1 and `OnNewRecord` 120 frames in | 16 or 18/19, when `Mg_IsNewRecord` | results, new record. **Seen** on a new Wanted! high score |
| 18 | 8, 3, 180 | touch or timeout, then 16 or 17 | game over in Cup, Panel, Luigi, Teresa, Sound, Jump, Jump2, Slots, Trampoline/2, Hanachan | game over. **Seen:** banner 8 is "GAME OVER", jingle 3 (Wanted!, Sound, Cup) |
| 19 | 3\*, 4, 180 | touch or timeout, then 16 or 17 | Snowball (banner 4 override) | finished, goal. **Seen:** Snowball's ball crossing the finish line shows "GOAL" (banner 4), jingle 4, then state 17 on a new record |

**Banner ids seen on screen** (`Mg_ShowBanner` r0, melonDS 2026-10-15):

| Id | Graphic | Seen in |
|---|---|---|
| 0, 2 | PLAY AGAIN, QUIT buttons | retry prompts |
| 3 | "CLEAR" | Sound, state 0 |
| 4 | rainbow "GOAL" over the checkered line | Snowball, state 19 (`Mg_SetNextBanner(4)`) |
| 5 | rainbow "HIGH SCORE" | new record (state 17), Pachinko |
| 6 | rainbow "PERFECT" | Sound, a round with no miss (`Mg_SetNextBanner(6)`) |
| 7 | "HIGH SCORE" header over the record table | state 16 |
| 8 | "GAME OVER" | state 18 |
| 9, 0xa, 0xb | win, lose and draw banners | states 4, 5 and 7. Memory places them at y −0x3c, off screen, so they were not seen |
| 0xc | red "START" | Snowball |
| 0xd | "LEVEL n" | every game with levels |
| 0xe | "TIME IS UP" | Snowball, before state 3 |
| 0x10, 0x13 | "LOVES ME", "LOVES ME NOT" | Flower |
| 0x18..0x1a | the poker hand names | Card (Picture Poker) |

**The `MgSharedState` enum** (`include/dMgState_c.h`, 2026-10-15). Every
`Mg_SetSharedState(n)` call now passes a name. Each name describes the state's
mechanics from the table above, and the enum marks the confidence of its
meaning word per entry:
- **high, all seen at runtime** (2026-10-15):
  - `ROUND_CLEAR`, `NEXT_ROUND` and `RETRY_PROMPT`;
  - `WIN`, `LOSE`, `DRAW` and `TIE`;
  - `RETRY_PROMPT_WIDE`;
  - `RESULTS`, `RESULTS_NEW_RECORD` and `GAME_OVER`;
  - `FINISHED` (Snowball's "GOAL").
- **low:** the rest, which no caller enters or one game enters.

The runtime check on `Mg_SetSharedState` settles the medium and low words. A
corrected name is a plain enumerator rename; the values are fixed.

Medium-confidence helpers:
- `Mg_SetNextBanner` (020b67e8) sets the one-shot override `sMgNextBanner` (0x1d means none).
- `Mg_SetPerfectBanner` (020b67f8) is `Mg_SetNextBanner(6)`. Memory calls it before state 4 when there were no misses.
- `Mg_ResetBannerLayout` (020b8a8c) seeds every state's banner x/y.
- `Mg_PlaceBannersOnTopScreen` (020b682c) moves several of them by the top-screen offset.
- `Mg_ResetRecordBannerPos` (020b6808).
- `sMgBannerButtonsArmed` is set by the layout reset and cleared by `Banner_Press`; button touches are ignored while it is 0.

## The credit coins (unit020b4aa4) and the options panel (renamed 2026-10-14)

**The coins, medium confidence.** `unit020b4aa4` animates up to twenty coin icons, drawn
with the credits icon (`Mg_DrawCreditsIcon`), between the credits counter at
(12, 12) and the top screen. Each sequence moves `mCredits` one coin at a time:
- `Coin_Launch` and `Coin_Spend` take a credit.
- `Coin_MoveThenBank`, `Coin_MoveThenRefund` and `Coin_FlyToCounter` give one back.

The entry points are:
- `MgCoins_Reset`, `MgCoins_Update` and `MgCoins_Draw`.
- **Bets.** `MgCoins_AddBetCoin` runs on a touch in the top-left box, which `MgCoins_Draw` marks with a button. `MgCoins_PlaceBet`, `MgCoins_RaiseBet`.
- **Losses.** `MgCoins_StartLose` and `MgCoins_StartLoseOneMore` send coins away, up from x 128.
- **Spending.** `MgCoins_StartSpend`.
- **Refunds.** `MgCoins_StartRefund` returns coins to the counter.
- **Payouts.** `MgCoins_StartPayout` lays coins out five wide and flies them to the counter, then submits `mCredits` as the score in scoring mode 2.

Games watch `gMgCoinsStatus`: 1 when a sequence is done, 2-4 while one runs.
`gMgCoinsShowDelta` makes the sequences draw their signed count.

**"Bet" is right; seen 2026-10-15.**
- **Memory.** `MgCoins_PlaceBet(3)` stakes three coins on the table. Each miss slides one coin over to Luigi, and clearing the board pays the coins still on the table ×6. The misses allowed are the stake.
- **Card (Picture Poker).** The BET! button runs `MgCoins_AddBetCoin` and adds one coin to the stake.
- **Memory_J.** `dScMgMemory2_c::OnDealDone` stakes five coins each deal (`MgCoins_PlaceBet(5)`). A lost round forfeits them, and with the purse empty the game ends at `UpdateGameOverToMenu`.
- **Coin moves.** Each staked coin is one `Coin_Launch` → `Mg_LoseCredits(1)` → `Mg_SubCredits(1)`. Each paid or refunded coin is one `Mg_GainCredits(1)` → `Mg_AddCredits(1)`.

**The options panel, high confidence.** Pass 2 (2026-10-15) made the
`dMgPsOpt_c` helpers real members: `dMgPsOpt_c::Open`, `Close`, `Update`, `Draw`
and `IsIdle` (formerly `PsOpt_*` on a `char *` view). `PsOpt_PlaceIcon` stays a
free function that takes the icon.

## dScMiniGm_c, the select menu (ov005, renamed 2026-10-14)

High confidence, from the bodies:
- **Drawing and loading:** `MiniGm_LoadGroupIcons` (the nine icons of the current group), `MiniGm_LoadIconPalette`, `MiniGm_DrawRecords` (the record under each icon, by scoring type), `MiniGm_DrawNumber`, `MiniGm_DrawNewBadges`.
- **Input:** `MiniGm_UpdateGroupTabs` (the four tabs down the left), `MiniGm_UpdatePageArrows`, `MiniGm_UpdateExitButton`, `MiniGm_UpdatePageScroll`, `dScMiniGm_c::UpdateGameSelect` (a touch on an icon copies its row into `gMgMenuRow` and fades to its scene).
- **Records and saving:** `MiniGm_HasSecondPage`, `MiniGm_IsUnlocked`, `MiniGm_GetSaveTable`, `MiniGm_GetRecord`/`MiniGm_SetRecord`, `MiniGm_SaveIfDirty`.
- **Tables:** `sMiniGmRows` (the 36-row menu table), `sMiniGmGroupBases`, and the icon, badge, digit and arrow tables.
- **arm9 globals:** `gMgGroupBase`, `gMgLastSelected`, `gMgMenuPage`, and `gMgSaveDirty` (set whenever a record changes; the menu saves while it is set).

Medium confidence:
- `MiniGm_IsNew` and `MiniGm_ClearNew` wrap a save flag. The menu draws a blinking badge over an unlocked game while it is set, and leaving a game clears it.
- "New" is the reading; a "played" or "cleared" flag would look the same. Check by watching a never-played game's icon.

## unit020bfec0: shared 3D components (renamed 2026-10-14)

This unit holds the 3D helper objects the Single3D games embed. None has RTTI, so
each one is named after the games that construct it and what it draws:

| Prefix | Built by | What it is | Confidence |
|---|---|---|---|
| `Camera_` | all | `Camera_UpdateMatrices` (already named), screen projection, and four eye/target presets: `Camera_InitFlowerView`, `Camera_InitPresenterView`, `Camera_InitSlotsView`, `Camera_InitDealerView` | high |
| `FlowerFace_`, `FallingPetal_` | Flower | The face that watches the player pull petals. It has idle and two reaction animations, and up to 22 falling petals with gravity and spin. | high |
| `Presenter_` | Cup, Sound | A character with 8 body, 8 texture-sequence and 8 texture-transform animations, whose head follows the stylus (`Presenter_TrackStylus`). | high (structure) |
| `Dealer_` | Card, BSC, MCarlo/2, Memory/2, Roulette | The card dealer, Luigi (`MG/luigi_d.bmd`): shuffle, deal, reveal, swap, win and lose animations, plus the table lamps that cycle colours. | high (pass 7) |
| `Mascot_`, `MascotWalker_` | Slots; inside `Dealer_` | One small character model (files 0x205/0x206). Slots shows it turning its head toward a target. Behind the dealer it walks a waypoint path and pauses at random. | medium |

The `Presenter_StateAnimN` names give the animation slot index (N = 0..7), which
is certain. **The four `Presenter_Play*` entry points were renamed by what they
show (2026-10-15, seen in Sound; high):**

| Slot | Old name | New name | What it shows | Cup calls it for |
|---|---|---|---|---|
| 1 | `Presenter_PlayAnim1` | `Presenter_PlayMiss` | Mario puzzled, one eye squinting | a wrong pick with lives left |
| 2 | `Presenter_PlayAnim2` | `Presenter_PlayMatch` | an open-mouthed grin and a nod, on each matched pair, with sound 0x1c | (not used) |
| 4 | `Presenter_PlayAnim4` | `Presenter_PlayLose` | Mario laughing and winking | the wrong pick that ends the game |
| 7 | `Presenter_PlayAnim7`, `...7Loop` | `Presenter_PlayCheer`, `Presenter_PlayCheerLoop` | Mario laughing, eyes shut, with sound 3 | a right pick and `OnNewRecord` |

All 45 minigame objects stayed identical modulo the renamed symbols.

A second watch of Sound and Cup saw the same four calls at the same moments. It
caught slot 1 as a wince, with the head flipping up off the screen and dropping
back, and slot 4 as a tilted head with one eye shut and a lopsided grin. The two
watches caught different frames; the names hold.

The dealer's shuffle, deal, reveal and swap names follow the animation files
each state plays (pass 7: the dealer is Luigi, `MG/luigi_d_*.bca`), high
confidence.

## Per-game helpers (ov006)

Each game's helpers are named from their bodies and their pointer-to-member state
tables. The tables are resolved through the relocations of the records each
`__sinit` copies, which makes a state handler's index certain. A name is high
confidence unless listed below the game.

**Sound (`dScMgSound_c`).** This is the note-pair memory game. A touched note plays its
pitch, two matching notes clear, and a mismatch closes them again.
- **Notes:** `DealNotes`, `ResetRound`, `UpdateNotes`, `CheckPair`, `CheckRoundOver`, and `RevealUnmatched` on a loss.
- **Note states 0-4:** `Note_StateWait`, `Note_StateOpening`, `Note_StateOpen`, `Note_StateClosing`, `Note_StateScroll`.
- **Pops:** `SpawnPops` launches the burst sprites, and each of the 13 pop kinds has `PopNN_Update`, `PopNN_Start`, `PopNN_Fly` and `PopNN_Finish`. The kind number is the index in the dispatch table; the look of each kind is not observed.
- **Win and loss:** the win plays `Presenter_PlayCheer` (seen) and the two win sprites (`Start/Update/DrawWinSpriteA/B`: the sparkle ring round "CLEAR" and the star at the top right, seen); the loss plays `Presenter_PlayLose` (seen). Each match plays `Presenter_PlayMatch` and each miss `Presenter_PlayMiss`. Sound's three hearts are `mTries` and move no credits.

**Teresa (`dScMgTeresa_c`, Hide and Boo Seek).**

The four state tables' handlers are named from their bodies (pass 6; they were `BigBoo_State0..8`, `Reveal_State0..4`, `Boo_State0..7` and `Scene_State0..4`), high confidence unless noted:
- **The big Boo (top screen):** `BigBoo_StateDropIn` (0: falls in, then the START banner 0xc), `StateHover` (1: waits 0x30 frames, then picks a side), `StateFlyLeft` / `StateFlyRight` (2, 3: crosses the screen), `StateOffscreen` (4: waits 8 frames, then crosses back), `StateExit` (5: leaves once the round is done), `StateRemove` (6: hides and reloads OBJ graphics from file 0x103), `StateGone` (7, empty), `StateResult` (8: `BigBoo_ShowResult` drops it in with the win or lose frames).
- **The reveal:** `Reveal_StateStart`, `StateSlideIn` (slides to x 0x80, sound 0x1f3), `StateAnimate` (eight frames, sound 0x1f9), `StateDipAndRise` (sinks, then rises with the marker), `StateFinish` (hides the marker, starts the play state, sets the Boos hiding and starts the big Boo).
- **Each Boo:** `Boo_StateAppear` (speed by kind), `StateFly`, `StateFadeOut` (fades into the dark), `StateHidden` (moves, turns at rubbed tiles, touch-tested), `StateHiddenTimed` (medium: hidden and touch-tested but not turned by rubbing, until a timer returns it to `StateHidden`; no setter found, the settle check treats it like `StateHidden`), `StateFound` (vanishes with a puff once the scene reaches its result), `StateFadeIn` (the round end shows the Boos left), `StateFadeAway`.
- **The scene:** `Scene_StateInit`, `StateReveal`, `StatePlay`, `StateRoundEnd` (found numbers and the BG3 scroll), `StateResult` (`MG_STATE_ROUND_CLEAR` or the GAME OVER banner).

Descriptive names, high confidence:
- **Boos:** `SpawnBoos`, `UpdateBoos`, `StepBoo`, `BounceBooOffWalls`, `MoveBoo`, `DrawBoos`.
- **The rub mechanic:** `UpdateRubbing` / `ReadRubTouch` (stylus lines drawn into BG0), `TouchTestBoo`, `CountRubbedTiles`.
- **The clock:** `StartCountdown` (3 seconds, or 2 from level 15), `UpdateCountdown`, `DrawCountdown`.
- **Round flow:** `CheckRoundEnd`, `CheckRevealDone`, `ResetRound`.

The three popup pools over found Boos, seen in play (2026-10-15, runtime-checks.md):
- `Exclaim_*` (+0x48a0): the red "!" the rub test spawns 0x18 px above the Boo it finds.
- `FoundNumber_*` (+0x4a60, was `RisingIcon_*`): the digit that rises off a found Boo, 1 for the first, 2 for the second. Its sprite is the Boo's found order + 1, its delay is staggered by that order, and it plays sound 0x1bc.
- `VanishPuff_*` (+0x4960, was `FoundMark_*`): the five-frame puff of smoke where a caught Boo vanishes, while Luigi raises his flashlight on the top screen.

Medium confidence: `Marker_*` (one sprite) and `StepBg3Scroll`.

`BigBoo_SetDone` (was `func_ov006_0211cc90`, pass 5): sets the big Boo's `done` byte (+0x19 of its record at 0x4bcc, so +0x4be5) when its `active` byte (+0x14, +0x4be0) is set; both round endings in `CheckRoundEnd` call it. Setting it sends the Boo out instead of turning it around. **Seen** once, as the clock ran out (lr 0x0211e69c), 5 s before GAME OVER.

**Bomroom (`dScMgBomroom_c`, Bob-omb sorting).**

The seven member-pointer tables' handlers are named from their bodies (pass 6; they were `Round_State0..4`, `Round_SubState0..3`, `Bomb_State0..6` and `Bomb_Roam0..4`, and three are the fixtures below), high confidence:
- **Scene states (`mSceneState`):** `Round_StateSetup` (0: shows the fixtures), `Round_StateStart` (1: enables the prompt), `Round_StatePlay` (2: spawns and moves bombs), `Round_StateEnd` (3: runs the round-end steps below and the fixtures), `Round_StateGameOver` (4: waits, then `MG_STATE_RESULTS`).
- **Round-end steps (`mSubState`):** `RoundEnd_StartMarch` (0: sends the finished pen's bombs, or after a blast every bomb, into the march, and opens their pen gates), `RoundEnd_WaitLinedUp` (1: waits until every marching bomb is lined up, then closes the top door), `RoundEnd_Tally` (2: counts the lined-up bombs one by one, sound 0x1bc), `RoundEnd_Finish` (3: removes them, submits the score, and goes back to play, or to state 4 after a blast).
- **Bomb states (`state`):** `Bomb_StateWaiting` (0: its spawn delay), `Bomb_StateWalk` (1: walks while its fuse burns), `Bomb_StateHeld` (2: follows the stylus), `Bomb_StateChainDelay` (3: set off by another bomb's blast, 0x20 frames later), `Bomb_StateExploding` (4), `Bomb_StateInPen` (5: bounces around inside its pen), `Bomb_StateMarch` (6: runs the march steps).
- **March steps (`substate`):** `BombMarch_ToGate` (walks to its pen's outer gate), `BombMarch_Exit` (walks off the screen's edge), `BombMarch_Offscreen` (empty), `BombMarch_ToLine` (after its delay, walks to its line-up slot on the top screen; sound 0x1d9 or 0x1da on arrival), `BombMarch_LinedUp` (empty).

Descriptive names, high confidence:
- **Bombs:** `SpawnBombs`, `UpdateBombs`, `DrawBombs`, `ClearBombs`, `Bomb_Animate`, `Bomb_CheckGrab`, `Bomb_CheckSorted`, `Bomb_CheckWrongPen`, `Bomb_PushApart`, `OnBombExploded`, `HideExplodedBombs`.
- **Round flow:** `CountSortedBombs`, `CheckColorComplete`, `ResetRound`, `UpdateBg2Lift`.

Medium confidence:
- `Marker_*`: the two sprites at +0x62b0. `HatchPanel_Activate` shows marker 0; the right panel shows marker 1 at frame 3. Neither was on screen during the stub check.
- `DrawRemaining`.

**The room's fixtures (pass 5, seen in melonDS 2026-10-16), high confidence.** These were the header's `SlotA_*`, `SlotB_*` and `SlotB2_*`. Each draw was stubbed in turn with POKE during a game (runtime-checks.md), and the state names come from the bodies:
- **`HatchPanel_*` (+0x6260, two):** the green panels on either side of the bomb hatch at the top of the bottom screen. `HatchPanel_Activate(side)` (was `SlotA_Trigger`) runs as the spawner sends a bomb out towards that side. It plays sound 0x1e2 and steps the panel's frame 0 to 4 (`StateAnimate`), then holds it (`StateHeld`) until the round resets. No caller sets `StateRevert`.
- **`PenGate_*` (+0x6280, two, indexed by bomb colour):** the green gates at the outer edge of each pen. `PenGate_Open(color)` (was `SlotB_Hit`) runs on each frame a sorted bomb walks out on that side, and at the round-end blasts. It opens the gate over 4 frames, holds it for 0x20 frames (each further call re-arms the hold), then closes it. Opening a shut gate also sets BG0's offset to (0x100, 0).
- **`TopDoor_*` (+0x62a0, one):** the emblem door at the top centre of the top screen. `TopDoor_Open` (was `SlotB2_Start`, sound 0x1e2) runs as the round end marches the lined-up bombs out. `TopDoor_Close` (was `SlotB2_Finish`, sound 0x1e3) runs once none is left marching.
- The header splits the old `mSlotsB[3]` into `mPenGates[2]` and `mTopDoor`, and renames `mSlotsA` to `mHatchPanels`. The dispatch tables are `sBomroomHatchPanelStateTable`, `sBomroomPenGateStateTable` and `sBomroomTopDoorStateTable`.

`AnyBombMarching` (was `func_ov006_020d8c88`, pass 5): returns 1 while any live bomb is in `Bomb_StateMarch`, the march out of a pen. When a pen's colour completes (or a pen blows), `RoundEnd_StartMarch` puts its bombs in `Bomb_StateMarch`. Each walks to a slot in rows on the top screen (sub-state 4 on arrival, sound 0x1d9/0x1da by colour). When `RoundEnd_Tally`'s timer ends, this check starts the lift (`mBg2State = 1`, sound 0x1dd), and the bombs are then retired one every four frames (sound 0x1bc). **Seen** once (lr 0x020d8b50), 3 s before state 16.

**Pachinko (`dScMgPachinko_c`, Bob-omb Squad).** These helpers are C-linkage free
functions on a `char *` view of the scene, so each name carries the `Pachinko`
prefix.

The five state tables' handlers are named from their bodies (pass 6; they were `PachinkoBg_State0..3`, `PachinkoFlyer_State0..4`, `PachinkoPipe_State0..3`, `PachinkoBall_State0..6` and `PachinkoBobOmb_State0..4`):
- **The board scroll (BG2):** `PachinkoBg_StateWaiting` (its delay), `StateRiseIn`, `StateScroll` (left until x -0x90), `StateSinkOut` (then hides BG2 and the deco and waits 0x258 frames again).
- **The flyer (Lakitu, seen):** `PachinkoFlyer_StateWaiting` (delay, then sound 0x188 and a random side), `StateDescend`, `StateCross` (looped sound 0x189; at the far edge it turns), `StatePause`, `StateLeave`.
- **The pipes:** `PachinkoPipe_StatePick` (rolls rise or sink), `StateRise`, `StateSink`, `StateIdle` (an eight-frame loop that counts laps).
- **The enemies:** `PachinkoBall_StateWaiting`, `StateFall`, `StateSpinAway` (spirals off screen with sound 0x187), `StatePop` (a burst and sound 0x18b, then removed). Medium: `StateDrift` (also picks a sideways speed), `StateDive` (falls faster as the wave rises and faces its velocity), `StateGlide` (drifts and turns to face down); the three move differently, and the names say how.
- **The slingshot Bob-omb:** `PachinkoBobOmb_StateWaitGrab`, `StateAim` (drag and release), `StateFlying`, `StateFallAway`, `StateRemove`.

The other subsystems are named by what they draw and update:
- `PachinkoTarget_*` (4 targets)
- `PachinkoFx_*` (30 effects)
- `PachinkoPopup_*` (score numbers)
- `PachinkoDeco_*` (3 records that scroll with the board)
- the aim line on sub BG2 (`Pachinko_DrawAimLine`, `Pachinko_ClearAimLine`, `Pachinko_PlotSubBg2Pixels`)

Medium confidence: "Ball" is a role word from code alone. The others are seen
(runtime-checks.md): the flyer is Lakitu, the board scroll (`PachinkoBg_*`) is the
airship that drops the Para-Bombs, and `PachinkoDeco_*` are its wings.

`Pachinko_UpdateNextBall` (was `func_ov006_020fea70`, pass 5): runs every frame of play and counts down `mNextBallDelay`, the same shape as Pachinko2's `UpdateNextBall`. When the delay runs out, it puts a new shot in the first free `mShot` slot at (0x80, 0x28) and plays 0x18d (silently the first time). **Seen:** that point is the slingshot's cradle. The ball sits there until fired, and the sling is empty right after a shot.

**Pachinko2 (`dScMgPachinko2_c`, Tamaire).** These are members, named after the
header's own `mCups` / `mBalls` / `mPaddles`.

The three tables' handlers are named from their bodies (pass 6; they were `Cup_State0..4`, `Ball_State0..7` and `Paddle_State0..14`):
- **Cups:** `Cup_StateInit`, `StateWait` (animates; aims through `Cup_PickTarget` until armed), `StateDecide` (stays or hops a random number of times, never the same choice three times running), `StateHopLeft` / `StateHopRight`.
- **Balls:** `Ball_StateWaitGrab`, `StateAim` (follows the stylus; release launches it towards (0x80, 0x20)), `StateFlying`, `StateTumble` (falls spinning; off the field it is removed, on the floor it bounces), `StateHop`, `StateWalkOff` (walks off its nearer side), `StateRemove`, `StateScored` (falls with a score popup and one point).
- **Paddles (the flying targets, looped sound 0x198):** `Paddle_StateSpawn`, `StateDescend` (to its lane height, then the movement that `mLaneConfig` picks): `StateSweep` (side to side, with `StatePause` at each end), `StateBoxLoop` (round a rectangle), `StateOrbit` (a circle of radius 0x50), `StateWeave`, `StateCross` (the two paddles cross vertically). After four hits (`Paddle_Hit`) one `StateKnockedDown` (sinks 0x60) or `StateFallToGround` then `StateSwing`, and `StateRiseHome` restores its saved movement. At the end `Paddle_Stop` sends them to `StateGather` (to the centre), then `StateDropBalls` releases the balls they hold, and `StateFlyAway` takes them off the top. Medium: the movement names describe the paths, not observed looks.

High confidence:
- **The aim line:** `DrawAimLine`, `ClearAimLine`, `PlotSubBg2Pixels`.
- **The clock:** `StartCountdown`, `UpdateCountdown`, `DrawCountdown`, `CheckCountdownEnd`.
- **The round:** `CheckRoundOver`, `StartRoundEnd`, `UpdateNextBall` (pens the next ball once `mNextBallDelay` runs out; was `UpdateRoundEnd`), `ResetRound`.
- **Ball hits:** `Ball_HitAt`, `Ball_Bounce`.

Medium confidence: `Popup_*`, `Spark_*`, `CupSlot_*` and `ChooseLaneConfig`, named from the
record they touch; their looks are not observed.

**Panel (`dScMgPanel_c`, Puzzle Panic).**

The six tables' handlers are named from their bodies and the scene's transitions (pass 6; they were `Scene_State0..7`, `Shake_State0..2`, `Banner_State0..2`, `IntroCard_State0..4`, `Input_State0..3` and `Panel_State0..3`):
- **Scene (`mState`):** `Scene_StateInit`, `StateIntro`, `StatePlay`; once the flips are used up (`StartSolveCheck`), with a try left, `StateShowMisses` (the wrong panels blink) and `StateRewind` (`UpdateMoves` undoes the moves, back to play), otherwise `StateResult`; `StateShowAnswer` (the answer demonstration below); `StateResult` (solved: `MG_STATE_ROUND_CLEAR`, else the panels fall); `StateGameOver` (`MG_STATE_GAME_OVER` once they are down). Medium: `ShowMisses` reads from `CountMatches`, which marks the panels it blinks.
- **The answer demonstration (was `Input_*`, now `Answer_*`):** `Answer_StateWaitButton` (a touch on the button at (0xe8, 0x10)), `StateResetBoard`, `StateFindSolution` (from the puzzle tables, the alternate set when `IsAltVariant`), `StateFlipSolution` (flips the answer cells one by one); `Answer_Update` runs them.
- **Each panel:** `Panel_StateIdle`, `StateFlip`, `StateBlink`, `StateFall` (drops off the bottom and hands the fall to the panel below).
- **Shake (sub BG0):** `Shake_StateIdle`, `StateShake`, `StateLoseTry` (shakes, plays 0x12f and takes a try).
- **Banner:** `Banner_StatePopUp`, `StateSettle`, `StateIdle`.
- **The intro card (`mSlide`):** `IntroCard_StateStart`, `StateSlideIn`, `StateHold` (0x80 frames), `StateSlideOut` (then shows the panels and the prompt), `StateDone`.

High confidence: `FlipAt` (a panel and its neighbours), `DealPuzzle`, `CheckSolved`, `CountMatches`, `UpdateTouch`, `DrawPanels`, `Particle_*`, `Scroll_*`, `ResetRound`.

Medium confidence: `ScramblePanels`, `HasDuplicateRows`, `UpdateMoves`, `StartSolveCheck`, `EnableOverlayLayer`, `DrawCursor`, the slide-delay helpers, `CountFlipping`.

**Coin (`dScMgCoin_c`, Coincentration).**

The five tables' handlers are named from their bodies (pass 6; they were `Phase_State0..5`, `Block_State0..5`, `Coin_State0..2`, `Caption_State0..4` and `Bag_State0..3`), high confidence:
- **Phases (`mPhase`):** `Phase_StateIdle`, `StateIntro` (blocks drop in, prompt on), `StatePlay`, `StateSettle` (waits until no block or coin moves), `StateScoreDelay`, `StateResult` (the result countdown, then `MG_STATE_RESULTS`).
- **Blocks:** `Block_StateWaiting` (drop delay), `StateHopDown` (hops down the stages), `StateWander` (along its row until it claims a coin), `StateDropOnCoin`, `StateResting`, `StateFallIn` (falls into place, landing sound, bumps the total).
- **Coins:** `Coin_StateWaitTouch`, `StateHop` (hops once per point of value, kicking the block under it), `StateSettled`.
- **Caption:** `Caption_StateDelay` (chime), `StateCountIn` (releases the moneybag on frame 2, phase + 1 at the end), `StateBlink`, `StateHidden`, `StateLoop`.
- **The moneybag:** `Bag_StateIdle`, `StateRise` (kicked up off the top), `StateParked` (spawns the blocks), `StateBounceDown` (bounces down the stages and off the bottom).

High confidence: `LayOutCoins`, `SpawnBlocks`, `KickBlockUnderCoin`, `Block_ClampToWalls`, `Sparkle_*`, `Popup_*`, `ResetRound`.

Medium confidence: `DrawScorePlayer` and `DrawScoreRival` (the two result scores), `Caption_*`, `Bag_Release`.

**Hanachan (`dScMgHanachan_c`, Which Wiggler?).**

High confidence:
- **Phases:** `Phase_SetupRound`, `Phase_WaitIntro`, `Phase_Play`, `Phase_Result`, `Phase_WaitTouch`, `BeginPlay` (a 1200-frame clock), `BeginResult`.
- **Wigglers:** `Hana_SpawnWigglers`, `Hana_ChooseStarWiggler`, `Wiggler_HitTest` (any segment within 0x14 px), `Wiggler_ClampToScreen`, `Wiggler_Move`.
- **Popups:** the single star popup (`StarPopup_*`) and the sixteen `HanaPopup_*`.

Medium confidence:
- The movement modes: `Wiggler_StartPathA/B/C`, `Wiggler_StepSegmentsA/B/C`, `Wiggler_StartBounce`/`Wiggler_MoveBounce`, `Wiggler_StateMoveA/B`, `Wiggler_StateHold`, `Wiggler_StateWait`, `Wiggler_Retarget`, `Wiggler_OnTouched`, `Wiggler_StopIfIdle`, `Wiggler_UpdateSway` (sounds 0x1e7/0x1e8 as the sway crosses zero).
- `Wiggler_DrawTarget` and `Wiggler_DrawField` (were `Wiggler_DrawPassA/B`): **high, seen**. With each stubbed to `bx lr` in RAM, A's absence removed the target Wiggler on the top screen and B's removed every Wiggler on the bottom play field; the scene calls A with `mTarget` and B for each entry of the field array. A runs on the main engine once a frame, B on the sub engine once per wiggler.
- `Wiggler_IsStarVisible`.

**unit020cd744: the stylus-drawn trampolines.**

- **`TrmpLine_*`, high confidence.** The four line slots at `sTrmpLines`, each a 4x4 vertex patch, used by Trampoline and Trampoline2.
  - **Building:** `TrmpLine_Create` (from a stylus start and end point), `TrmpLine_Build`, `TrmpLine_ComputeNormals`.
  - **Drawing:** `TrmpLine_Draw`/`DrawAll`.
  - **Behaviour:** the three states `StateFade`, `StateSettle` and `StateFlex`, each with its setter; `TrmpLine_PushJumper` applies the line's spring to the jumper.
  - **Setup:** `TrmpLine_CreateStartLineT1/T2`, the line each game starts with.
- **`TrmpItem_*`.** The bouncing items of Trampoline2; the layout is in `include/TrmpItem.h` (pass 5).
  - The nine motions, indexed by the item's kind byte (+0x98, `mMotionKind`), are named from their code (pass 5): `MotionStill`, `MotionPatrol` (moves along x, reversing at a size-dependent range), `MotionBob` (moves along y between two heights), `MotionSpin` (turns its facing 0x100 a frame), `MotionSway` (x speed flips with a phase), `MotionSpinSway`, `MotionHover` (y speed flips with a faster phase), `MotionSpinHover` and `MotionSpinDrift` (spins like MotionSpin but keeps its vertical speed). Each has its `SetMotion*`.
  - **Seen** (pass 4: each kind forced, then tracked; runtime-checks.md): 0 still, 1 slides side to side, 2 rises and falls, 3 spins in place, 4 sways on a timer, 5 sways and spins, 6 bobs on a timer, 7 bobs and spins, 8 spins in place on fixed axes. The items are rainbow rings.
  - In `dScMgTrampoline2_c::SpawnWave`, the local `faces[i]` holds the motion kind and `kinds[i]` the position index, so the locals and `Trmp2_PickFaceA/B` / `Trmp2_ColumnForKind` read backwards. Renaming them is a candidate for the next pass. Read that way, the dealer is: `Trmp2_ColumnForKind` maps a position (0..10) to one of five columns. `Trmp2_PickFaceA` picks a motion kind, sway (4/5) for positions below 5 and bob (6/7) above. `Trmp2_PickFaceB` can also give 8, spin in place. `Trmp2_OccupancyBits` marks the neighbouring columns a placed ring blocks. Candidate names: `PickMotionA/B`, `ColumnForPosition` and `BlockedColumns`. `SpawnWave` never ran in natural play, so this is from code.

**Curling (`dScMgCurling_c`).** Named mostly from the bodies' own comments, high confidence.
- **Scene:** `Scene_StartRound`, `Scene_Play`, `Scene_Score`, `Scene_End`, `Scene_Idle`.
- **Stones:** `Stone_WaitRelease`, `Stone_Slide`, `Stone_Collide`, `Stone_Drag`, `SeparateStones`, `ResolveStoneCollision`, `SetStoneSpin`, `BringOutNextStone`, `ClearStones`, `ScoreStones`.
- **Aim:** `Aim_WaitTouch` and `Aim_Swing`.
- **Houses:** `PickHouse` and `ResetHouse`.
- **The falling bits (50 records):** `Bit_PickKind`, `Bit_RunKindA/B`, and the states named from their bodies in pass 6 (they were `Bit_State0..2`, `BitA_State0..3` and `BitB_State0..3`). The fall speed runs `Bit_StateFallStart` (picks a top speed), `StateFallSpeedUp` and `StateFallSlowDown`. The sideways sway runs `BitA_StatePause` (a random wait, then a direction), `StateDriftRight`, `StateDriftLeft` and `StateDriftStop`, up to 0x300; `BitB_*` is the same up to 0x400. In both `DriftLeft` handlers, reaching full leftward speed stores the full rightward speed (`vx = 0x300` / `0x400`); the bytes require it, and Snowball's copy does the same. What the bits are (snow?) is not observed.

**Roulette (`dScMgRoulette_c`, Mushroom Roulette).** These are C-linkage free helpers with a `Roulette` prefix.
- **`RouletteRacer_*`, high confidence:** the racers dealt onto the board, how they move to a tile, and the payout.
- **`RouletteBoard_*`, high confidence:** the spinning board. Its two states are confirmed by the records their setters store (`RouletteBoard_StateSpin`, `RouletteBoard_StateSlow`).
- **`RouletteStrip_*`, `RouletteResults_*`, medium confidence:** the per-player result strips.

**Snowball (`dScMgSnowball_c`).**

High confidence:
- **The ball:** `UpdateSnowball`, which runs 32 probes per frame through `HitsWall`, `HitsBreakable` and `IsInWater`.
- **The course:** `BuildCourse`, `DrawCourseColumn`, `RedrawCourse`.
- **Camera and reset:** `Snowball_UpdateCamera`, `ResetScene`.
- `dScMgSnowball_c_Construct`: the out-of-line constructor themes.md describes (it is not yet a real C1).

Medium confidence:
- `SnowSpray_*`: the 32 moving records at +0xba14.
- The falling-bit machine, which has the same shape as Curling's and the same names (`Bit_*`, `BitA/B_State*`; pass 6 named the states).

**Cup, Jump2, Slots, Trampoline2 (one batch).**

- **Cup.** High confidence: `CupFx_*` (the 32-sprite effect pool), `Cup_DrawOamList`, `Cup_UpdateAnim`, `Cup_PlayAnimSound`. Medium confidence: `Cup_PickShuffleSpeed`, which reads the round row's speed nibbles. **Seen:** 8 calls per round in two rounds, all from `StatePrepareShuffle` (lr 0x020dfd0c), one per swap. Besides the speed, it picks which of the three pairs swaps and sets the pair's midpoint, half-distance and start angle, plus a fake-out from row 7 up. Candidate name: `Cup_SetupSwap`.
- **Jump2.**
  - A linked list of score popups: `Jump2Popup_*`, plus the members `Jump2Popup::Spawn/Tick/DrawNumber` and `Jump2PopupLink::Link/Unlink` (were `Elem` and `Node`). **Seen:** a yellow number, the running score, rising from where a Shy Guy was knocked out (`Heyho_Settle`, lr 0x020c4e58). Trampoline2 also clears the list at setup (lr 0x02124360), so the `Jump2` prefix is narrower than its use.
  - Phases: `BeginIntro`, `State_Intro`, `BeginPlay`, `State_Play`, `BeginEnd`, `State_End`, `State_Result`, `State_Idle`. These are medium confidence, from the PMF records the transitions store. **Seen**, in both Jump games: Intro about 3 s under "A RULES", Play, End about 0.5 s (after the third miss), Result 2-3 s (until `Heyho_AllIdle`), then state 0x12 and Idle.
- **Slots.**
  - Slot3's state table is named from its bodies (pass 6; it was `Slot3_State0..7`, with `Slot3_State3and4` shared by states 3 and 4): `Slot3_StateIntro` (shows the target face and the LEVEL banner), `StateSpin` (a touch in a row's band stops it), `StateJudge` (compares the rows against the target and scores), `StatePayout` (states 3 and 4: pays the win coin by coin, or takes three coins for a miss; seen), `StateWait` (5, empty), `StateGameOver` (slides the reels apart, then `MG_STATE_GAME_OVER`), `StateDone` (7, empty).
  - `SlotLamp_*` and `SlotBlink_*`: the blink helpers the file's own comments describe.
  - Medium confidence: `SlotBet_*`, `SlotReel_SetSpeed`, `SlotReels_CheckStopped`, `Slot1_InitHud`.
  - **Seen** (Slot1, one 3-coin spin that hit three feathers, ×5):
    - `SlotBet_Place` and `Slot1_InitHud` run at setup. `InitHud` loads `mCredits` from the saved record. `SlotBet_Reset` follows, and each BET touch is a `SlotBet_OnTouch` (one coin, up to 3).
    - `SlotReels_CheckStopped` ran once, as the third reel stopped (lr 0x0210cd4c). It is the payline check: symbol 5 is wild, and it returns 1 when the other symbols agree. Candidate name: `SlotReels_CheckLine`.
    - `SlotReel_SetSpeed` is called on the bet icon, not a reel (lr 0x0210ceec, r1 5, the ×5 line). It stores bet × multiplier × 4 at icon +0x20, and the credits rose by bet × 5 (7 → 22). Candidate name: `SlotBet_SetPayout`.
- **Trampoline2.** All medium confidence:
  - `Trmp2_ColumnForKind`, `Trmp2_OccupancyBits` and `Trmp2_PickFaceA/B`: the face-row dealer.
  - `Trmp2Spinner_*`: a spinning decoration pool. **Seen:** the reward star. `TrmpItem_UpdateWithJumper` spawns one (via `Trmp2Spinner_InitAll`, which fills one free slot) each time Mario takes a ring, sized by the combo count. Three slots written live showed large white stars spinning off the top screen to the lower left within a second. The background shooting stars are something else: they kept moving with `Trmp2Spinner_UpdateAll` stubbed. Candidate names: `Trmp2Star_*`, with `InitAll` → `SpawnOne`.
  - `TrmpItem_Construct`, `Trmp2_DestructJumperModels`.

**Small leftovers (one batch).**

High confidence:
- **Smartball:** the hand-written constructors of the twelve RTTI-named object classes (`cMgSmartball_wing_c_Construct` ... `cMgSmartball_object_c_Construct`), and `SmartballBall_UpdatePhysics`.
- **Jump:** the phases (`BeginIntro`, `State_Intro`, `BeginPlay`, `State_Play`, `BeginEnd`, `State_End`, `BeginFinal`, `State_Final`, `State_Result`, `State_Idle`). Each `Begin*` stores the record of the state named after it.
- **D3DBase:** the dual-3D display helpers (`D3D_InitDualScreen`, `D3D_CaptureToSub`, `D3D_CaptureToMain`, which write DISPCAPCNT) and the panned-sound thunks.
- **Elsewhere:**
  - Trampoline: `TrmpFx_*`, a linked effect list.
  - Amida: `Amida_Save/RestoreBgChars`.
  - Flower: `UpdatePetals`, `DealPetals`.
  - Hanachan: `Wiggler_Construct`.
  - Curling2: `Curling2_ResolveStoneCollision`.
  - Single3DBase: `LoadCard2SubBg3`.
  - The empty element constructors and destructors.

Medium confidence:
- `Puff_*` (unit020c8a30): three rising, growing sprites spawned at a jumper's x. **Seen** (Jump, Jump2; the same `Puff_Spawn(x, 0xc0)` calls fire on each miss in both trampoline games): the cyan "MISSED 1/2/3" label where a Mario fell. The sprite is picked by the miss count (`mIndex`) from a per-language table; it rises with a sideways wobble, and the third stays. Candidate prefix: `MissLabel_*`.
- `Luigi_PrepareScanlines` / `Luigi_PushScanlineRecord`. **Seen** (Wanted!): the iris's HBlank pair. `PrepareScanlines` runs from the IRQ dispatcher (lr 0x01ffd9d4) once the clock runs out and the iris opens from the centre circle to the full poster wall. Each line, it copies the next line's window registers (0x04000040) from the double-buffered [2][192] table. On lines 192 and up (r0 = VCOUNT + 1, 0xcb..0xdb), it calls `PushScanlineRecord`, which latches the pending buffer and loads line 0. Candidate names: `Luigi_IrisHBlank` / `Luigi_IrisLatchBuffer`.

The three left unnamed here were named in pass 5: `AnyBombMarching` (Bomroom), `BigBoo_SetDone` (Teresa), `Pachinko_UpdateNextBall` (Pachinko).

**dMgJump3DMario_c (Jump, Jump2).** The member names are checked against the state records each setter stores: `StartWindUp` → `StateWindUp`, `StartRiseOut` → `StateRiseOut`, `StartFallOut` → `StateFallOut`, `HideAndIdle` → `StateIdle`. The pool helpers are `Jump3D_*`. Medium confidence: `Jump3D_DisableInput`/`EnableInput` (a flag the scenes toggle) and `Jump3D_SetLowGravity`/`StopGravity` (tuning words). Pass 6 named the three `dMg3DHeyhoObjAdapter_c` slots it implements, which were `Unk_020c76d8/d0/2c`: `GetPos` (slot 0, returns `&mPos`), `GetVelocity` (slot 1, returns `&mVel`) and `IsLaunched` (slot 2, 1 once Mario has left `StateIdle` and `StateWindUp`; medium, since what reads it through the adapter is not traced).
- **Seen:** the flag (`data_ov006_02140400`) is not input. `dMgJump3DMario_c::Draw` reads it to draw the bobbing blue arrow over each Mario. Jump sets it at setup (`EnableInput`, lr 0x020eea38) and shows the arrows; Jump2 clears it (`DisableInput`, lr 0x020efab4) and shows none. Candidate names: `Jump3D_ShowArrows`/`HideArrows`.
- **Seen:** `SetLowGravity` runs once as `State_End` hands to `State_Result`, in both games (lr 0x020ee4b0, 0x020ef550). It sets gravity 0x100 and a floor of −0x4800, then sends every Mario to `StartFallOut`. `StopGravity` (gravity 0, every Mario to `StartRiseOut`) is only called by `dScMgJump_c::BeginFinal`, which was not reached. Candidate names: `Jump3D_FallOutAll`/`RiseOutAll`.

**dMg3DHeyhoObjAdapter_c (the Shy Guys of the Jump games).**
- **Motions** (pass 6; they were `Heyho_Motion0..14` with `Heyho_StartMotion0..14`), named from their bodies. Each `Heyho_StartMotion<Name>` stores its motion's record. `MotionFlyIn` (3: `Heyho_SpawnAll` sends it to its start point, then it switches on `mMoveKind`) and `MotionRegroup` (8: the same from `Heyho_SettleAll`) pick one of: `MotionHover` (bobs in place), `MotionPatrolBob` (crosses the screen, bobbing), `MotionBobVertical`, `MotionBounceNear` (inside a 0x60 box by its start), `MotionVShape` (bottom, a top corner, bottom centre), `MotionBounceInRange`, `MotionDiagonal` / `MotionDiagonalMirror` (corner to corner of its range), `MotionCircleUpper` / `MotionCircleLower` (laps of the upper or lower field, turning opposite ways), or `MotionChase` (heads for the nearest launched jumper through the adapter's `IsLaunched`). `MotionHitSpin` is the squash-and-spin after a jumper hits one (sound 0x1c8, then 0x1c7), and `MotionLeave` eases it 0x100 to the side, off screen (`Heyho_React`). The kind-to-motion map is 0 Hover, 1 PatrolBob, 2 BobVertical, 3 BounceNear, 4 VShape, 5 BounceInRange, 6 Diagonal, 7 DiagonalMirror, 8 CircleUpper, 9 CircleLower, 10 Chase.
- Pool and object helpers: `Heyho_InitPool`, `Heyho_SpawnAll`, `Heyho_ReactAll`/`Heyho_React`, `Heyho_SeparateAll`, `Heyho_CheckHitA/B`, `Heyho_Update`, `Heyho_Draw`, `Heyho_Construct`/`Heyho_Destruct`, `Heyho_LoadResources`.
- Medium confidence: `Heyho_Settle`, `Heyho_SettleAll`, `Heyho_AllIdle`, `Heyho_IsChasing`.
- **Seen** (Jump and Jump2, runtime-checks.md "Ten more"):
  - A Mario landing on a Shy Guy starts `Motion11` (the spin). `Settle` then runs: a white puff (particles 0x122/0x123), score +1, and in Jump2 a yellow `Jump2Popup` number (the running score) rising from the spot. Candidate name: `Heyho_KnockOut`.
  - `SettleAll` runs right after, from Jump2's `State_Play` (lr 0x020ef674). It sends the first idle Shy Guy back in with `Motion8` from a random entry point, and it flies in from above. Candidate name: `Heyho_SendInNext`. Jump never called it.
  - `AllIdle` is polled every frame of Jump2's `State_Result`, and GAME OVER follows once it returns 1. The name fits.
  - The motions seen: `Motion3` at setup (one per Shy Guy, from the 0x0212d560 table); `Motion12` for all three when play starts; `Motion4`, `5` and `9` now and then in play (from 0x020c6848, 0x020c686c and 0x020c6830); `Motion14` once at the end.

**dMgTrmpln2Mario_c / dMgTrmpln3DMario_c (the trampoline Marios).** The two classes mirror each other, so their helpers carry parallel names.

High confidence:
- **States** (pass 6; they were `Trmp2Mario_State0..7` and `Trmp3DMario_State0..12`, each with its `EnterStateN` setter), named from their bodies and their callers:
  - Both Marios: `StateKnockedOut` (pops up and falls off the bottom, then hidden or offstage), `StateBounce` (rising after a line bounce), `StateMissed` (fell past the bottom: a puff, one life off, sound 0x130), `StateFall` (falling; a line contact calls `BounceOffLineA/B`).
  - Trampoline2: `StateWallBounce` (bumped up off a side wall), `StateRespawnWait` (waits at the top, then drops in at a random x), `StateAppearWait`, `StateHidden`.
  - Trampoline: `StateHardBounce` (from `BounceOffLineA`, contact kind 2), `StateLand` and `StateFallToLedge` (landing on a tower row or the ground), `StateGoal` (on the goal row: walks off the side and scores the combo), `StateOnLedge` / `StateWalkBack` (the tower ledge, right), `StateOnGround` / `StateWalkIn` (the ground, left), `StateOffstage`; `IsMissed` and `IsOffstage` were `IsInState6/12`.
- **Setters:** each `Enter<State>` stores the record of its state, checked through ov006 relocs.
- **Getters:** `GetPos`, `GetPrevPos` and `GetVelocity` return the +0x24/+0x30/+0x3c (2Mario) and +0x1c/+0x28/+0x34 (3DMario) vectors, which the Update/Matrix bodies read as position, previous position and velocity.
- **Lifecycle:** the destructors and constructors the file banner identifies.
- **Object adapters:** the `TrmpObjAdapter_*` and `TrmpRingObjAdapter_*` constructors, which store those classes' vtables.

Medium confidence: `BounceOffLineA/B` (the reflection off the drawn line), `EnterByKind`, `MoveToStart`, `PlaceAtStart`, `CountFinished`, `SetSpawnHeight`.

**Seen** (Trampoline and Trampoline2, runtime-checks.md "Ten more"):
- `BounceOffLineA/B`: `State7` (3DMario) and `State5` (2Mario) dispatch on the u16 at +0x18, set by the line as it catches a jumper. Kind 2 (A, sound 0x1b1) is a catch near the line's centre, where |offset| × half-length < 0x400000; kind 1 (B, sound 0x1ae) is a catch further out. B ran 22 times to A's 2, with flat and slanted lines alike. Trampoline2's start pad catches with B.
- `CountFinished` and `SetSpawnHeight` are polled every frame (lr 0x02121878 and 0x02121bfc). Game over follows `CountFinished` at once.
- `EnterByKind` ran once per Mario at game end (four Marios, stride 0xd0).

`Trmp2Mario_EnterAll` (was `func_ov006_020ca840`, pass 5): sets the launch globals, then runs `EnterByKind` and `UpdateMatrix` on every trampoline Mario. **Seen** once, at Trampoline2's game over (`BeginResults`, lr 0x02123c84). It sets gravity 0x100 and a floor of −0x4800 for every Mario, and raises the flag (`data_ov006_021405ac`) that stops `TrmpItem_UpdateWithJumper` scoring. Each Mario then goes through `EnterByKind`: a hop (vy 0x2000, sound 0x1c9) and a slow fall, or State7 from State3.

## Member-pointer records (`pmf_*`, renamed 2026-10-15)

630 8-byte `{function, adjustment}` records in ov004/ov006 `.data` were address
names (`data_ov006_0213cb84`). Each is now named for the function its first word
relocates to: `pmf_<Class>_<Method>`, or `pmf_<Function>` for a free one. A
`_2`/`_3` suffix marks a second record pointing at the same function. This is certain:
the name is read off the relocation. So `mState = pmf_dScMgJump_c_State_Play;` says
which state is installed.

## Pass 2 names: types and record fields (2026-10-15)

These come from the shape pass. Each field is named from the functions that
read and write it, and every header change was checked against the object
compare (all 45 TUs identical).

**Scene base.**

| Name | Where | Confidence | Evidence |
|---|---|---|---|
| `gMgScene` typed `dScMgBase_c *` | `dScMgBase_c.h` | high | The `dScMgBase_c` constructor stores `this` and the destructor clears it. |
| `mTopRecord` | `dScMgBase_c` +0x4650 | high | `Mg_SetSessionBest` compares scores against it, and the constructor seeds it from the save table. |
| `dMgPsOpt_c::Open/Close/Update/Draw/IsIdle` | ov004 | high | The former `PsOpt_*`; each casts its argument back to `dMgPsOpt_c`. |

**Teresa (`dScMgTeresa_c`), confidence by field.**

- **`SmallBoo mBoos[16]`** (0x4660):
  - **high:** `mX`/`mY`, `mHeading` (indexes the sine table; its sign picks `mAnim`), `mFrame`/`mAnimTimer`, `mFade` (nonzero draws semi-transparent), `mVisible`, `mPriority` (passed as the OAM priority), `mState`.
  - **medium:** `mSpeed` (0xc00 adjusted by `mKind`), `mInPlay`, `mKind`, `mFound` (set by the touch test), `mFoundOrder`.
  - **unknown:** `unk_12`, `unk_19`. (`unk_21` is now `mPlaySettleSound`; see the pass 3 fields below.)
- **`mExclaims`, `mVanishPuffs`, `mFoundNumbers`** (16 each): **high, seen** (the "!", the vanish puff and the found-order digit; see the Teresa section above).
- **`mReveal[1]`, `mMarker`** (0x4ba0, 0x4bc0):
  - **medium:** the field mechanics, including the bounce `phase` and `angle` (the `OAM::Render` rotation).
  - **low:** the one-element array. It is inferred from handlers that take an index while the dispatcher passes 0.
- **`mFoundCount`, `mLastSettledBoo`, `mBooCount`, `mFirstFoundBoo`** (0x4c21..0x4c26): **medium**, named from their writers.

**Pachinko (`dScMgPachinko_c`).**

- **`mFx[30]`:** **medium**. The hit bursts; `kind` 2 is the long animation.
- **`mTarget[4]`:** **medium**. `sprite` is high: it indexes the target sprite table.
- **`mFlyer[1]` and `mBg[1]`:**
  - **medium:** the fields. `vy` is high: `PachinkoFlyer_StateWaiting` seeds it and `StateDescend` adds it to y.
  - **low:** the one-element arrays, inferred the same way as Teresa's `mReveal`.
  - **seen:** the flyer is Lakitu and the targets are the four flowers (runtime-checks.md, 2026-10-15). The background is seen too: `mBg` scrolls the airship across the top screen, and `PachinkoDeco` is its wings.
- **0x5418..0x5958:** no reconstructed function touches it.

## Pass 3 fields (2026-10-15)

The `unk_` members that some code reads, named from that code. Each batch kept
all 45 minigame objects byte-identical (`mgsnap`). Confidence is high unless the
row says otherwise; "seen" marks a name checked in play.

| Class | Names |
|---|---|
| `dScMgSlot1_c` | `mReelScroll[3]`, `mSpinSound`, `mStep`, `mPayout`, `mReelStrip`, `mReelStop[3]`, `mReelSpinning[3]`, `mReelsStopped`, `mSpeedLevel`, `mMatchSymbol`, `mLineMatched`, `mWilds`, `mTimer`; medium: `mStartCredits` |
| `dScMgSlot3_c` | `mStripSpan`, `mDifficulty` (0..5, sets strip length and reel directions), `mPayoutLeft`, `mRowStopped[3]`, `mRowsStopped`, `mTimer`, `mReelReverse0..2`; seen: `mTargetFace`, the face Toad shows |
| `dScMgCard_c` | `mPlayerHandFace`/`2` and `mDealerHandFace`/`2` (EvaluateHand's faces, which RenderHud highlights); medium: `mDealCount` |
| `dScMgMCarlo_c`, `dScMgMCarlo2_c` | `mRoundState`, `mTimer`, `mDealCount` |
| `dScMgCurling2_c` | the drag (`mDragX/Y`, `mDragOffsetX/Y`, `mGrabX`, `mTurnY`, `mLastDragDy`, `mDragState`, `mDragPhase`), the throw (`mThrowPower`, `mThrowAngle`), `mTopRecord`, `mShowCursor`, `mShowCounter` |
| `dScMgBomroom_c` | bombs: `fuse`, `exploded`, `overPen`, `dragSound`, `marchDelay`, `lineSlot`, `visible`; scene: the round-end tally (`mTallyDelay`, `mTallyTick`, `mTallyCount`, `mShowTally`), `mFusesFrozen`, `mDispatchDelay` |
| `dScMgPachinko_c` | `mPhase`, `mEndTimer`, `mNextBallDelay`, the enemy waves (`mWaveTimer`, `mWaveSpawns`, `mWaveLevel`), `mHitPause` |
| `dScMgPachinko2_c` | balls: `spin`, `active`, `visible`, `sprite`, `animStep`, `facingRight`; paddles: `step`; scene: `mPhase`, `mNextBallDelay`, `mEndTimer`; medium: `mBallDealt` |
| `dScMgJump2_c` | `mAnimPhase`, `mAnimRate`, `mParticleId`, `mStateTimer` |
| `dScMgTeresa_c` | `mSceneState`; Boos: `mPlaySettleSound` (one Boo per settling group plays 0x1f1) |
| `dScMgAmida_c` | `mLineColor` (the colour argument of every `Mg_DrawLine`) |
| `dScMgSnowball_c` | `mBallBmd`, `mHitSoundX` |
| `dScMgD3DBase_c` | `mDrawSubScreen` (flips every frame; picks which screen the 3D is captured to), `mSavedSubObjBank` |
| `dMgState_c` | `mSuspended` (state 6 sets it, `Mg_ResumeSharedState` clears it) |

**Corrections.**
- Bomroom's bomb `+0x28` was commented "set to the spawn step base". Its only uses are a sound handle while the bomb is dragged and a reset, so it is `dragSound`.
- Slot3's `mLineActive` ("which pay line is bet") is set when that row stops, and the row's step is skipped after that. It is now `mRowStopped`.

**Renamed in pass 4.** `dScMgPachinko2_c::UpdateRoundEnd` puts the next ball in the pen once `mNextBallDelay` runs out; it is not a round-end step, so it is now `UpdateNextBall`.

**Left as `unk_`.** The rest have no read in the reconstructed code (reset-only or written only), or only reads by offset whose meaning is not settled: Flower's `unk_5fcd`, Snowball's `unk_ab7a`, the card dealer's `unk_01a`, and `dMg3DEspModel_c`'s first three words. Most of Memory's, Memory2's and Roulette's are reset-only.

## Pass 4 (2026-10-15)

- **`MgEntry::sceneParam`** (`dScMiniGm_c`, was `unk4`): high. `StartSceneFade(id, sceneParam, 0)` opens the game with it as param1, and its low byte is what `IsAltVariant` tests (seen: the two Snowballs, runtime-checks.md).
- **Teresa's `FoundNumber` and `VanishPuff`, Pachinko2's `UpdateNextBall`, Hanachan's `Wiggler_DrawTarget`/`Wiggler_DrawField`:** see those games' entries above.

## Pass 5 (2026-10-16): offsets, layouts and globals

**Functions.** The last unnamed functions are named: `AnyBombMarching` (Bomroom), `BigBoo_SetDone` (Teresa), `Pachinko_UpdateNextBall`, `Trmp2Mario_EnterAll` and `Coin_Construct`. Every function defined in `src/minigames/` now has a real name.

**Offset reads to members.** `*(T *)(alias + 0xNNN)` reads were rewritten to the declared member wherever the cast width and signedness match the field and the function's bytes do not move (bisected per function against strict byte equality). About 190 sites in the scene classes, through `this` aliases, typed and macro views, and indexed records (`mBalls[i].x`). The functions that moved keep their offsets; most are the loops the file comments mark as codegen-pinned.

**Recovered layouts.** Four record types had no header; each layout is recovered from the helpers that use it (every offset is read at one width) and named from those uses. The helpers read them through cast macros, the pattern `PACHINKO()` already used:

| Header | Record | Size | Sites |
|---|---|---|---|
| `dMgTrmpln2Mario_c.h` | Trampoline2's Mario | 0xdc | 231 |
| `dMgTrmpln3DMario_c.h` | Trampoline's Mario | 0xd0 | 241 |
| `Heyho.h` | a Jump Shy Guy (coined name) | 0xf0 | 301 |
| `TrmpLine.h` | a drawn trampoline line (coined name) | 0x32c | 100 |

The two Marios share their first five fields with the line code's jumper view (`TrmpLine_PushJumper`): the contact normal, offset, half width and contact kind (0 none, 1 hit, 2 hard hit, 3 the line's end). A Mario's bounce scales by `halfWidth / (halfWidth + |offset|)`, so a centre hit bounces hardest.

**Globals.** Named from their writers and readers (free data symbols, so no lineage rows):
- the record pools and counts of each game that keeps one (trampoline Marios, items, lines, effects, spinners, Heyho, Jump2 popups) and their tuning (fall speed, gravity step, maximum rise speed);
- 80 member-pointer dispatch tables, each after its handlers' common prefix (`sTeresaBooStateTable` holds the eight `Boo_State*` handlers);
- 15 file handles, after the file each resolves to through `tools/asset_catalog.py` (`gEspCardWaitAnimFile` is `MG/esp_card_wait.bca`); handles with the 0x8000 bit stay unresolved;
- the shared banner positions `dMgState_c` places each banner at, the digit sprite table, the 3D scenes' three cameras, and per-game state (card picks and lists, scores, deal progress, slot reel positions, sound note effects).

**Left as they are.** Smartball's scene code is pinned by volatile and laundered spellings (see its file comments), so its offsets stay. The dealer, presenter and mascot records in `unit020bfec0`, and Trampoline2's item record, still read by offset: their embedded file and animation objects need more layout work. About 200 BSS globals and 650 initialized data symbols with few uses each, mostly sprite tables, keep their address names.

## Pass 7 (2026-10-16): the remaining data

**Archive file IDs.** A file handle of 0x8000 or more is an archive entry, so `tools/asset_catalog.py` leaves it unresolved. Pass 7 read `ARCHIVE/arc0.narc`'s own file table from the EU ROM: 0x80NN is entry NN of `arc0.narc`. The trampoline Marios' thirteen animations confirm the mapping against the code that plays them. 0x8078 is `data/player/jump.bca`, 0x808e is `roll_jump.bca` (the bounce), 0x80b4 is `walk.bca` (the walk states), and 0x80ac and 0x80ab are `turn.bca` and `trned.bca` (the ledge's turn, then its turn end).
- **Trampoline Marios:** `sTrmpMarioModelFile`, the `sTrmpMario<Anim>AnimFile` handles and the `sTrmpAnim<Anim>` files loaded from them (Jump, DoubleJump1/2, WallJump, HangUp, RollJump/RollJumpEnd, Walk, JumpEnd, Turn/TurnEnd, Struggle, Sdown). The existing `sTrmpAnimBounce` is the loaded `roll_jump.bca`. Also `sTrmpRowHeights` (the tower rows; [0] is the ground), `sTrmpGoalRow`, `sTrmpGoalSlotCursor` (cycles 0..5 through `sTrmpGoalSlots`, the six places a scoring Mario is set), `sTrmpSpeedScale` and `sTrmp2DriftScale`.
- **Jump3D Marios:** the six animation handles resolve the same way (`sJump3DMario<Anim>AnimFile` -> `sJump3DAnim<Anim>`: WallJump, Sdown, Struggle, RollJump, Jump, DoubleJump1). Also `sJump3DMarioPool` (the 0xb8-byte records), `sJump3DMariosLeft`, `sJump3DRisenOutCount` (bumped as a Mario rises out of the top) and `sJump3DSpinAngle`.
- **Effects and card lists:** `sPuffs` (the three puffs), `sPuffNextIndex` and `sPuffXTable` (set by `Puff_ClearAll`); `sMCarloCardsTail` and `sMCarlo2CardsTail` (the tail of each card list, `sMCarloCards` / `sMCarlo2Cards` being the head); `sD3DOamShadow` (the 0x400-byte OAM copy the 3D scenes flush).
- **Picture Poker:** `sCardSwapRequested` and `sCardSelectLocked`, both set by the swap button at (0x80, 0x58); the first sends the picked cards to swap, the second stops further picks.
- **Trampoline2 items:** `sTrmpItemsHit` (items hit by jumpers, which raise the tuning and drop the prompt at 3), `sTrmpItemsActive` and `sTrmpItemHitsOff`.
- **Function-local constants:** a local `static Vector3` compiles to a guard word, the vector and a destructor record. The ones still carrying address names are named `s<Class>_<Function>_<Value>`, with `...Guard` and `...DtorRec` (`build/mgnames-tools/localstatics.py` finds them). Examples: the axis vectors in the TrmpItem spin motions (`sTrmpItem_MotionSpinHover_UnitY` / `_UnitZ`), and Heyho's ±0x100 side offsets (`sHeyho_React_Left256` / `_Right256`).
- **Counters and tuning:** `sHeyhoUnsettledCount`, `sTrmpMariosOnLedges` and `sTrmpMariosInPlay` (the Trampoline scene checks `< 3`), `sTrmp3DBounceScale`, and the Monte Carlo boards (`sMCarloRows` × `sMCarloCols`, the deal and board timers, and MCarlo2's).
- **Per-game state:** `sHanaStarPopups`, `sHanaRoundPattern` (the round's Wiggler layout, from a table), `sWigglerSprites`, `sRouletteLandedRacer`, `sAmidaLaneTargets` / `sAmidaLaneSaved`, `sLuigiSlotHandlers`, `sFallingPetalAxis` (medium), and the background file pointers `sJumpBackFile` / `sJump2BackFile`.
- **Drawing helpers:** `sMgPaintWord`, `sTrmpBrushWord` and `sTrmp2BrushWord` (the one-word read-modify-write buffers the paint routines use on BG tiles), `sMgSparkleSprites`, `sD3DSubObjPalette`, `sTrmp2StarModelFile` (`ASSET_MINI_STAR_BMD`).
- **Trampoline2's items are rings.** `TrmpItem`'s three models are `ASSET_RINGA/B/C_BMD` (`sTrmpItemRingAModelFile`, which was `sTrmpItemModelFile`, plus `...RingB...` and `...RingC...`). `include/TrmpItem.h` says so; the coined record name stays.
- **More named in pass 7:** `sSmartballCellOrder`, `sTrmpStartLine` / `sTrmpStartLinePoints`, `sTrmpLineTexAddr` / `sTrmpLinePlttAddr`, `sHanaPickedIds` / `sHanaPickedLow` (medium), `sPanelSprites`, `sCardObjSprites`, `sRouletteRacerGoal`, `sRouletteTilePos`, `sRouletteResultsTimer`, the banner burst positions (`sBannerWaveFxPos`, `sBannerZoomFxPos`), `sTrmp3DHardBounceScale`, `sJump2Back02ModelFile`, `sHeyhoLayout`, `sJump3DArrangeLevel` / `sJump3DLayout`, and `sPsOptIconPosA/B`.
- **`Trmp3DMario_JumpOff`** (was `Trmp3DMario_PickIdleAnim`, a medium guess). Both walk states call it when Mario reaches the jump point. It sets his gravity and a sideways speed toward the field (`sTrmp3DJumpOffSpeed` plus a random part), then enters `StateBounce`, so it starts a jump rather than picking an animation. Also `sTrmpAnyOnLedge` (holds off the door switch while a Mario stands on a ledge), `sD3DLanguageFile`, `sHanaSpinAngle`, `sTrmp2MarioPtrs`, and the PsOpt icon tables' second halves (`sPsOptIconPosAY` / `BY`, `sPsOptStylePos` / `Y`).
- **Initialized data (pass 7):** `sTrmp2Lives` / `sTrmp3DLives` (start at 3; a miss takes one, 0 ends play), `sTrmpBounceAnimTable` (the bounce animations by `mAnim`), `sTrmpStartTouchX/Y` (medium: the intro's stylus point), `sHanaFieldWigglerCount`, `sHanaBackgroundParts`, `sSnowballCourseTiles` (the course column's tile words), `sRouletteTileBounds`, `sRouletteResultIndex`, `sSmartballMarkerSprites`, `sMCarloDeckSize`, `sMgScoreDigitsX/Y` (where `DrawScoreDigits` puts the result digits; `Mg_ResetBannerLayout` sets both to 0x80).
- **Sprite tables:** a data symbol whose every use is the sprite argument of a render call is named `s<Game><Drawer>Sprites`, after the function that draws it (`build/mgnames-tools/spritetables.py`). Examples: `sCoinBagSprites`, `sTeresaExclaimSprites`, `sPachinkoFlyerSprites`, `sBomroomPenGateSprites`. Tables drawn by a generic `Render`/`Update` that draws several tables stay unnamed, since a counter suffix would say nothing.
- **Tables and tuning words (pass 7):** the Jump3D Mario's tuning (`sJump3DBounceVy`, `sJump3DRiseVy`, `sJump3DFallSpeed`, `sJump3DGravityStep`, `sJump3DMoveSpeed`; `Jump3D_SetTuning` scales them, `SetLowGravity` / `StopGravity` override the last two) and `sJump3DAnimTable`; Monte Carlo's `sMCarloPayout` (5, 8 or 10 by board), `sMCarloFaceCounts`, `sMCarlo2DeckSize` / `sMCarlo2CardsWanted`; Cup's `sCupHomePositions`, `sCupRoundTable` (swaps per round), `sCupShuffleOffsets` / `Y`, `sCupAnimTable`; Pachinko2's cup stop positions and `sPachinko2CrossRowY`; the presenter's idle and cheer texture animations; Card's `sCardFaceWeights` and `sCardFaceSprites`; Luigi's sway speed and acceleration, picture sprites and board modes and flags (the flags medium); `sSlot3FaceSprites`, `sTrmpItemPatrolSpeed`, `sTrmpLineTexCoords`, `sCurlingHouseSpots`, `sBscLaneX`, `sHanaBackgroundPartsB`; and the shared `sMgScoreX/Y`, `sMgBrushPalette`, `sMgStockCounterFiles` (per language).
- **More tables:** Panel's `sPanelPuzzles` / `sPanelPuzzlesAlt` (the alternate set when `IsAltVariant`) and `sPanelPuzzleSizes`; Slot1's `sSlot1Sprites` and `sSlot1Payouts`; the Pachinko pipe stops and sprites; `sSnowballBitSprites`; the presenter's texture animations (`Lose`, `Miss`, `Match`, plus `Anim4` / `Anim7` for the two states named by index); `sMascotWalkerOffset`; `sTrmpItemHitWidths` (by `mSizeIdx`); and, medium, Trampoline's `sTrmpRowScreenY` and `sTrmpPuffXByLanguage`.
- **The presenter is Mario's face.** `Presenter_Construct` builds a `face_demo_mario.bmd` model in front of `f_demo_back.bmd`, and its eight animation slots resolve to `MG/face_wait`, `face_mistake`, `face_win`, `face_lose`, `face_lose_st`, `face_lose_ed`, `face_smile` and `face_surprised.bca` (`tools/asset_catalog.py`). That settles the states named by index:
  - `Presenter_StateMistake` (was `StateAnim1`) after `PlayMiss`, and `StateWin` (was `StateAnim2`) after `PlayMatch`;
  - `StateLoseStart` (was `StateAnim4`): `PlayLose` plays lose_st, then loops lose;
  - `StateSurprisedThenSmile` and `StateSurprisedThenIdle` (were `StateAnim7` / `StateAnim7Loop`), after `PlayCheer` / `PlayCheerLoop`;
  - the texture animations `sPresenterLoseLoopTexAnim` and `sPresenterSmileTexAnim`.

  `include/Presenter.h` holds the 0x1ac-byte layout, which the helpers read through `PRESENTER()` (36 sites). The embedded BlendModelAnim, Model, TextureSequence and TextureTransformer stay byte regions. `mTracking` and `mQuiet` are medium: only the constructor writes them.
- **The dealer is Luigi.** `Dealer_Construct` builds `MG/luigi_d.bmd`, and its thirteen animation slots resolve to `luigi_d_wait`, `shuffle_start`, `shuffle`, `deal_start`, `deal`, `deal_end`, `deal_end2`, `deal_wait`, `throw`, `win_start`, `win`, `win_end` and `lose.bca`. Two of the earlier names had the shuffle and the deal the wrong way round, so they are corrected:
  - `Dealer_StartShuffle` / `StateShuffle` (were `StartDeal` / `StateDeal`) play shuffle_start, then loop shuffle;
  - `Dealer_StartDeal` / `StateDeal` (were `StartCollect` / `StateCollect`) play deal_start, then loop deal, counting each card dealt (sound 0x142), and end with deal_end or deal_end2;
  - `StateSwap` plays throw, `StateReveal` the deal ends, and `StateWin` / `StateLose` their animations.
  `include/Dealer.h` holds the 0x270-byte layout (`DEALER()`, 108 sites in the `Dealer_*` helpers): the shuffle and deal counters, the lamp timer, pattern and light angle, the thirteen animation files, and the once-per-loop sound flags. The embedded BlendModelAnim, table Model and MascotWalker stay byte regions.
- **The mascot is Toad** (`MG/kino_d.bmd`, `kino_wait.bca`). `include/Mascot.h` holds the 0xac-byte layout: the look target, position, body yaw, head pitch and yaw (`Mascot_Draw` clamps them to ±0x1000 and ±0x2800), and the `mLooking` flag that `Mascot_LookAt` / `Mascot_StopLooking` set and clear. The helpers read it through `MASCOT()` (22 sites). `Mascot_Draw`'s local view now uses the same member names.
- **The flower game's face is Yoshi.** `FlowerFace_Construct` loads `MG/yoshi_model.bmd`, the petal `MG/f_parts.bmd`, and `yoshi_wait`, `yoshi_pick`, `yoshi_like` and `yoshi_hate.bca`. So `FlowerFace_PlayLike` / `PlayHate` (were `PlayReactA` / `PlayReactB`) are the two endings, and `StateLike` / `StateHate` / `StatePick` were `StateReactA` / `StateReactB` / `StateReact`. `d_s_mg_flower.cpp` plays Like when `mPetalToggle` is 1, matching the red "LOVES ME" banner seen in play. `include/FlowerFace.h` holds Yoshi's 0xdac-byte record and the 0x98-byte `FallingPetal`.
- **Smartball's board objects (pass 7).** The rewrite driver now covers the `cMgSmartball_*_c` methods, with the object base's members folded into each child. It kept 88 member reads in 16 methods and dropped none, so the scene's pinned spellings did not reach these methods. The fields it exposed are named: the base's `mTriggered` (was `unk_031`: raised when an object is triggered, as each class header describes), the spring's latched touch X (`mGrabX` / `mGrabXSaved`), and the wing's three trigger fields (`mTriggerTime`, `mTriggerPhase`, `mTriggerFlag`; Trigger writes them and nothing reads them back). The scene's own pinned helpers and the shadow structs the file lists keep their offsets.

## Pass 8 (2026-10-16): the data tables

About 490 more data symbols are named, mostly from the files they load and the strings they hold. Two facts about language order did most of the work:

- **`GetGameLanguage()` indexes 0 English, 1 Spanish, 2 French, 3 German, 4 Italian.** Every five-word handle table indexed by it resolves (`tools/asset_catalog.py`) to one file's variants in that order: `d_2d_mg_card_ncg`, `_S_`, `_F_`, `_G_`, `_I_`. Those tables are named `s<Game><File>NcgByLanguage` (`sCardNcgByLanguage`, `sSlotPauseBgNcgByLanguage`, `sMgOptionNcgByLanguage` ...).
- **`GetOwnerLanguage()` is the DS firmware order** (2 French, 3 German, 4 Italian, 5 Spanish). Smartball chooses its line sprites and its layout file name from it, so those are `sSmartballLineSprites{French,German,Italian,Spanish}` and `sSmartballLayoutNcgFmt{French,German,Italian,Spanish}`. The plain and `_E` formats are chosen by `param1`'s low byte, which the code does not explain, so they keep the file's letter (`sSmartballLayoutNcgFmt`, `...FmtE`). The four Smartball tables chosen by the same byte are `...Sprites` / `...SpritesAlt`.

The rest:
- **Shared text sprites.** Five sprite-pointer arrays, `sMgTextSpritesEnglish` ... `Italian`, and each game's five-entry table of them by language (`sJumpTextSpritesByLanguage`, `sLuigiTextSpritesByLanguage` ...).
- **Banners.** Each of the 28 handle tables in `Mg_LoadBannerFiles` holds the five language variants of one `MG/d_2d_msg_*_ncg` file, so the handle table, its per-language sprite table, and the English sprite record in `sMgBannerSprites` are named after that message (`sMgMsgRetryNcgByLanguage`, `sMgMsgRetrySpritesByLanguage`, `sMgMsgRetrySpritesEnglish`). Entry 1 is GIVE UP (`sMgMsgGiveUpSprites`, handle 0x140), the one banner without language variants.
- **File paths** are named from their strings (`sFlowerBgUpNcgPath`, `sSmartballLay0Bg1FrameNscPath` ...).
- **Single-use tables** are named from their one use: per-difficulty counts (Memory2's deal and preview tables, `sEsp3DCardsPerDifficulty`), animation frame and time tables, positions, Sound's per-pop `VX` / `XOffsets`, and sprite tables after what they draw.
- **Two corrections while doing it:** `sMCarloPickAllowed` (MCarlo's flag is set when the board has a removable pair and gates touch input), and `sTrmpTouchBlocked`.

- **`Trmp2Mario_Construct`** (0x020ca78c) was filed as `_ZN6Player29TryExitCharacterDoorWithIntroEv`. It constructs `dMgTrmpln2Mario_c` the way `Trmp3DMario_Construct` constructs its sibling. The vptr it stores (`sTrmp2MarioOps`, 0x0213b2e0) is preceded by offset 0 and `_ZTI17dMgTrmpln2Mario_c`, and its slots are `Trmp2Mario_GetPos`, `GetPrevPos`, `GetVelocity` and `Reset`. `sTrmp3DMarioOps` (0x0213b2c4) is the same for `dMgTrmpln3DMario_c`.
- **Coined classes renamed after their records:** `MgFx` (was `Ent`, the base's `sMgFxPool`), `MgBanner` (was ov004's `Elem`, `gMgBanners`), `Jump2Popup` / `Jump2PopupLink` (were Jump2's `Elem` / `Node`), and `TrmpScorePopup` (was `Desc`, on `sTrmpFxList`).
- **From closer reading:** the Trampoline line's bounce-sound pitch at its middle and its ends (`sTrmpLine{Weak,Strong}Pitch{Mid,End}` and the `Push` pair; the third argument reaches `func_02012860`'s track-pitch call), Sound's row lengths, Amida's per-pattern modes (`GoalShuffle`, `RungSkip`, `ExtraRungs`, `SpeedUp`), Cup's swap pairs, the Slot blink positions, Teresa's spawn kinds and burst flag, Luigi's touch sounds and the wanted poster's tile offsets.

- **`sHeyhoScorePopups`**: only Jump2's InitResources sets it, and while it is set a Shy Guy that settles spawns a score popup.
- **`sJump3DStartVx`** (Jump3D_SetTuning scales it; StartRoundLayout multiplies it by the layout entry into each Mario's `mVel.x`) and **`sPanelFlipFrameSprites`** (the `sPanelSprites` index for a flipping panel by kind and frame).
- **Sprites read off their draw code:** Snowball's breakable (animated and static), obstacle, side-obstacle, ball and course-meter sprites (`mArray1` is the breakables, `HitsBreakable`); the life icons Trampoline and both Jump games draw for each Mario left (a lost one is drawn from the language set); Trampoline's door signs; Smartball's sparks (`Alt` again by `param1`); and the three menu items' sprites by language (`mMenuItem0..2`).

About 73 data and 11 BSS symbols keep their address names. Their one use doesn't say what they hold: lerp endpoints, flags written once, or tables read through an offset into a stride.

`datadump.py` reads ov004 at its overlay-table base, 0x020ad660. ov005 and ov006 load at 0x020bfec0.
