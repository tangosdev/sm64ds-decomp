# Active claims

Claim a module or address range here before you start matching, so two people (or
two AI sessions) do not grind the same functions. The batch tools are range-scoped,
so one claimed range per worker keeps everyone on disjoint work by construction.

## How to claim

1. Pick a free module or address range that is not listed below.
2. Add a row to the table: the range, your name/handle, today's date, and status.
3. Commit it on its own (`Claim ov0XX`) and push, or open a small PR. Do this before
   you start matching.
4. When you are done or stop, change the status to `done` or remove your row.

Keep one active range per worker. If a range has not moved in a couple of weeks,
it is fair to take over: ping the claimant first.

## Claims

| Range | Who | Claimed | Status |
|---|---|---|---|
| main (arm9): 7 funcs — 02048234, 02048720, 020490b0, 020494cc, 0204bbd8, 0204be40, 0204c304 (also locked via claims api) | ai-tdd-labs (claude-fable) | 2026-07-16 | released (api locks freed; no matches landed) |
| ov004: func_ov004_020af2f8 (0x020af2f8, size 0x2e8) | lunavyqo (Grok) | 2026-07-16 | **done** — verified byte-identical (mwccarm 1.2/sp2p3); claim clm_a25f174bbe49 kept active |
| ov006 func_ov006_020fc8c0 (0x020fc8c0, size 0xf0) | Codex/Raman | 2026-07-16 | active — batch 11, free div=6 refinement |
| arm9 func_02069918 (0x02069918, size 0x7c) | Codex/Lovelace | 2026-07-16 | released — required base-copy is optimized away; DB-best restored, no tracked change |
| ov007 func_ov007_020b782c (0x020b782c, size 0xa4) | Codex/Lovelace | 2026-07-16 | active — batch 11 replacement, free div=8 refinement |
| ov007 func_ov007_020c1448 (0x020c1448, size 0x70) | Codex/Mendel | 2026-07-16 | active — batch 11, free div=8 refinement |
| ov006 func_ov006_0211b0ec (0x0211b0ec, size 0x90) | Codex | 2026-07-16 | done — exact 144 bytes, strict relocs + linkcheck VERIFIED; PR #382 |
| ov007 func_ov007_020c3598 (0x020c3598, size 0xf4) | Codex | 2026-07-16 | active — batch 11 replacement, free div=7 refinement |
| arm9 func_020453c0 (0x020453c0, size 0xbc) | Codex/Raman | 2026-07-16 | done — exact 188 bytes, strict relocs + linkcheck VERIFIED; PR #379 |
| ov002 func_ov002_020f5990 (0x020f5990, size 0xdc) | Codex/Lovelace | 2026-07-16 | released — DB-best div=5 confirmed argument-register coloring floor; no tracked change |
| ov006 MgBounceAndPounce_Spawn (0x020eeafc, size 0xec) | Codex/Mendel | 2026-07-16 | done — exact 236 bytes, strict relocs + linkcheck VERIFIED; PR #380 |
| ov006 func_ov006_020c6088 (0x020c6088, size 0x100) | Codex | 2026-07-16 | done — stale DB entry already byte-exact; canonical calls close BLIND-2; PR #378 |
| ov006 func_ov006_020effb8 (0x020effb8, size 0x74) | Codex/Raman | 2026-07-16 | done — exact 116 bytes, strict relocs + linkcheck VERIFIED; PR #376 |
| arm9 func_0205d688 (0x0205d688, size 0x8c) | Codex/Lovelace | 2026-07-16 | released — DB-best div=5 confirmed allocator floor; semantic correction documented, no tracked change |
| ov006 func_ov006_020d1958 (0x020d1958, size 0xe4) | Codex/Mendel | 2026-07-16 | done — exact 228 bytes, strict relocs + linkcheck VERIFIED; PR #377 |
| ov075 func_ov075_0211a948 (0x0211a948, size 0xb8) | Codex | 2026-07-16 | released — documented pure register-coloring floor (~150 prior variants); DB-best restored |
| ov006 func_ov006_020c7734 (0x020c7734, size 0x12c) | Codex | 2026-07-16 | released — stale DB entry; existing src from PR #338 independently strict-matches and linkcheck VERIFIED |
| ov006 func_ov006_0211aed0 (0x0211aed0, size 0x90) | Codex | 2026-07-16 | released — DB-best div=6 confirmed three-register coloring floor; no tracked source change |
| arm9 func_02058568 (0x02058568, size 0x64) | Codex/Lovelace | 2026-07-16 | released — DB-best div=5 confirmed hand-asm/context register floor; no tracked source change |
| arm9 _ZN8CapEnemy11GetCapStateEv (0x02005fa0, size 0xb4) | Codex | 2026-07-16 | released — DB-best confirmed four-word scheduling floor; no tracked source change |
| ov006 func_ov006_0211dad0 (0x0211dad0, size 0xac) | Codex/Raman | 2026-07-16 | released — DB-best raw div=3 confirmed literal-load scheduling floor; no tracked source change |
| arm9 func_02068dc8 (0x02068dc8, size 0x7c) | Codex/Lovelace | 2026-07-16 | released — DB-best div=4 confirmed r3/r0 register-coloring floor; no tracked source change |
| arm9 func_02058df4 (0x02058df4, size 0xac) | Codex/Mendel | 2026-07-16 | done — NitroSDK arena-high function exact 172 bytes, linkcheck blind=0; PR #375 |
| ov004 func_ov004_020ad878 (0x020ad878, size 0x40) | Codex/Lovelace | 2026-07-16 | released — existing src independently confirmed exact + linkcheck VERIFIED; no change needed |
| ov005 func_ov005_020bfefc (0x020bfefc, size 0x50) | Codex/Mendel | 2026-07-16 | done — exact 80 bytes with canonical relocation targets, linkcheck blind=0; PR #374 |
| ov007 func_ov007_020adb84 (0x020adb84, size 0x44) | Codex | 2026-07-16 | released — existing src independently confirmed exact + linkcheck VERIFIED; no change needed |
| ov003 func_ov003_020ad69c (0x020ad69c, size 0x50) | Codex | 2026-07-16 | done — 80-byte exact match with verified relocation targets; PR #372 |
| ov007 func_ov007_020c6550 (0x020c6550, size 0x6c) | Codex/Raman | 2026-07-16 | released — DB-best div=3 confirmed register-allocation floor; no tracked source change |
| ov071 _ZN6Coffin8BehaviorEv (0x021224cc, size 0x90) | Codex/Lovelace | 2026-07-16 | done — 144-byte exact match, strict relocs + linkcheck VERIFIED; PR #371 |
| ov074 _ZN8Goomboss16CleanupResourcesEv (0x02121abc, size 0xb4) | Codex/Mendel | 2026-07-16 | done — 180-byte exact match, strict relocs + linkcheck VERIFIED; PR #373 |
| ov060 func_ov060_021151d4 (0x021151d4, size 0x140) | Codex | 2026-07-16 | released — existing DB-best div=3 confirmed materialized-base floor; no tracked source change |
| ov095: func_ov095_021357d8 (0x021357d8), func_ov095_021358cc (0x021358cc), func_ov095_02135cdc (0x02135cdc), UpDownLiftBbh::InitResources (0x021365d8), Flamethrower::Behavior (0x021368f0), Flamethrower::InitResources (0x02136d60) | lunavyqo | 2026-07-12 | done (partial) — 357d8 + UpDownLift InitResources MATCH (PR #305); 35cdc near-miss div≈40 in DB; 358cc/Flamethrower still open |
| ov019 func_ov019_02111558 (0x02111558, size 0x1fc) | lunavyqo | 2026-07-12 | done - verified byte-identical, draft PR |
| _example: ov004 0x020b0000-0x020b8000_ | _handle_ | _2026-06-17_ | _example_ |
| ov029 8 funcs (0x02111254-0x02112354) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #221 open |
| ov032: __sinit_ov032_02112c10 (0x02112c10), 02111254 (0x02111254), 02111620 (0x02111620), 02111830 (0x02111830), 02111e24 (0x02111e24) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #222 open |
| ov094: _ZN10HootTheOwl13InitResourcesEv (0x02136634), 021359d8 (0x021359d8), 02136024 (0x02136024), 021362e0 (0x021362e0) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #223 open |
| ov018: 02111368 (0x02111368), 021116b4 (0x021116b4), 02111804 (0x02111804), 02111bf0 (0x02111bf0) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #224 open |
| ov014: _ZN10ChainChomp13InitResourcesEv (0x02112b14), 021115ec (0x021115ec), 02111b70 (0x02111b70) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #225 open |
| ov025: _ZN11PyramidLift13InitResourcesEv (0x02112498), _ZN11PyramidLift8BehaviorEv (0x02112288), 021119f4 (0x021119f4), 02111a84 (0x02111a84), 02112288, 02112498 | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #226 open |
| ov015: _ZN11FallBlockWf13InitResourcesEv (0x02112dbc), _ZN11FallBlockWf16CleanupResourcesEv (0x02112da8), 02111eec (0x02111eec), 021128f8 (0x021128f8), 02112c84 (0x02112c84) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #227 open |
| ov019: 0211131c (0x0211131c), 021117a8 (0x021117a8), 02111dec (0x02111dec), 02111fec (0x02111fec) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #228 open |
| ov096: 02135948 (0x02135948), 02136264 (0x02136264), 02136434 (0x02136434), 02136754 (0x02136754), 02136fd4 (0x02136fd4), 02137088 (0x02137088), 021372c0 (0x021372c0) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #230 open |
| ov016: 02111284 (0x02111284), 021115c0 (0x021115c0), 02112fa8 (0x02112fa8) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #231 open |
| ov077: 021241ac (0x021241ac), 021243c0 (0x021243c0), 021256b4 (0x021256b4), 02125bb4 (0x02125bb4), 02126300 (0x02126300), 0212679c (0x0212679c) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #232 open |
| ov070: _ZN6FlyGuy8BehaviorEv (0x02120210), 0211f368 (0x0211f368), 0212156c (0x0212156c) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #233 open |
| ov005: 020c0140 (0x020c0140), 020c0878 (0x020c0878), 020c14a0 (0x020c14a0) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #236 open |
| ov030: _ZN9UkikiCage13InitResourcesEv (0x02111638), _ZN9UkikiCage16CleanupResourcesEv (0x02111610), _ZN9UkikiCage8BehaviorEv (0x02111624), 02111b20 (0x02111b20), 02112da0 (0x02112da0), 02113094 (0x02113094) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #237 open |
| ov071: 0211f8d0 (0x0211f8d0), 0212070c (0x0212070c) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #238 open |
| ov081 14 funcs (0x02123910-0x02127e1c) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #240 open |
| ov021: _ZN10ShutterHmc13InitResourcesEv (0x02112ec0), _ZN10ShutterHmc16CleanupResourcesEv (0x02112e68) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #241 open |
| ov020: _ZN15BookShotSpawner8BehaviorEv (0x02112418), 02111aa8 (0x02111aa8) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #242 open |
| ov066: 0211603c (0x0211603c), 02116390 (0x02116390), 02116db0 (0x02116db0), 021171b0 (0x021171b0), 02118188 (0x02118188), 0211a2e4 (0x0211a2e4) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #243 open |
| ov021 _ZN12WorkElevator13InitResourcesEv (0x02111c6c, size 0x258) | lunavyqo | 2026-07-10 | near-miss (PR #247 open) |
| ov021 _ZN12WorkElevator8BehaviorEv (0x021116c8, size 0x5a4) | lunavyqo | 2026-07-10 | near-miss (PR #249 open) |
| ov078 10 funcs (0x021238ac-0x02126660) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #250 open |
| ov089: 02130fb4 (0x02130fb4), 02131df4 (0x02131df4) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #251 merged |
| ov034: 02112020 (0x02112020), 02112484 (0x02112484) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #253 open |
| ov073: 0211f144 (0x0211f144), 0211f494 (0x0211f494), 0211fa74 (0x0211fa74), 0212128c (0x0212128c) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #256 open |
| ov073: 0211f2c0 (0x0211f2c0), 02120ed0 (0x02120ed0) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #260 open |
| ov072 10 funcs (0x0211f1dc-0x021218dc) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #235 open |
| ov015: _ZN13PoleBillboard8BehaviorEv (0x02111720), _ZN14MovingBarSmall8BehaviorEv (0x021124d0), 021114f0 (0x021114f0) | lunavyqo | 2026-07-10 | near-miss (PR #229 open) |
| ov070 func_ov070_0211f48c (0x0211f48c, size 0x164) | lunavyqo | 2026-07-10 | near-miss (PR #234 open) |
| ov005: 020c0378 (0x020c0378), 020c1130 (0x020c1130), 020c16e4 (0x020c16e4) | lunavyqo | 2026-07-10 | **partial done** — 020c1130 MATCH (PR #385); 020c0378 + 020c16e4 still near-miss (banked in nearmiss/db.jsonl; old PR #239 stale) |
| ov020: 02111340 (0x02111340), 021115ac (0x021115ac) | lunavyqo | 2026-07-10 | near-miss (PR #244 closed; 02113148 matched by ai-tdd-labs in #471) |
| ov089: _ZN3Key8BehaviorEv (0x02132194), 021311c0 (0x021311c0), 0213162c (0x0213162c), 02131b18 (0x02131b18) | lunavyqo | 2026-07-10 | near-miss (in main, refine-tier); PR #245 closed - matches split to #251 |
| ov092: _ZN6ToxBox13InitResourcesEv (0x02131da0), _ZN6ToxBox8BehaviorEv (0x02131c44), 02131010 (0x02131010), 021311b0 (0x021311b0), 021316d8 (0x021316d8), 02131878 (0x02131878) | lunavyqo | 2026-07-10 | near-miss (PR #246 open) |
| ov021: _ZN11RollingRock8BehaviorEv (0x02112854), 02112544 (0x02112544) | lunavyqo | 2026-07-10 | near-miss (PR #248 open) |
| ov091: _ZN22RotatingUpDownPlatform13InitResourcesEv (0x0213220c), _ZN6Thwomp8BehaviorEv (0x02132ab0), 02131160 (0x02131160), 02131db8 (0x02131db8), 02133098 (0x02133098), 021334b8 (0x021334b8), 02133c6c (0x02133c6c) | lunavyqo | 2026-07-10 | near-miss (PR #254 open) |
| ov073: 0211f2c0 (0x0211f2c0), 02120ed0 (0x02120ed0) | lunavyqo | 2026-07-10 | done - verified byte-identical, PR #260 open (supersedes near-miss #257) |
| AI-assisted crack sweep: smallest untried funcs (~0x100 band), spread across modules (this batch mostly ov006/ov007) | beansntoast (AI-assisted) | 2026-06-29 | in progress |
| ov002 __sinit_ov002_021019d0 (0x021019d0, size 0x5470) | Cursor/Grok | 2026-07-02 | done |
| ov001 func_ov001_020ab550 (0x020ab550, size 0x60) | Cursor/Grok | 2026-07-02 | done |
| ov002 func_ov002_020ec0a4 (0x020ec0a4, size 0x134) | Cursor/Grok | 2026-07-02 | done |
| arm9 func_02008080 (0x02008080, size 0x30) | Cursor/Grok | 2026-07-02 | done |
| ov085 func_ov085_0212aaec (0x0212aaec, size 0x150) | Cursor/Grok | 2026-07-02 | done |
| Unclaimed coddog siblings (arm9/ov002/ov004, ~0x30–0x80 band) | Cursor/Grok | 2026-07-02 | in progress — param-clone crack; handoff scratch/HANDOFF_matches_2026-07-02.md |
| func_ov006_021173c8 (0x021173c8, 0x10c0) + func_02009e70 (0x02009e70, 0x109c) | Cursor/Grok | 2026-07-02 | paused — v10 best NOT MATCH; sp40@0x7c blocker; scratch/HANDOFF_ov006_021173c8.md |
| _ZN5Stage9PS_UpdateEv (0x0202635c, 0x30ac) + func_ov006_02115b0c (0x02115b0c, 0x18bc) + func_0206a928 (0x0206a928, 0x1360) | Cursor/Grok | 2026-07-02 | paused — large funcs; scratch/HANDOFF_batch_4funcs.md (func_ov006_02112ad8 matched by andrewboudreau in #486) |
| ov002: func_ov002_020f3310, 020f3d98, 020f562c, 020f5848 (0x4c each) | Cursor/Grok | 2026-07-02 | done - all four verified byte-identical |
| ov006: func_ov006_0211a4b0 (andrew, PR #74) + 0211a648/0211a7ac/0211aa44/0211abdc/0211ad44/0211af60/0211b17c (Cursor/Grok) | mixed | 2026-07-03 | done - all eight verified byte-identical |
| arm9: func_02056674 (0x02056674, 0x68) | (already matched earlier) | 2026-07-02 | done previously; Grok edit was a no-op and was reverted |
| arm9 _ZN5Stage9PS_UpdateEv (0x0202635c, 0x30ac) | lunavyqo (AI-assisted) | 2026-07-10 | **active** mm=2 (was 69): fixed sel3_0 wrong-global f1ec→f2c4, stale-tmpv backlight guard flipped whole tx/ty cascade, direct e40 reload at opt_okback; residual = case-1 preheader ldr sb/r4 emission order (+0x7ac/+0x7b0); permuter hunting; handoff `scratch/HANDOFF_Stage_PS_Update.md` |
| batch2 NONMATCHING near-misses: func_ov002_020bf224, func_ov007_020bc3dc, func_0205d304, _ZN16MeshColliderBase6EnableEP5Actor, func_0206e3dc | ruspecial (Claude-assisted) | 2026-07-06 | near-miss div=2-3 (regperm/predicated-order/schedule walls) |
| batch2 matched: func_ov062_021161a8, func_ov062_02119628, func_ov020_021112b0, func_ov006_0210d6b8 | ruspecial (Claude-assisted) | 2026-07-06 | done - 4 verified byte-identical |
| batch3 NONMATCHING near-misses: func_ov079_02126a84 (div2), func_ov095_021365d8 (div3) | ruspecial (Claude-assisted) | 2026-07-06 | 021365d8 / UpDownLiftBbh::InitResources matched in PR #305 (`/2` average); 02126a84 still scheduling residual |
| batch3 matched: func_ov078_02125c98, func_ov060_021146d0, func_ov004_020b7cd0, _ZN3HUD15RenderCoinCountEv | ruspecial (Claude-assisted) | 2026-07-06 | done - 4 verified byte-identical |
| ov001 func_ov001_020aa420 (0x020aa420, size 0x290) | lunavyqo (AI-assisted) | 2026-07-06 | done - verified byte-identical |
| ov001 func_ov001_020aadac (0x020aadac-0x020aaf40) | lunavyqo | 2026-07-06 | done - verified byte-identical |
| ov001 func_ov001_020ab110 (0x020ab110, size 0x118) | lunavyqo | 2026-07-06 | done - verified byte-identical |
| ov017 func_ov017_021112c4 (0x021112c4-0x021113c0) | lunavyqo | 2026-07-07 | done - verified byte-identical |
| ov014 func_ov014_02111a6c (0x02111a6c, size 0x84) | lunavyqo | 2026-07-08 | done - verified byte-identical |
| ov023 func_ov023_021113b0 (0x021113b0-0x02111670) | lunavyqo | 2026-07-09 | done - verified byte-identical, PR #159 open; API claim kept active |
| ov043 small wrappers: func_ov043_021114b0, 021115cc, 021115e0, 02111744, 02111758 | lunavyqo | 2026-07-08 | done - verified byte-identical, PR open |
| ov035 func_ov035_0211195c (0x0211195c-0x02111a98) | lunavyqo | 2026-07-08 | done - verified byte-identical |
| ov013 func_ov013_021112a8 (0x021112a8-0x0211133c) | lunavyqo | 2026-07-08 | done - verified byte-identical |
| ov002 _ZN6Player18St_YoshiPower_MainEv (0x020d7504, 0x9cc) | ruspecial (Claude-assisted) | 2026-07-05 | done - matched byte-identical, PR open |
| ov002/006/065/080 batch: func_ov006_020ef794, _ZN6Player19St_CrazedCrate_InitEv, func_ov080_02123fcc, func_ov006_020c87d0, func_ov002_020cec2c, func_ov065_0211a6d0 | ruspecial (Claude-assisted) | 2026-07-06 | done - 6 verified byte-identical |
| ov063 func_ov063_0211640c (0x0211640c, 0x2a0) | ruspecial (Claude-assisted) | 2026-07-06 | later MATCH by lunavyqo PR #493 (was div=1 near-miss) |
| ov058 func_ov058_0211123c (0x0211123c, size 0x3e0) | lunavyqo | 2026-07-09 | done - verified byte-identical |
| 12-func tiered LLM batch (spread ~0x64–0x138): func_ov002_020aeee4, func_ov003_020ae1a4, func_ov004_020b77b4, func_ov005_020c21ec, func_ov006_020e0884, func_ov007_020b9fc0 | andrew (Claude-assisted) | 2026-07-09 | done - 6 byte-identical this PR (ov002 NONMATCHING→match). ov012 matched upstream concurrently; ov010/ov015/ov016 already upstream. ov009 (div2) + _ZN8CapEnemy11GetCapStateEv (div4) near-miss walls. API claims 401 (console auth), locked via row. |
| coddog fan-out batch 1 (30 funcs, size 0x100-0x280): scattered in ov006, ov002, arm9 (addrs 0x020xxxxx) | Tango (Claude-assisted) | 2026-07-11 | done - 21/30 landed byte-identical, merged (cdd62d5b). API claims 401, coordinated via row. |
| coddog fan-out batch 2 (30 funcs, size 0x100-0x280): more ov006/ov002/arm9 + Player state siblings (addrs 0x020xxxxx) | Tango (Claude-assisted) | 2026-07-11 | in progress - LLM drafting. API claims 401, coordinated via row. |
| coddog large-band batches (size >= 0x280): scattered ov006/ov002/arm9 + Player state siblings (addrs 0x020xxxxx) | Session2 (Claude-assisted) | 2026-07-11 | in progress - batch 1 (0x280-0x400, 30 funcs) in flight via sched_run.js/wl_s2.jsonl. Disjoint from session 1's [0x100,0x280). API claims 401, coordinated via row. |
| harvest_wl small-func band (size < 0x100): smallest unmatched funcs + refine of the <0x100 near-miss backlog, spread across modules (addrs 0x020xxxxx) | Session3 (Claude-assisted) | 2026-07-11 | in progress - fresh fan-out exhausted (band was really a near-miss backlog); pivoted to refine_wl --max-size 0x100. Removed 17 stale NONMATCHING shadows. Disjoint from S1 [0x100,0x280), S2 [0x280,0x800]. API claims 401, coordinated via row. |
| coddog/refine large-func batches (size >= 0x800): scattered ov006/ov002/arm9 + Player state siblings (addrs 0x020xxxxx) | Session4 (Claude-assisted) | 2026-07-11 | reserved - parallel worker on the >=0x800 large-func band via coddog + refine. Disjoint by construction from S1 [0x100,0x280), S2 [0x280,0x800], S3 <0x100. API claims 401, coordinated via row. |
| easy/broad LLM fan-out sess4 (72 cands, size ~0x118-0x17c, no-example tail): scattered ov006/ov002/arm9/ov007 + others | andrew (Claude-assisted) | 2026-07-11 | in progress - 6-agent disjoint fan-out, harder no-example pile. Read-only API check() (no overlap w/ 9 live locks); no key. Siblings PR #279 (36), #280 (14). |
| easy/broad LLM fan-out sess5 (35 funcs, size ~0x118-0x17c): scattered ov002/ov006/ov007/arm9 + others | andrew (Claude-assisted) | 2026-07-11 | in progress - 6-agent disjoint fan-out. HARD-LOCKED each span via MCP claims API (35/36 acquired; dropped _ZN11VirtualDoor13InitResourcesEv - live conflict w/ bmanus2-dotcom). Siblings PR #279/#280/#283. |
| easy/spread LLM fan-out (2 waves, size ~0x70-0x1fc): 36 funcs across arm9(6), ov002(7), ov006(17), ov007(1), ov062(1), ov080(1), ov098(3) | andrew (Claude-assisted) | 2026-07-11 | done - 36 byte-identical + linkcheck VERIFIED, PR open. Worklist --easy --spread top-of-pile picks; filtered against src/ before work. |
| easy/broad LLM fan-out sess3 (72 cands, size ~0x6c-0x1ec): 72 named funcs across arm9/ov006/ov007/ov002 + others (addrs 0x0200xxxx-0x0214xxxx) | andrew (Claude-assisted) | 2026-07-11 | in progress - 6-agent disjoint fan-out. Read-only API check() first (no overlap with the 9 live locks); no key so coordinating via this row. Sibling PR #279 (36 funcs) already open. |
| ov077 _ZN6Lakitu13InitResourcesEv (0x02124908, 0x19c) | lunavyqo (Grok) | 2026-07-13 | **done** — verified byte-identical |
| arm9 MulMat4x3Mat4x3 (0x02052914, 0x220) | tangosdev (Claude/Opus) | 2026-07-12 | near-miss, floor(register allocation) - structurally byte-exact (all 12 outputs correct); ROM uses 7 callee-saved regs, mwccarm gives clean C 5 and spills (+9 insns). Hand-tuning + 23min permuter (206->112 div) did not crack. Draft banked to nearmiss/db.jsonl for a stronger tier. RELEASED. |
| arm9 fan-out (0x1f0-0x27c, 14 funcs): scattered main-binary 0x02013f4c-0x02072fcc | tangosdev (Claude/Fable) | 2026-07-12 | DONE - 12/14 matched; **func_0204b244 later MATCH (PR #385)**; func_02072fcc still near-miss div 17 banked. RELEASED. |
| arm9 fan-out #2 (0x100-0x400, 16 funcs): func_02046bbc,0206610c,0200c394,ShowCrashScreen,020316d8,02040c34,0205bc88,02048234,0200bb28,020197b8,0200c66c,020662c0,02046e28,0205f300,0201b7cc,02031e00 | tangosdev (Claude/Opus) | 2026-07-12 | DONE - 6/16 matched + link-gated (020197b8,0200c66c,020662c0,0205f300,0201b7cc,02031e00); 10 near-miss (mostly register-coloring floors, banked to nearmiss/db.jsonl). Opus 6/16 @ 219K tok/landed vs Fable 12/14 @ 52K - FABLE is the better fan-out model, Opus for finishing near-misses. RELEASED. |
| ov018 func_ov018_02111fac (0x02111fac, size 0x230) | lunavyqo (Grok-assisted) | 2026-07-12 | done - verified byte-identical; API claim clm_5551ed44d304 kept active |
| ov072: func_ov072_02121368 (0x02121368, 0x174) + __sinit_ov072_02122414 (0x02122414, 0x2f4) | lunavyqo (Grok-assisted) | 2026-07-13 | done - both verified byte-identical |
| ov025 func_ov025_021113f0 (0x021113f0, 0x3ec) | lunavyqo (Grok) | 2026-07-13 | **done** — verified byte-identical |
| ov079 batch13: 13 Whomp/BulletBill funcs | lunavyqo (Grok) | 2026-07-13 | **done (12/13)** — 12 matched; 02124008 banked div=1 FLOOR(materialization) — notes/func_ov079_02124008-floor.md + nearmiss/db.jsonl |
| ov009: func_ov009_0211145c, func_ov009_021115d8, _ZN4Bird13InitResourcesEv | lunavyqo (Grok) | 2026-07-13 | **done** — 3 verified byte-identical |
| ov064 batch11: 8 funcs (15f98,16220,17220,Bully::Behavior,17d24,18760,LavaBubble::InitResources,18ee4) | lunavyqo (Grok) | 2026-07-13 | **done** — 8 verified byte-identical (RotatingFirebar::Behavior, 193b4, 1a4c4 dropped: hand-asm dcd dumps, not matches) |
| ov016 Unagi batch: func_ov016_021119ec, _ZN5Unagi8BehaviorEv, _ZN5Unagi13InitResourcesEv (0x021119ec-0x02112e1c) | lunavyqo (Grok) | 2026-07-13 | **done** — 3 matched byte-identical; near-misses func_ov016_02111c40 (div=54) + func_ov016_02112b50 (div=10) banked in nearmiss/db.jsonl |
| ov066: func_ov066_02118e04 (0x02118e04, 0x218) + _ZN6Eyerok8BehaviorEv (0x02119838, 0x4b0) | lunavyqo (Grok) | 2026-07-15 | **done** — both verified byte-identical |
| ov034 Wiggler batch: func_ov034_021113d4/115cc/11e68/120ac/12688/12874 (0x021113d4-0x0211323c) | lunavyqo (Grok) | 2026-07-15 | **done** — 6 matched byte-identical; _ZN7Wiggler8BehaviorEv near-miss (div=277, size 0x6e0) banked in nearmiss/db.jsonl |
| ov004 batch16: __sinit_020b9b24 + 020b53f0/410c/6ddc/556c (5 funcs) | lunavyqo (Grok) | 2026-07-15 | **done** — 5 matched byte-identical (mwccarm 1.2/sp2p3); near-misses func_ov004_020b484c/38ac/3b38/4360/45c0 banked in nearmiss/db.jsonl |
| ov006 batch 0x020c0f0c-0x020c7860 (14 funcs) | lunavyqo (Grok) | 2026-07-15 | **done** — 14 matched byte-identical; near-misses func_ov006_020c3990 (div=14) + func_ov006_020c4fa4 banked in nearmiss/db.jsonl |
| ov080 batch16: 15/16 MATCH (261f4 div=5 4a8 regperm wall, same as 25460) | lunavyqo (Grok) | 2026-07-15 | **active** — 15 matched PR draft; 261f4 near-miss banked |
| ov006 batch16: 0x020c7a30-0x020cc198 (16 claimed) — **7 MATCH** (9098,9c8c,caadc,8ddc,cb1a8,cb528,ca840); 8 banked nearmiss (8680,9d7c,ca8e0,cb72c,7a30,8c78,7c68,cbfd8); c94e0 open | lunavyqo (Grok) | 2026-07-16 | **done (7 matched + DB bank)** |
| easy/spread fan-out (36 funcs, size ~0x70-0x1fc): arm9(4), ov002(5), ov004(1), ov006(6), ov007(11), ov030(1), ov060(3), ov063(1), ov074(1), ov095(1), ov100(1), ov102(1) | andrew (Claude-assisted) | 2026-07-16 | in progress — worklist --easy --spread top-of-pile picks, filtered against rebuilt ledger + live belongto.us claims (no key, coordinating via this row) |
| mixed batch (arm9/ov002/ov006/ov007): 21 funcs incl. Player::CleanupResources, HUD::Behavior, Stage::VE_Update, Stage::RenderModelTransparent | ruspecial (Fable) | 2026-07-16 | **done** - 21 matched byte-identical; 15 near-misses banked in nearmiss/db.jsonl |
| batch8 arm9+ov005: 0204b244, 0204b81c, 020c1130 (+ near-miss bank for rest of 8-pack) | lunavyqo (Grok) | 2026-07-16 | **done (3 matched PR #385)** — b244@0x0204b244/0x21c, b81c@0x0204b81c/0x3bc, c1130@0x020c1130/0x370 byte-identical; near-misses banked in nearmiss/db.jsonl (not src/): af3c div≈25, 0b04 div≈25 (size 0x434), c0378 div≈37, a730 div≈281 (size 0x6fc), c16e4 div≈156 |
| arm9: func_02065c2c/65de4/6610c/6655c/676e0/67bfc/681b8/688fc (0x02065c2c–0x02068a28) | lunavyqo (Grok) | 2026-07-16 | **done (4 matched + DB bank)** — 65c2c/65de4/681b8/688fc byte-identical (PR #368); near-misses banked: 676e0 (div=4 case2 schedule wall), 67bfc (div=38), 6610c (div=50), 6655c (div=179) in nearmiss/db.jsonl |
| div1 refine: func_ov006_020e84b8 (0x020e84b8) + func_ov007_020c5854 (0x020c5854); near-misses banked func_ov090_021327e4 (div=1) + func_02071364 (div=1) | lunavyqo (Grok) | 2026-07-16 | **done** — 2 matched byte-identical (PR #362); near-misses later cracked 2026-07-18 (PRs #490/#491) |
| ov002: _ZN6Player8BehaviorEv (0x020e4d24, size 0x868) | lunavyqo (Grok) | 2026-07-16 | **done** — verified byte-identical (draft PR #389); walk-before-abs closed residual; claim clm_ce7b107538fd |
| ov085 Toad batch: 021290b4, 021291ac, Toad::Behavior, Toad::InitResources, 0212ac4c, 0212ae08, 0212b4b4, 0212b75c (0x021290b4–0x0212b86c) | lunavyqo (Grok) | 2026-07-17 | **done (partial)** — **7/8 MATCH** (draft PR): 290b4, 291ac, InitResources, 2ac4c, 2ae08, 2b4b4, 2b75c. Toad::Behavior banked near-miss div=6 (improved from 14) in nearmiss/db.jsonl. |
| div1 near-miss batch (2026-07-18): func_ov006_0211f554 (0x0211f554), func_02071364 (0x02071364), func_ov090_021327e4 (0x021327e4); floors func_02068dc8, func_ov002_020d5c6c, func_0205fb58, func_02068398, func_ov006_020fbcb8/021027e4, func_ov071_02120200, func_ov063_0211640c | lunavyqo (Grok) | 2026-07-18 | **done (3 matched)** — 0211f554 PR #489 merged; 02071364 PR #490 merged; 021327e4 PR #491 merged; 3 matched PRs #489–#491; ov063 later MATCH PR #493; remaining regperm/if-conv/pool/tile/RMW floors |
| ov006 func_ov006_0211f554 (0x0211f554, size 0x80) | lunavyqo (Grok) | 2026-07-18 | **done** — verified byte-identical, PR #489 merged |
| arm9 func_02071364 (0x02071364, size 0x1ac) | lunavyqo (Grok) | 2026-07-18 | **done** — verified byte-identical, PR #490 merged |
| ov090 func_ov090_021327e4 (0x021327e4, size 0x274) | lunavyqo (Grok) | 2026-07-18 | **done** — verified byte-identical, PR #491 merged |
| arm9 Message::DisplaySaving (0x0201eb94, size 0x3a4) | lunavyqo | 2026-07-18 | done - verified byte-identical, draft PR |
| div1 floors continue (2026-07-18): func_ov063_0211640c + func_02068dc8, func_ov002_020d5c6c, func_0205fb58, func_02068398, func_ov006_020fbcb8/021027e4, func_ov071_02120200 | lunavyqo (Grok) | 2026-07-18 | **active** — grinding ov063 div1 (mvn+smulbb); rest floors |
| ov063 func_ov063_0211640c (0x0211640c, size 0x2a0) | lunavyqo (Grok) | 2026-07-18 | **done** — verified byte-identical, PR #493 |
