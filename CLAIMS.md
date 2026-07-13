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
| ov005: 020c0378 (0x020c0378), 020c1130 (0x020c1130), 020c16e4 (0x020c16e4) | lunavyqo | 2026-07-10 | near-miss (PR #239 open) |
| ov020: 02111340 (0x02111340), 021115ac (0x021115ac), 02113148 (0x02113148) | lunavyqo | 2026-07-10 | near-miss (PR #244 open) |
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
| _ZN5Stage9PS_UpdateEv (0x0202635c, 0x30ac) + func_ov006_02115b0c (0x02115b0c, 0x18bc) + func_0206a928 (0x0206a928, 0x1360) + func_ov006_02112ad8 (0x02112ad8, 0x113c) | Cursor/Grok | 2026-07-02 | paused — large funcs; scratch/HANDOFF_batch_4funcs.md |
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
| ov063 func_ov063_0211640c (0x0211640c, 0x2a0) | ruspecial (Claude-assisted) | 2026-07-06 | NONMATCHING div=1 (ang*-1 rsb vs ROM smulbb); near-miss draft submitted |
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
| ov077 _ZN6Lakitu13InitResourcesEv (0x02124908, 0x19c) | lunavyqo (Grok-assisted) | 2026-07-11 | near-miss div=4 (regperm scale 0x1000 r2 vs r0); improved from div=10 |
| arm9 MulMat4x3Mat4x3 (0x02052914, 0x220) | tangosdev (Claude/Opus) | 2026-07-12 | near-miss, floor(register allocation) - structurally byte-exact (all 12 outputs correct); ROM uses 7 callee-saved regs, mwccarm gives clean C 5 and spills (+9 insns). Hand-tuning + 23min permuter (206->112 div) did not crack. Draft banked to nearmiss/db.jsonl for a stronger tier. RELEASED. |
| arm9 fan-out (0x1f0-0x27c, 14 funcs): scattered main-binary 0x02013f4c-0x02072fcc | tangosdev (Claude/Fable) | 2026-07-12 | DONE - 12/14 matched byte-identical + link-gated (52K tok/landed); func_0204b244 near-miss div 6 (POLYGON_ATTR r0/r2 coloring floor), func_02072fcc near-miss div 17 (1-insn b+1 hoist, Fable hit quota). Both banked to nearmiss/db.jsonl. RELEASED. |
| arm9 fan-out #2 (0x100-0x400, 16 funcs): func_02046bbc,0206610c,0200c394,ShowCrashScreen,020316d8,02040c34,0205bc88,02048234,0200bb28,020197b8,0200c66c,020662c0,02046e28,0205f300,0201b7cc,02031e00 | tangosdev (Claude/Opus) | 2026-07-12 | DONE - 6/16 matched + link-gated (020197b8,0200c66c,020662c0,0205f300,0201b7cc,02031e00); 10 near-miss (mostly register-coloring floors, banked to nearmiss/db.jsonl). Opus 6/16 @ 219K tok/landed vs Fable 12/14 @ 52K - FABLE is the better fan-out model, Opus for finishing near-misses. RELEASED. |
| ov018 func_ov018_02111fac (0x02111fac, size 0x230) | lunavyqo (Grok-assisted) | 2026-07-12 | done - verified byte-identical; API claim clm_5551ed44d304 kept active |
| ov072: func_ov072_02121368 (0x02121368, 0x174) + __sinit_ov072_02122414 (0x02122414, 0x2f4) | lunavyqo (Grok-assisted) | 2026-07-13 | done - both verified byte-identical |
| ov025 func_ov025_021113f0 (0x021113f0, 0x3ec) | lunavyqo (Grok) | 2026-07-13 | **done** — verified byte-identical |
