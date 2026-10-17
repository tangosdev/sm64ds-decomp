# Minigame names: runtime checks

[names.md](names.md) marks some names **medium** or **low** confidence. The code
settles their mechanics but not their game-facing meaning: which graphic a banner
id shows, what an animation looks like, or what a flag means to the player. This
page lists what to watch, in melonDS, to settle each one. None of it changes code.
A confirmed or corrected name is then a plain in-place symbol rename, like the
ones this pass made.

## Setup

The repo already drives the melonDS GDB stub from Python (`tools/trace/rsp.py`).
`tools/trace/README.md` has the exact `melonDS.toml` keys and the stub's quirks:
one client per launch, detach before closing, never send a break.
1. Start melonDS with JIT off and the GDB stub on (port 3333), then load the EU ROM.
2. Enter the minigame menu: the title screen's minigame option, or the rec-room
   objects in the castle.
3. Attach with a short script built on `rsp.py`. It sets the breakpoints below,
   and on each hit logs `pc`, `r0`-`r3` and `lr` (resolve the caller with
   `tools/trace/symindex.py`), then continues.
   - `tools/trace/gdb_harness.py --names <symbol,...>` does this for symbols it can resolve.
   - `tools/trace/behavior.py` records inputs and outputs per call.
4. Play the game named in each row and note what is on screen when the breakpoint fires.

All addresses are EU (ASMP). ov004, ov005 and ov006 are resident only while a
minigame or the menu runs. ov005 and ov006 share 0x020bfec0, so a breakpoint in
one is meaningless while the other is loaded.

## What to watch

| Question | Breakpoint (symbol, address) | Log | Play |
|---|---|---|---|
| What each shared state (0..19) shows | `Mg_SetSharedState` 0x020b0a54 | r0 = state | **Done:** seen 0, 1, 3, 4, 5, 7, 8, 12, 16, 17 and 18; still open are 2, 6, 9-11, 13-15 and 19 (Snowball has 19). |
| Which graphic each banner id is | `Mg_ShowBanner` 0x020b0cac | r0 = file id, r1/r2 = x/y | The same plays. Fill in the "Guess" column of names.md's state table. |
| One-shot banner override (e.g. "Perfect") | `Mg_SetNextBanner` 0x020b67e8 | r0 = id | **Done:** Sound with no miss sets 6, which is "PERFECT". |
| What jingle ids 1, 3 and 4 sound like | `Mg_PlayJingle` 0x020ae274 | r0 = id | **Done:** they play with the new record (1), GAME OVER (3) and CLEAR (4). They were placed by the moment they play, not by ear. |
| `StartRound(state)` is really "start or restart a round" | `Mg_CallStartRound` (see symbols.txt) | r1 = finished state | **Done:** seen with 0, 1, 4, 5, 7, 8 and 0x12. |
| `OnNewRecord` fires as the record banner goes up | `dScMgBase_c::OnNewRecord` 0x020b2990, plus the Cup/Sound/Teresa/3DEsp overrides | hit or not | **Done:** yes. On a new record, state 17 shows banner 7 over the record table, then `OnNewRecord` runs, then banner 5 (rainbow "HIGH SCORE") and jingle 1. A tied score goes to state 16 with no call. Overrides seen: Sound and Cup (each plays `Presenter_PlayCheer`) and Teresa. The base runs in Trampoline2, Panel and MCarlo2. 3DEsp's override was not reached. |
| `OnDealDone` fires when the dealer finishes dealing | `dScMgBase_c::OnDealDone` 0x020b298c, plus the BSC/Card/Memory/Memory2 overrides | hit timing | **Done:** it fires once Luigi finishes dealing, in Memory and in Card. |
| What Presenter animations 1, 2, 4 and 7 look like | `Presenter_PlayMiss` / `PlayMatch` / `PlayLose` / `PlayCheer` 0x020c2664 / 0x020c271c / 0x020c2440 / 0x020c2594 | hit | **Done:** seen in Sound and renamed (names.md). |
| `mCredits` really means lives or coins | `Mg_LoseCredits` / `Mg_GainCredits` (symbols.txt) | r0 = amount | **Done:** coins in Slot3, Memory, Card and Roulette. |
| Is "bet" the right word for the coin animator | `MgCoins_PlaceBet` 0x020b63a0, `MgCoins_AddBetCoin` 0x020b6234 | r0 = count | **Done:** yes. Memory stakes 3 coins, and Card's BET! button runs `AddBetCoin`. |
| The menu's "new" badge | `MiniGm_IsNew` 0x020c007c, `MiniGm_ClearNew` 0x020c0030 | r1 = row | **Done:** `ClearNew(row)` fires on opening a game. Rows map as tab + 4 × position. |
| ~~`IsAltVariant` = the second menu row of a game~~ **Done 2026-10-15** | `dScMgBase_c::IsAltVariant` 0x020ad660 (Amida overrides) | param1 read through PEEK | See Observed. The menu passes `MgEntry::sceneParam` as param1; its low byte is the variant. |
| ~~Wiggler draw passes A and B~~ **Done 2026-10-15** | `Wiggler_DrawTarget` 0x020ea914, `Wiggler_DrawField` 0x020eac38 | screen with each pass stubbed | See Observed: A draws the top-screen target, B the bottom-screen field. |
| ~~Trampoline2 item kinds 0..8~~ **Done** | `TrmpItem_SetMotionStill .. SpinDrift` | hit and kind | All nine kinds were forced and tracked (see below), and each motion's code fixes how the item moves (names.md, pass 5). |
| Pachinko's flyer, background scroller and hit bursts | `PachinkoFlyer_StateWaiting`, `PachinkoBg_Start`, `PachinkoFx_Spawn` (see ov006 symbols.txt) | hit timing | **Done:** the background is the airship that drops the Para-Bombs. |
| ~~Teresa's exclaim, found mark, rising icon and reveal sprites~~ **Done 2026-10-15** | `dScMgTeresa_c::Exclaim_Spawn`, `VanishPuff_Spawn`, `FoundNumber_Spawn`, `Reveal_StateStart` | hit timing | See Observed: the "!", the found-order digit, the vanish puff. `Reveal_StateStart` runs at each round start. |
| ~~The graph-callback hooks (slots 22-25)~~ **Done 2026-10-15 (from code)** | `dScMgBase_c::OnGraphCallback0..3`; arm9 `func_02019390` / `func_02019404` / `func_02019144` / `func_02019100` | call order within a frame | Read from the main loop (`func_020197b8`) and `IRQ::VBlankHandler`: 0 at frame start, 1 after the scene step, 2 in VBlank before the OAM/register commit, 3 at the end of VBlank. See `include/dScMgBase_c.h`. |
| Ten more medium names, bottom-up from names.md (Trampoline Marios, Heyho, Jump3D, Puff, Luigi's scanlines, the three unnamed functions, Trampoline2's spinner and dealer, Slot, the Jump phases, Cup) | `mgwatch.py --set next10` (91 symbols) | hit, args, lr | **Done**, except the Trampoline2 dealer, which never ran (see "Ten more" below). |

## Observed so far (melonDS 1.1, EU ROM, 2026-10-15)

Logged with `tools/trace/mg/mgwatch.py` (setup in that folder's README) plus window
screenshots, over two games of Wanted! (Luigi):
- `MiniGm_IsNew` polls menu rows 0-3 and 7 every frame; picking Wanted! ran `MiniGm_ClearNew(3)` once. The badge names are confirmed, and Wanted! is row 3.
- Banner 0xd, shown at (0x80, 0xa8) at each new level, is the "LEVEL n" text.
- State 0x12 shows banner 8, the "GAME OVER" text, with jingle 3.
- State 16's banner 7 at (0x80, 0x14) is the "HIGH SCORE" header over the record table.
- A new high score went through state 17. `OnNewRecord` fired 120 frames in, then banner 5 and jingle 1 played. A tied score went to state 16 with no `OnNewRecord`.
- `Mg_CallStartRound` passed the finished state's number (1). This confirms `StartRound(state)`.
- Banners 0 and 2, at (0x80, 0x6c) and (0x80, 0x9c) after the record table, are the PLAY AGAIN and QUIT buttons.
- Mario's page: the ladder game (Amida, menu row 0) and the propeller Shy Guy game (row 4) both open with banner 0xd "LEVEL 1" and end in state 0x12, then state 16. Banner 0xd is the shared level banner; each game places it (y 0xa8 in Wanted!, 0x60 in these two).
- Wario's slingshot game is Pachinko (`dScMgPachinko_c`, menu row 2). The flyer is Lakitu on his cloud crossing the top; `PachinkoFlyer_StateWaiting` runs every frame while its delay counts down. The four targets are the four flowers along the bottom: at game end `PachinkoFx_Spawn` fires at y 0xa0000 and x 0x38000/0x68000/0x98000/0xc8000, and the flowers are gone. It ends straight in state 16, then state 17 on a new record. Banner 5 is the big rainbow "HIGH SCORE".
- Snowball (Wario's page, menu row 6): banner 0xc is the red "START". At time up the game calls `Mg_SetNextBanner(0xe)` and then enters state 3: banner 0xe is "TIME IS UP" over PLAY AGAIN / QUIT. `IsAltVariant` is polled at start.
- The flower game "loves me, loves me not" (Yoshi's page, menu row 7): banners 0x10 and 0x13 alternate per petal and are "LOVES ME" (red) and "LOVES ME NOT" (purple). It ends in state 0xc, which shows PLAY AGAIN / QUIT side by side (banners 0 and 2 at x 0x40 and 0xc0) under the result. That state is now named `MG_STATE_RETRY_PROMPT_WIDE`; it was `CONTINUE_PROMPT`.
- The stub accepts one client per melonDS launch. A second attach after `D` timed out and left the game halted. Restart melonDS (and re-enable `[Instance0.Gdb]`, which melonDS can rewrite to false) before reattaching.
- State 1, explained: Luigi enters it only when `mLevel % 5 == 4`, so after every fifth level, and otherwise restarts the round directly. A four-level game never entered it. On screen it is the fade into level 6, with the poster halves split aside. `MG_STATE_NEXT_ROUND` is confirmed. A nine-level game showed it exactly once, between level 5's and level 6's banners.
- PLAY AGAIN after game over calls `Mg_CallStartRound` with 0x12, the finished state's number. Retry also goes through `StartRound(state)`.
- Toad's face slot game (`dScMgSlot3_c`, Mario's page 2, middle card) was played with taps timed by a colour tracker (`tools/trace/mg/slotbot.py`). Toad projects a target face, and three strips of caps, eyes and mouths scroll. A touch in DS y 0x40/0x58/0x70 stops that row on the next slot (`Slot3_StateSpin`). The run starts at 12 coins (`StartRound(3)`).
  - A miss gave three `Mg_SubCredits(1)` (`Slot3_StatePayout`, local state 4) and the counter went 12 → 9 → 6.
  - A Wario column on the right gave three `Mg_AddCredits(1)` (local state 3; the middle column pays 6) and the counter went 6 → 9. A star appeared by Toad (`mHudScore`, rounds won). Then `StartRound(4)`: banner 0xd, Yoshi as the new target, and a fourth face in each strip (`mStripLength` 3 → 4).
  - **`mCredits` is coins here**, moved one coin per tick. Slot3 settles a round in its own state machine and never calls `Mg_SetSharedState` for a win or a loss. Only an empty purse reaches `MG_STATE_GAME_OVER` (`Slot3_StateGameOver`).
- **Memory** (`dScMgMemory_c`, Luigi's page, top-left card, menu row 1) has eight cards and starts with 12 coins. Luigi deals, then:
  - `Dealer_StateShuffle` (was `Dealer_StateDeal`) calls `OnDealDone`, whose override is `MgCoins_PlaceBet(3)`. Three coins go onto the table, each via `Coin_Launch` → `Mg_LoseCredits(1)` → `Mg_SubCredits(1)`.
  - A miss slides one table coin over to Luigi and calls nothing watched.
  - **Clearing the board** with 2 coins left: `Mg_SetSharedState(4)` and banner 9, then twelve `Mg_GainCredits(1)` + `Mg_AddCredits(1)` pairs (the "+12" on screen), then `StartRound(4)` and a new deal and bet.
  - **Three misses:** `Mg_SetSharedState(5)` and banner 0xa. The board is revealed until touched, then `StartRound(5)` deals and bets again.
  - Memory passes banners 9 and 0xa at y −0x3c, and neither was visible on screen.
- **Picture Poker** (`dScMgCard_c`, Luigi's page, top-right card, menu row 9) also opens with `OnDealDone`. It stakes one coin itself, without `PlaceBet`, and the BET! button runs `MgCoins_AddBetCoin` for one more coin. The banners 0x18..0x1a carry the hand names.
  - **Luigi's full house against the player's two pair:** `Mg_SetSharedState(5)`, then `StartRound(5)`.
  - **Equal hands:** `Mg_SetSharedState(7)` and banner 0xb. One `Mg_GainCredits(1)` + `Mg_AddCredits(1)` refunds the bet, then `StartRound(7)`.
- **Roulette** (`dScMgRoulette_c`, Luigi's page, bottom-left card, menu row 0xd). Each spin, five coins go to the chip rack (`Mg_LoseCredits(1)` ×5). Chips not staked come back with `Mg_GainCredits(1)`.
  - **No chip staked:** payout 0 = stake 0, so `Mg_SetSharedState(8)`, silent, then `StartRound(8)` at once.
  - **One chip on black, and black came up:** `Mg_SetSharedState(4)` and banner 9, two coins paid, then `StartRound(4)`.
- **Sound** (`dScMgSound_c`, Yoshi's page, bottom-middle card, menu row 0x13) is six chests, three note pairs and three hearts (`mTries`, no credits). The pairs were read from `mNotes[].note` with the watcher's PEEK.
  - Each match plays `Presenter_PlayMatch`, and each miss `Presenter_PlayMiss` and a heart.
  - **Third miss:** `Presenter_PlayLose`, then `MG_STATE_GAME_OVER` (banner 8, jingle 3), then the record table (banner 7).
  - **Round cleared:** `Presenter_PlayCheer`, then `Mg_SetSharedState(0)` with banner 3 "CLEAR" and jingle 4, then `StartRound(0)` and banner 0xd. A round with no miss first calls `Mg_SetNextBanner(6)`, and banner 6 is the rainbow "PERFECT".
- **The menu table.** A game's menu row is its tab + 4 × its position on the page, with Mario 0, Luigi 1, Wario 2 and Yoshi 3. Rows 24..35 are each tab's second page. `MiniGm_ClearNew(row)` agrees for Memory 1, Card 9, Roulette 0xd, Sound 0x13 and Slot3 0x1f.
- So shared states 4, 5, 7 and 8 are seen: `MG_STATE_WIN`, `MG_STATE_LOSE`, `MG_STATE_DRAW` and `MG_STATE_TIE` all hold. Memory and Card never called a `Presenter_Play*`; their dealer has its own `Dealer_PlayWin/Lose`.
- **Sound, a second watch** (menu row 19). The beach has Mario's head as the presenter on the top screen; the hearts are drawn as `mTries + 1`, and stars count rounds cleared.
  - The first chest of a pair: `SpawnPops(slot)` (r1 = the chest), `Presenter_PlayCheerLoop` (r2 varies per note: 0x48, 0x49, 0x4f, 0xfa) and three `Pop00_Start`. Mario sings with his eyes shut while ♪ sprites rise. `IsAltVariant` is polled on every `SpawnPops`.
  - A mismatch: `Presenter_PlayMiss` (r2 0x12f) half a second after the second chest; one heart goes. A match: `Presenter_PlayMatch` (r2 0x26), and the pair disappears.
  - Round clear: `StartWinSpriteA`/`B` are the sparkle ring round "CLEAR" and a star at the top right. About 3.6 s later `Mg_CallStartRound(0)` runs `StartRound`, `ResetRound` and `DealNotes`.
  - The final miss: `Presenter_PlayMiss`, `RevealUnmatched`, then `Presenter_PlayLose`. On a new record, `dScMgSound_c::OnNewRecord` runs 2 s after banner 7 and plays `Presenter_PlayCheer` before banner 5 and jingle 1.
- **Cup (`dScMgCup_c`, "Where's Yoshi?", Yoshi's page, menu row 23)** is a shell game: three Thwomp-faced boxes, Yoshi hidden in one, 2 hearts. Banner 0xf is the "WHERE'S YOSHI?" prompt, shown as `StateWaitForInput` ends.
  - A right pick: `Presenter_PlayCheer` (r2 0x3c), then `Mg_SetSharedState(0, score)`: r1 is the score (1, 2, 3, ...). Banner 3 "CLEAR" goes up at y 0x88 with jingle 4, then `Mg_CallStartRound(0)` and banner 0xd "LEVEL n". The hearts are back to 2 at the next level.
  - A wrong pick with lives left: `Mg_SubCredits(1)`, then `Presenter_PlayMiss` (r2 0x96). **Cup never calls `Mg_CallStartRound` for a lost life**: it restarts the level itself (banner 0xd from `StartRound`, lr 0x020e0054). Nothing passed -1 or slot 18. `mCredits` is lives here.
  - The last wrong pick: `Mg_SubCredits(1)` and `Presenter_PlayLose` (r2 0x1e), then the same game-over path as Sound: state 0x12 (banner 8, jingle 3), banner 7, `dScMgCup_c::OnNewRecord`, `Presenter_PlayCheer`, banner 5 and jingle 1. The results end on the wide prompt (banners 0 and 2 at x 0x40 and 0xc0, y 0xa0).
  - From memory reads: `mIds` stayed 0, 1, 2 across shuffles, since the shuffle moves `mCup[mIds[i]]` and swaps them back. Yoshi sat in the left cup with `mCupContents` = {1, 0, 0}, `mTargetContent` = 1 and `mRevealCup` = 0 after a miss. Later levels hide a blue Yoshi as a decoy in another box, so `mCupContents` is the Yoshi colour in each cup and `mTargetContent` is the one asked for.
- Jingles by context: 4 = round CLEAR (state 0); 3 = GAME OVER (state 0x12); 1 = new HIGH SCORE (state 17). The audio itself was not heard.
- The GDB stub on the second CPU (ARM7, port 3346) can read main RAM while `mgwatch.py` holds the ARM9 port, but only once per launch, the same limit as the ARM9 stub.

### Bottom-up pass (2026-10-15, a second watcher)

A second melonDS ran alongside the first, taking the table from the bottom row up.
Input was stylus only (posted mouse messages), and memory was read and written
through the watcher's command queue.

- **The graph callbacks, by frame phase.** The arm9 main loop `func_020197b8` steps a phase word, `data_0209d50c`:
  - 2 calls `func_02019390` (callback 0). It returns before the OAM reset and the frame setup when the callback returns 0.
  - 3 calls `SpawnIfNecessary`, and 4 calls `func_02044120`, the scene update that runs `BeforeBehavior`.
  - 5 calls `func_02019404` (callback 1), 6 submits the frame (`data_0209d4f0 = 1`) and 7 waits for VBlank (`func_0201a4bc`).
  - `IRQ::VBlankHandler` calls `func_02019144` (callback 2) only when a frame was submitted and is due. It runs before `OAM::Flush` and the display-register writes, which are skipped when it returns 0. `func_02019100` (callback 3) runs at the end of every VBlank.
  - **Logged order, every frame:** `GraphCallback0` (lr 0x020193b4), `BeforeBehavior`, `GraphCallback1` (lr 0x02019430), `GraphCallback2` (lr 0x02019168), `GraphCallback3` (lr 0x02019124). A scene's first frame ran only callback 3.
  - **Candidate names, not applied:** 0 = frame start, 1 = after the scene update, 2 = VBlank before the OAM flush, 3 = VBlank end.
- **Teresa** (`dScMgTeresa_c`, Yoshi's page, top-right card, row 11). The top screen is Luigi with a flashlight. The bottom screen is dark until the stylus rubs it, and a 3-second clock starts at banner 0xc START. A bot scribbled over each unfound Boo, reading `mBoos` (4 × 0x90), and found one.
  - **A find:** `Exclaim_Spawn` (r1 0, r2 1, lr 0x0211ea20), then `FoundNumber_Spawn` (lr 0x0211e100), then `VanishPuff_Spawn` (lr 0x0211f090). Then `Mg_SetSharedState(0, 1)` (r1 is the score), banner 3 "CLEAR" at (0x80, −0x80) and jingle 4.
  - **Next level:** `StartRound(0)`, banner 0xd "LEVEL 2", `Reveal_StateStart` and START again. Level 2 has two Boos flying, and a star at the top right counts finds.
  - **A missed clock:** banner 8 from Teresa itself (lr 0x0211fe50), state 0x12 (GAME OVER, jingle 3), banner 7, then `dScMgTeresa_c::OnNewRecord` (lr 0x020b6e88), and banner 5, the rainbow "HIGH SCORE", across the top screen (y −0x80), with jingle 1.
  - **The sprites, from a screenshot burst on a second find.** The gaps are wall-clock times, slowed by the breakpoints.
    - `Exclaim_Spawn` fired as a red "!" appeared over the Boo on the dark bottom screen.
    - `FoundNumber_Spawn` came 1.6 s later, as a yellow "1", the find count, took the "!"'s place over the Boo. The room then lit up, the Boo faded to sparkles, and Luigi pointed on the top screen.
    - `VanishPuff_Spawn` came 3.3 s after that, 0.5 s before CLEAR. A small yellow star then appeared at the right edge of the top screen and stayed through the record table. A play with no find has no star, so the star counts finds.
    - **The star is not the found mark.** With a `bx lr` written over `VanishPuff_Spawn`'s first word, a find still showed the star, the bottom-screen sparkles and CLEAR. The word was then restored to the ROM's `00402de9`. What the found mark draws is still unidentified.
    - The "!" and the "1" are matched to their calls by timing only.
- **Pachinko's background** is the airship. `PachinkoBg_*` scrolls main-engine BG1 (0x04000018), and `PachinkoDeco_ShowAll`/`HideAll` bracket each pass:
  - It starts at x 0x100, y 0x40. State 1 moves it left and up to y 0, and state 2 carries it on left to x ≤ −0x90.
  - State 3 drops it to y ≥ 0x40, hides it, and waits 0x258 frames (10 s) before the next pass.
  - **On screen:** a dark winged airship hull crossing the top of the top screen. The `PachinkoDeco` sprites are its white wings, and it drops the Para-Bombs (parachute Bob-ombs).
  - `PachinkoBg_Start` fires after `Mg_CallStartRound(0x10)` at each round start (twice on a replay: lr 0x020fedb8 and 0x020fed50). The game starts on the card tap; "A RULES" is only an overlay.
  - With no input the flowers were all lost in about 10 s, then state 16.
- **Trampoline2** (`dScMgTrampoline2_c`, Mario's page 2, right card, menu row 0x20). Mario drops between castle towers onto an "M" pad, and the player draws trampolines with the stylus. The items are rainbow rings; there are 3 lives (Mario heads).
  - **Natural play** (a bot redrawing a full-width line) only showed kind 0, up to a score of 600.
  - **A new record** runs the base `dScMgBase_c::OnNewRecord` (no override): banner 5 at y −0x70, jingle 1. Game over is state 0x12 from lr 0x02123c3c.
  - **Item records:** the pool is `data_ov006_0214082c` (stride 0x1d0, 10 slots, `data_ov006_02140838`), and the live count is `data_ov006_02140828`. The fields:
    - +8/+0xc/+0x10 position; +0x2c/+0x30 velocity; +0x38 an orientation vector;
    - +0x96 size (model 0/1/2); +0x98 the motion kind.
  - **In `SpawnWave`,** `faces[i]` is the motion kind and `kinds[i]` is the position index; the local names read backwards.
  - **Each kind was forced:** write +0x98, zero the velocity, and send the item back through `TrmpItem_StateAppear` (the PMF at +0). Then its position was tracked:
    - 0 stays still; 1 slides side to side between ± a bound from the size table (`data_ov006_0212e07c`/`088`); 2 rises and falls (y −0x60..0x80, vy ±0x1000).
    - 3 spins in place in the screen plane (0x100 a frame); 4 sways side to side, reversing on a phase timer (+0x9a += 0x40), faster the further it is from y 0x74; 5 is 4's sway plus the spin.
    - 6 bobs up and down on a timer (vy ±0xc00); 7 is 6's bob plus the spin; 8 spins in place like 3, on its own fixed axes (`SetMotionSpin`, kind 3, also zeroes vy).
  - **`SetMotion*` does not clear the velocity.** A forced ring kept its old one and flew off-screen, which held the live count at 1 and blocked respawns. It was retired through `TrmpItem_GrowScale`, which decrements the live count.
- **Hanachan's draw passes** (`dScMgHanachan_c`, Yoshi's page 2, left card, row 27). The top screen shows one straight reference wiggler, and the bottom screen three wandering wigglers.
  - **Calls per frame:** `Wiggler_DrawTarget` once (r0 0x21893b8, r1 0x12, lr 0x020ed0cc), and `Wiggler_DrawField` once per wiggler (r1 0, 6 and 0xc, lr 0x020ecf68).
  - **Placement:** A draws at fixed points with `OAM::Render(0, ...)`. B draws at each wiggler's own segment positions (+0x18 + i × 8) with `OAM::Render(1, ...)`.
  - **Stubbing each one** (a `bx lr` written at its entry, then the original word restored) removed the top-screen reference for A and all three bottom-screen wigglers for B. **A is the reference wiggler (main engine), B the playfield wigglers (sub engine).**
  - The clock ran out to GAME OVER.
- **`IsAltVariant` and the menu rows.** `dScene_c::StartSceneFade(sMiniGmRows[sel].id, sMiniGmRows[sel].unk4, 0)` hands a row's second word to the scene as `param1`.
  - **The table** is `sMiniGmRows` (ov005 0x020c24d8, 36 rows of 0x34). Byte 0 of that word is the variant: 2 on row 24, 1 on rows 20, 22, 25, 26, 28-30 and 32-35, and 0 on the rest.
  - **The six duplicated scenes are one game in two variants:** Amida (rows 0 and 24), Smartball (10, 22), Snowball (6, 34), Coin (14, 30), Panel (15, 35) and Slot1 (17, 33). Each second row is variant 1, except Amida's, which is 2.
  - **Variant 1 also marks the single-row second classes:** Jump2 (20), MCarlo2 (25), Tamaire (26), Curling_J (28), Memory_J (29) and Trampoline2 (32).
  - **Sound** has one row (19, variant 0), so its `IsAltVariant` is always false from the menu.
  - **Amida's override asks `== 1`,** which neither Amida row sets. Its own slot 36, `IsVariant2` (`== 2`), is what tells row 24 apart.
  - **Seen:** Panel's second row (35) gave `param1` a low byte of 1, and `IsAltVariant` was polled every frame (lr 0x021063bc and 0x021061f4).
    - That board is a tall six-column grid of gold mushroom and brown leaf tiles spanning both screens, with golden statues at the sides.
    - Tapping every tile gave state 0 (r1 2), CLEAR and "LEVEL 2". A timeout then went to GAME OVER and the base `OnNewRecord`.
- **Memory_J** (`dScMgMemory2_c`, Luigi's page 2, middle card, row 0x1d) has 16 cards and a 15-coin purse.
  - **The bet:** after the deal, `dScMgMemory2_c::OnDealDone` calls `MgCoins_PlaceBet(5)`. Five coins go onto the table by five `Mg_LoseCredits(1)` + `Mg_SubCredits(1)` pairs (15 → 10), and each miss takes one away.
  - **A lost round** is state 5 (r1 8; banner 0xa at y −0x3c, off screen). `StartRound(5)` then deals and bets 5 again.
  - **With the purse empty,** losing the round sends `Lose_Update` to `dMgState_c::UpdateGameOverToMenu`: banner 8 and jingle 3, then straight back to the menu with no record table.
  - **The NEW! badge:** the card had it before this one play and not after.
- **MCarlo2** (`dScMgMCarlo2_c`, Luigi's page 2, left card, row 0x19) keeps the base `OnDealDone` and stakes nothing; its card shows no coin count. Luigi deals a 5 × 4 grid face up. Tapping two adjacent matching faces removes them, and the deck refills the gaps; the counter at the top right counts the cards taken (2 after one pair).
  - **The end:** a brute-force tap sweep played it to 110. Then came `Mg_SetSharedState(0x12)` (lr 0x020fa364), banner 8 "GAME OVER" and jingle 3. After it, banner 7, the base `dScMgBase_c::OnNewRecord` (no override), banner 5 and jingle 1 over the record table, and PLAY AGAIN / QUIT. The card then showed 110.

### Ten more (2026-10-15, the same second watcher)

The next ten open items from the bottom of names.md's per-game section, each
played once or more with the watcher's `next10` set. Candidate names are in
names.md; nothing was renamed.

1. **Trampoline Marios** (Trampoline, row 12; Trampoline2, row 32).
   - **`BounceOffLineA/B`:** `Trmp3DMario_State7` dispatches on the u16 at +0x18; the logged r1 was that value, not an argument. The value comes from the line: as it settles, `TrmpLine` picks the jumper under it and stores 2 if |offset| × half-length < 0x400000 (sound 0x1b1), else 1 (sound 0x1ae).
     - 2 → A → State4 (particles 0xf3-0xf5, sound 0x1b5); 1 → B → State5. So A is a catch near the line's centre and B one further out.
     - B ran 22 times to A's 2, with slanted and flat lines alike (`trmp_slant.py`), so slope is not the split.
   - **Polled every frame:** `CountFinished` (scene lr 0x02121878) and `SetSpawnHeight` (r0 0x20, lr 0x02121bfc). Game over is state 0x12 from lr 0x0212188c, right after.
   - **`EnterByKind`** ran once for each of the four Marios at game end (lr 0x020cd234).
   - **`func_ov006_020ca840`** (now `Trmp2Mario_EnterAll`) ran once, at Trampoline2's game over (lr 0x02123c84), then `Trmp2Mario_EnterByKind` for the one Mario and state 0x12.
2. **Heyho** (Jump2, row 20; Jump, row 4). Each landing was `Motion11` → `Settle` (a white puff, score +1). In Jump2 that is followed by `Jump2Popup_Spawn` and `SettleAll` → `Motion8` (the next Shy Guy flies in from above).
   - A 24-shot burst showed the popup as a yellow "1", then "2", rising from the hit spot.
   - `AllIdle` gates GAME OVER from `State_Result`.
   - Jump called `Settle` without a popup and never called `SettleAll`.
3. **Jump3D.**
   - The `EnableInput` flag draws the blue arrows over Jump's Marios. Jump sets it and has the arrows; Jump2 clears it and has none.
   - `SetLowGravity` ran at `State_End` → `State_Result` in both games. `StopGravity` (only `BeginFinal`) was not reached.
4. **Puff** is the cyan "MISSED n" label. Its three spawns line up with the three misses in a 24-shot Jump2 burst and a 16-shot Jump burst ("MISSED 1", "MISSED 2", then "MISSED 3", which stays). The earlier reading of a smoke column in Trampoline, made from one screenshot, was wrong and was not recorded.
5. **Luigi's scanlines** (Wanted!, row 3). Nothing ran until the clock hit 0. Then `PrepareScanlines` fired from the IRQ dispatcher (lr 0x01ffd9d4), and `PushScanlineRecord` from it on lines 0xcb-0xdb, as the iris opened from the centre circle to the full poster wall.
6. **The three unnamed functions.**
   - Teresa's fired once as the clock ran out (lr 0x0211e69c).
   - Bomroom's fired once (lr 0x020d8b50), 3 s before state 16. Four bombs were dragged to the black pen. The next shot showed the board empty apart from one bomb in the top door, and the round ended at score 0.
   - Pachinko's runs every frame of play (lr 0x020feef8), and the slingshot's cradle at (0x80, 0x28) is where it places the next ball.
7. **Trampoline2's spinner and dealer.**
   - **Spinner:** twelve `Trmp2Spinner_SetModel` calls at load (r1 0x8000, lr 0x02122c40), and `HideAll` at setup.
     - Stubbing `Trmp2Spinner_UpdateAll` (0x02122ab8, `bx lr`; original word `f0402de9` restored and read back) did not stop the background shooting stars.
     - Three slots written live (+0x50: scale 0x6000, position (x, 0x100000, 0x18000), velocity (−0x1c00, −0x1c00, 0), life 0xf0) showed large white stars spinning off the top screen to the lower left.
     - In play, a spinner is spawned per ring taken (`TrmpItem_UpdateWithJumper` → `Trmp2Spinner_InitAll`).
   - **Dealer:** `Trmp2_PickFaceA/B`, `OccupancyBits` and `ColumnForKind` never ran. `SpawnWave` was not reached in this play, so the dealer reading in names.md is from code.
8. **Slot1** (row 17).
   - **Setup:** `SlotBet_Place`, `Slot1_InitHud`, then `SlotBet_Reset`.
   - **Betting:** three `SlotBet_OnTouch` (one from the card tap, two from BET; 10 → 7 coins).
   - **The spin:** tapping the three reels stopped them on three feathers. `SlotReels_CheckStopped` ran as the third stopped (lr 0x0210cd4c). 2.7 s later `SlotReel_SetSpeed(icon, 5)` ran (lr 0x0210ceec), and the credits counted up to 22.
9. **The Jump phases** (`dScMgJump_c`, `dScMgJump2_c`). In both games:
   - **Timeline:** `BeginIntro` at setup, `BeginPlay` 2.8 s later, `BeginEnd` after the third miss, `State_End` for 0.5 s, then `State_Result` for 2-3 s. Then state 0x12 (Jump lr 0x020ee414, Jump2 0x020ef4b4) and `State_Idle`.
   - **Jump's intro** shows banner 0xd (lr 0x020ee644).
   - `BeginFinal`/`State_Final` were not reached.
10. **Cup** (row 23, "WHERE'S YOSHI?": three crates). `Cup_PickShuffleSpeed` ran 8 times per round (16 over two lost rounds), all from `StatePrepareShuffle` (lr 0x020dfd0c), once per swap.

## Recording the answer

For each row, put the observed meaning in names.md's row and raise or lower the
confidence. A changed name goes through the same in-place rename the pass used:
- symbols.txt, the manifests, attribution and `symbols/actor_renames.tsv` lineage
  rows for mangled members
- then a TU object compare (identical modulo the rename) and the usual gates
- **Teresa (Hide and Boo Seek), 2026-10-15.** The bottom screen goes dark while the Boo keeps flying, and the top screen counts down (TIME 3, 2, 1). Rubbing clears the dark (`UpdateRubbing` draws colour-0 lines into sub BG0), and `TouchTestBoo` finds the Boo once its centre and one of the four points at +/-6 px are cleared. Played with a bot that read the Boo records through the watcher's PEEK file and held the stylus on the Boo. On a find, in order: `Exclaim_Spawn` (a red "!" over the Boo), `FoundNumber_Spawn` (the digit 1 rises off it), `VanishPuff_Spawn` (a puff where it vanishes, Luigi waving his flashlight on top), then shared state 0: banner 3 "CLEAR", jingle 4, `StartRound(0)` and the next level. `Reveal_StateStart` runs at each round start, with banner 0xd at (0x80, 0xa8) and banner 0xc at (0x80, 0x40). A miss ends with Teresa's own banner 8 and state 18 (GAME OVER); a 1-point run that beat the record went through `OnNewRecord`, banner 5 and jingle 1 (state 17).
- **IsAltVariant, 2026-10-15.** The ordinary Snowball (Wario's page, row 6) and the penguin Snowball (Wario's second page, row 34) share scene id 377. Read through PEEK, their param1 words are 0x0a00 and 0x2201: low byte 0 and 1, which is what `IsAltVariant` tests. `dScMiniGm_c` starts a game with `StartSceneFade(id, sceneParam, 0)` from its `sMiniGmRows` entry, and the table lists six games twice: Amida (rows 0/24), Snowball (6/34), Smartball (10/22), Coin (14/30), Panel (15/35) and Slot1 (17/33), each with low byte 0 first and nonzero second (Amida's second is 2, matching its `== 1` override). Sound calls `IsAltVariant` but its only entry (row 19) has 0, so that branch is not reached from the Rec Room; Smartball and Slot1 have second entries but do not call it. The penguin Snowball is pushed up the slope against a 40-second clock (any upward drag adds to the ball's velocity; downward drags brake).
- **Which Wiggler? (Hanachan, Yoshi's second page, row 27), 2026-10-15.** The watcher's new POKE file wrote `bx lr` (0xe12fff1e) over each draw pass's first instruction and restored it after one screenshot. With `Wiggler_DrawField` (was pass B) stubbed, all three Wigglers on the bottom play field vanished and the top one stayed; with `Wiggler_DrawTarget` (was pass A) stubbed, the target Wiggler on the top screen vanished. The scene calls A with `mTarget` and B for each field Wiggler.
- **Snowball's finish (state 19), 2026-10-15.** Pushed up the course by a bot (`snowbot.py`: repeated upward swipes, reading the ball through PEEK), the ball got from y ~8000 to ~2560 before time ran out; three obstacle hits cost about 3 s each. To see the finish, the ball's y (+0xab3c) was then POKEd to 0x200000, just short of the goal row (+0xba04 = 0x190), and pushed over by the bot: the run reached phase 2, and 0x3c frames later `Mg_SetNextBanner(4)` and shared state 19 ran. The screen shows "GOAL" in rainbow letters over a checkered finish line (banner 4), jingle 4 plays, and, as the time beat the record, `OnNewRecord`, jingle 1 and the rainbow "HIGH SCORE" (banner 5) of state 17 follow, over the record table.
- **Trampoline2's results-screen stars, 2026-10-16.** With POKE writing `bx lr` over `TrmpItem_UpdateMatrix` (0x020ce988), the coloured shooting stars over the record table kept falling and kept their trails, so they are not TrmpItems; the stub was restored (read back as 0xe92d4010). Which item sprite goes with which motion is still unobserved.
- **Bomroom's fixtures, 2026-10-16.** Bomroom is `MG_BOMROOM` (Mario's page 1, top-right card). During a game, POKE wrote `bx lr` over each of four draw functions in turn, took one screenshot, and restored the original word (each read back unchanged). Stubbing `SlotA_DrawAll` hid the two green panels on either side of the bomb hatch at the top of the bottom screen. `SlotB_DrawAll` hid the green gate strips at the left and right screen edges, beside the red and black pens. `SlotB2_Draw` hid the emblem door at the top centre of the top screen. `Marker_DrawAll` changed nothing, because no marker was active. The three groups are now `HatchPanel_*`, `PenGate_*` and `TopDoor_*` (names.md, Bomroom). A game left alone ends within about a minute, as unsorted bombs go off.
