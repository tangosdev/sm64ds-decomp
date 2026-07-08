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
| _example: ov004 0x020b0000-0x020b8000_ | _handle_ | _2026-06-17_ | _example_ |
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
| arm9 _ZN5Stage9PS_UpdateEv (0x0202635c, 0x30ac) | ruspecial (Claude-assisted) | 2026-07-06 | NONMATCHING div=1170 from-scratch decomp (prior stub div=3032); 16/20 cases byte-identical; claims API was 401 so locked via this row only — taking over the paused Cursor/Grok row above |
| batch2 NONMATCHING near-misses: func_ov002_020bf224, func_ov007_020bc3dc, func_0205d304, _ZN16MeshColliderBase6EnableEP5Actor, func_0206e3dc | ruspecial (Claude-assisted) | 2026-07-06 | near-miss div=2-3 (regperm/predicated-order/schedule walls) |
| batch2 matched: func_ov062_021161a8, func_ov062_02119628, func_ov020_021112b0, func_ov006_0210d6b8 | ruspecial (Claude-assisted) | 2026-07-06 | done - 4 verified byte-identical |
| batch3 NONMATCHING near-misses: func_ov079_02126a84 (div2), func_ov095_021365d8 (div3) | ruspecial (Claude-assisted) | 2026-07-06 | scheduling residuals (arg-setup / store-hoist emission order) |
| batch3 matched: func_ov078_02125c98, func_ov060_021146d0, func_ov004_020b7cd0, _ZN3HUD15RenderCoinCountEv | ruspecial (Claude-assisted) | 2026-07-06 | done - 4 verified byte-identical |
| ov001 func_ov001_020aa420 (0x020aa420, size 0x290) | lunavyqo (AI-assisted) | 2026-07-06 | done - verified byte-identical |
| ov001 func_ov001_020aadac (0x020aadac-0x020aaf40) | lunavyqo | 2026-07-06 | done - verified byte-identical |
| ov001 func_ov001_020ab110 (0x020ab110, size 0x118) | lunavyqo | 2026-07-06 | done - verified byte-identical |
| ov017 func_ov017_021112c4 (0x021112c4-0x021113c0) | lunavyqo | 2026-07-07 | done - verified byte-identical |
| ov014 func_ov014_02111a6c (0x02111a6c, size 0x84) | lunavyqo | 2026-07-08 | done - verified byte-identical |
| ov043 small wrappers: func_ov043_021114b0, 021115cc, 021115e0, 02111744, 02111758 | lunavyqo | 2026-07-08 | done - verified byte-identical, PR open |
| ov035 func_ov035_0211195c (0x0211195c-0x02111a98) | lunavyqo | 2026-07-08 | done - verified byte-identical |
| ov002 _ZN6Player18St_YoshiPower_MainEv (0x020d7504, 0x9cc) | ruspecial (Claude-assisted) | 2026-07-05 | done - matched byte-identical, PR open |
| ov002/006/065/080 batch: func_ov006_020ef794, _ZN6Player19St_CrazedCrate_InitEv, func_ov080_02123fcc, func_ov006_020c87d0, func_ov002_020cec2c, func_ov065_0211a6d0 | ruspecial (Claude-assisted) | 2026-07-06 | done - 6 verified byte-identical |
| ov063 func_ov063_0211640c (0x0211640c, 0x2a0) | ruspecial (Claude-assisted) | 2026-07-06 | NONMATCHING div=1 (ang*-1 rsb vs ROM smulbb); near-miss draft submitted |
