# Crack-loop runbook (coddog fan-out)

The whole matching loop in commands. State as of this writing: **66.9% matched
(7,624 / 11,390)**, goal 70% (7,973). All tooling below is committed, so any fresh
session or the coworker can run it straight from a clone.

## One batch = three commands

```sh
# 1. Prep: coddog worklist (fuzzy opcode similarity, ~30s, no LLM cost) + claim the
#    module spans + trim to what we locked. Rebuild EVERY batch (the matched corpus
#    changes as functions land, which changes the best siblings). Prints the launch line.
python tools/crackloop.py prep --min 0x100 --max 0x280 --limit 30

# 2. Fan out (self-verifying). Default Sonnet 5; model:"fable" for a capability push.
#    In Claude Code: Workflow({ scriptPath: "tools/sched_run.js", args: <names[]> })
#                    Workflow({ scriptPath: "tools/sched_run.js", args: {names:[...], model:"fable", effort:"high"} })

# 3. Land: bank (independent re-verify + park misses + auto-ingest near-misses into
#    nearmiss/db.jsonl) + free post-pass (clone/paramclone) + release claims + progress.
python tools/crackloop.py land --output <the Workflow task .output file>

# Commit (src + nearmiss DB are committed; matched.jsonl/nonmatching.jsonl are gitignored).
git add src/ nearmiss/ README.md && git commit -m "Match N functions via coddog fan-out (XX.X%)"
```

Every step still exists standalone (coddog.py, claims.py lock-worklist/release-active,
bank_run.py, clone.py, paramclone.py, progress.py) if a batch needs surgery.
All banking tools are DRY-RUN BY DEFAULT and take `--apply` to actually bank
(bank.py, bank_harvest.py, clone.py, paramclone.py, swarm.py, sweep.py, cascade.py).
crackloop.py land passes `--apply` where needed, so the one-command flow is unchanged.
Ledger reads/writes go through tools/ledger.py: one canonical (module, addr) key,
locked atomic appends with an under-lock duplicate check, and bank refusals when a
src/<name>.* file belongs to a different function.
tools/claims.py is committed; its API key is NOT (CLAIMS_API_KEY env var or the
gitignored tools/claims_key.txt). Without a key, claim checks still work read-only -
coordinate via CLAIMS.md.
ALWAYS run `land` (or at minimum `python tools/claims.py release-active`) even on a
stopped or failed batch, so claims do not go stale.

Safety gates in `land`: every banked function is independently oracle re-verified
(bank_harvest), then LINK-verified (tools/linkcheck.py reconstructs the linked bytes -
catches a wrong same-shaped callee the wildcarded oracle cannot see). Only close,
compiling misses (divergences <= 12) are parked into nonmatching.jsonl; a function that
merely failed the run falls back into the scheduling pool. `prep` refuses to rebuild the
worklist while a batch is in flight (progress/claims_active.json present).
If your local nonmatching.jsonl predates 2026-07-01, prune the poisoned entries once:
drop rows whose reason starts with "fan-out miss" AND divergences == 2 exactly (that
value was fabricated by the old bank_run; ~33 retryable functions were parked that way).

## After every batch: mine it, then optimize (standing rule)

The levers that broke the walls came out of batch RESULT NOTES, not the batches
(u64-mask laundering, PMF int-param, two-stage retry). After every land, before or
with the next launch:

1. Read every agent note, matches AND misses. A lever that worked goes into
   notes/mwccarm-codegen.md AND the prompt lever lists (refine_run.js / sched_run.js),
   credited. A repeated new floor shape gets documented so agents stop early on it.
2. Compare hit rate and tokens/landed against this runbook's numbers for the
   band/model; on decay, pivot the next prep and update the table here.
3. If a note reveals anything MECHANICAL (a transform, a misdiagnosis), sweep it
   across the near-miss DB locally before spending more LLM tokens - the pragma
   sweep and the u64 sweep each banked matches for zero tokens this way.

## Loop economics (upgraded 2026-07-01)

- **abverify.py now prints the per-instruction diff on NOMATCH** (mismatching words
  only, capped at 32 lines, reloc slots wildcarded) plus a final
  `NOMATCH divergences=N/words` line. Agents converge on the exact wall instead of
  guessing blind; before this they only got a byte count.
- **The fan-out prompt enforces early stop**: if divergences do not improve for 2
  consecutive attempts, the agent reports its best attempt and quits. Floor residuals
  (pure ordering, base-materialization) are named in the prompt so agents do not grind
  them. This is what keeps tokens/landed down as a band drains.
- **Near-misses are captured end to end**: the schema requires the lowest-divergence
  source + its divergence count from every agent, sched_run returns them as
  `nearMisses`, and bank_run ingests them into nearmiss/db.jsonl and writes real
  divergence counts into nonmatching.jsonl. Nothing close is ever discarded.
- Agents within ~6 divergences are told to read notes/pret-idioms.md and
  notes/mwccarm-codegen.md (the full idiom catalogue) before giving up - on-demand,
  so easy functions never pay that context cost.

## Two-stage retry (bank first, then retry the misses)

Measured by the coworker on PR #66's PMF family: Sonnet first pass 6/15, then bank
those 6, THEN retry the 9 misses - the retry agents find the freshly-banked twins in
src/ and copy the working idiom (4/9 recovered at the same cost/landed). Retrying
BEFORE banking would have re-proven "walls". When a batch has same-family misses,
land it, then immediately re-prep the misses as their own batch.

## The refine tier (near-miss backlog -> matches)

When fresh bands run dry, the committed near-miss DB is the other paid vein: stored
compiling drafts a few instructions off. The refine specialist fixes STRUCTURAL residuals
the permuter cannot (branch/store order, loop form, && chains, arithmetic idiom, load
width, push-set). Validated: 49 recovered at ~100K tok/landed on the backlog.

The near-miss DB is SHARED (committed), so refine batches are claims-coordinated too:
`crackloop.py refine` locks each chosen draft's address range per-function and drops
conflicted drafts (the other agent is already refining them); `land --refine` releases.
Use the post-lock launch line it prints, not refine_wl's pre-lock one.

```sh
python tools/crackloop.py refine --max-div 6 --limit 20   # category-routed export, prints launch line
# Workflow({ scriptPath: "tools/refine_run.js", args: <names[]> })
python tools/crackloop.py land --output <task.output> --refine
```

Routing (tools/refine_wl.py, cached in progress/nm_categories.json): structural
categories -> refine agents; "register allocation" / "instruction reorder" -> permuter
(tools/permuter); "base materialization / addressing" -> floor, skipped. One refine shot
per draft (progress/refine_attempted.txt); improved drafts flow back into the DB for the
permuter / hand-fix tiers. Do NOT run refine on a fresh fan-out's leftovers - measured
zero lift there (high-effort fan-out already captures what refine would).

**"base materialization / addressing" is RETIRED from refine** (2026-07-01 recheck:
0/16 with agents armed with the sec 6g triggers; the floor bound held case-by-case).
refine_wl skips the category by default - do not --only-category it again.

**Refine ONLY the head of the backlog.** Measured 2026-07-01, back-to-back same model:
batch 1 (the 16 closest, mostly div 1-4) = 5/16 (31%) at ~107K/landed; batch 2 (the next
20, deeper into div 4-6) = 1/20 (5%) at ~660K/landed - the heuristic categorizer mislabels
floor residuals as structural, and past the head the pool is floor-dominated (agents
confirmed: two-word batching, scheduler ordering, materialization, tail-merge epilogues).
So: run ONE refine batch on the closest drafts after fresh batches replenish the DB, then
stop - depth does not pay. The consolation prize of a deep batch is diagnosis: misses come
back floor-labeled and several drafts get improved in place.

## LARGE functions (0x400+): m2c semantic drafts (2026-07-01)

The 0x400+ tier (221 functions) rarely has a close matched sibling, so its rows arrive
scaffold-less (coddog_sim < 0.5). `--draft` on prep attaches a free m2c semantic C
draft to exactly those rows and abrow.py prints it to the agent ("m2c semantic
draft" block). The draft is gcc-flavored pseudo-C - control flow, resolved callees,
field offsets - a comprehension scaffold, NOT a matching candidate. Zero tokens,
~1s/function, validated 25/25 on an unmatched 0x400-0x113c sample.

```sh
python tools/crackloop.py prep --min 0x400 --max 0x800 --limit 24 --draft
# standalone: python tools/m2c_draft.py --name func_0206a928
```

Needs a local `git clone https://github.com/matt-kempster/m2c vendor/m2c` (gitignored).
Details, dialect notes, and caveats: notes/m2c-setup.md.

## Pragmas tested, dead end (2026-07-01)

mwccarm 1.2 accepts `#pragma scheduling off` / `#pragma peephole off` silently and they
change NOTHING (0 divergence changes across 40 closest near-misses x 3 variants). The
SFA-decomp pragma technique does not transfer; the ordering floor stays hand-fix only.

## Model choice

- **Sonnet 5** is the validated default: parity with Opus 4.8 on matching at ~half the
  cost (see [[sm64ds-sonnet5-ab]]). `sched_run.js` and `refine_run.js` default to it.
- **Fable 5** (`model:"fable"`, effort `high`): MEASURED 2026-07-01 on the exact 12
  lowest-divergence drafts Sonnet had exhausted (Sonnet: 0/12 in its own batch):
  **Fable 5/12 at ~108K tok/landed** - the same per-landed cost as Sonnet on the EASY
  head, converting functions Sonnet cannot. It cracked a documented permuter-proof
  two-word-batching residual and two prologue-order walls, and discovered new levers
  (now in notes/mwccarm-codegen.md sec 6e and the refine_run.js prompt). STANDING TIER:
  after a Sonnet refine batch drains the head, run a **Fable mop-up** on its misses
  (same wl_refine.jsonl, `{names:[...], model:"fable", effort:"high"}`). What Fable
  also could not move (7/12): first-access-fold materialization, pre-indexed writeback,
  pure register-coloring swaps, store-emission order - that is the REAL floor.
- **THE MID-BAND SHAPE WALL IS FABLE TERRITORY (measured 2026-07-01): 9/12 (75%) at
  ~45K tok/landed** in 0x80-0x140, the band every pre-Fable model floored at ~5% -
  including first-try matches at coddog sim 0.28 and 0.43 (whole-function structure
  reasoning, the documented "reverse-engineer the original author's C" wall). Cost per
  landed is at parity with Sonnet on easy fresh bands. The 3 misses were all retired
  floor classes. Default for mid-band batches is now `model:"fable"`; the remaining
  mid-band residue is open again. Round two held 12/16 (75%) at 49K.
- **0x140-0x280 is ALSO Fable territory (measured 2026-07-02): 10/12 (83%) at
  ~66K tok/landed** - that band was only ever drained for Sonnet, and it holds the
  single biggest unmatched byte pool (~427KB). The u64 laundering idiom carried five
  of the ten. Byte-cost ~146 tok/byte.
- **The LARGE band converts on SONNET with m2c drafts (measured 2026-07-02): 8/12
  (67%) at ~80K tok/landed in 0x400-0x800** including a scaffold-less logic function
  matched from its m2c draft. At ~1.3KB/function this is the best byte-per-token tier
  in the game (~64 tok/byte) - run it FIRST when pushing the code-bytes metric.
  Round 2 decayed to 7/16 (44%) at 150K: the big __sinit initializers inside it stay
  ~100% (5/5) but the big LOGIC functions are hard for Sonnet - consider routing the
  low-sim large logic functions to Fable once the sinit vein drains.
- **Fable 0x140-0x280 round 2: 16/16 (100%) at 64K/landed** - zero decay, the star
  lane. The u64 laundering idiom carried 6 of 16; one agent reached for asm{} on a
  pooled-offset RMW and pure C (laundering + encoding-forced split) reproduced it -
  no asm needed, repo stays asm-free for game logic.
- **Opus→Fable two-stage on 0x140-0x280 round 3 (2026-07-02, third pass, sims
  0.54-0.75): Opus 4/12 (33%) at 162K/landed, then Fable on the 5 non-floor misses
  = 5/5 (100%) at 30K/landed** - div 3/4/11/20/28 all cracked, including two new
  levers (laundering mask placement, escaped-array pre-indexed writeback - see
  mwccarm-codegen 6g 2026-07-02). Triage rule that produced the 5/5: promote
  materialization/source-shape residuals, leave coloring/ordering/tail-merge
  diagnoses in the DB. The Fable retry is now the cheapest paid tier measured;
  the expensive part is the Opus first pass on a thrice-drained band.
- **0x400-0x800 round 3 on Opus->Fable (2026-07-02, m2c drafts, sims 0.27-0.60):
  Opus 5/12 at 182K/landed but BIG targets (~6.5KB, ~140 tok/byte incl. the 39-entry
  PMF table func_02008550 and Player::St_MetalWaterGround_Main); then Fable retry
  4/4 at 52K/landed** - including TWO Opus-declared floors (store-emission order,
  decl-order-proof coloring swap; levers now in mwccarm-codegen 6g 2026-07-02) and
  the real-C++-vtable misshape (sec 5). TRIAGE UPDATE: on LARGE functions, promote
  floor-labeled misses at div<=8 too - they are 97-99% matched and Fable's 60+-attempt
  grind cracked 2/2 of them; park only deep cascades (div 27/32/91 all stayed parked).
  Fable promotion tier record so far: 9/9 across two batches.
- **WRONG-DEST salvage (2026-07-02, func_ov066_02119ce8):** when the land link-gate
  rejects an agent MATCH with `WRONG-DEST reloc` and the function stores/loads two
  same-shaped globals (e.g. two zero-stores), the agent likely swapped the symbols -
  swap them in the source, re-run abverify (still MATCH), then land via a synthesized
  single-function output file ({result:{landed,landedNames,results,sources}}). The
  byte oracle is blind to this class by design; the gate catches it.

## Reading the hit rate (why it decays, what to do)

coddog front-loads the best-scaffolded functions, so hit rate falls as a band drains:
observed 71% -> 88% (__sinit vein) -> 60% -> 57% -> 43%, tokens/landed 25K -> 176K.
**2026-07-01 continuation: the 0x100-0x280 band is DRAINED** - 4/24 (17%) at ~265K/landed,
median coddog sim of the batch only 0.53 (the good scaffolds are gone). When a band floors
(~40% and dropping), do NOT re-grind it -- **pivot to a fresh size band** coddog has not
touched (e.g. 0x280-0x400), which resurfaces high-similarity clusters (the larger __sinit
initializers were still a rich vein at 0x200-0x400). When even fresh bands come in low, the
accessible vein is worked: switch to the free tiers (clone/paramclone, recurring.py -> new
templates), the REFINE tier below, and hand-fix, and stop the paid fresh fan-out.

**Two agents, one band (measured 2026-07-01):** both sides run the same deterministic
scheduler, so simultaneous preps pick the SAME functions - the coworker's first 0x280-0x400
prep conflicted 20/20 with our locks. Per-function claims locking absorbs this (conflicted
rows drop, the rest proceed), but the survivor picks are SECOND-TIER scaffolds: their
disjoint batch ran 1/20 (5%) at ~2.1M tokens/landed vs first-tier picks at ~71% earlier.
Lesson: fresh fan-out in a band someone else is actively working is a dead vein - the
second agent should run REFINE (claims-coordinated) or take a different band entirely.
In a contested band, request extra width (higher --limit) so the post-lock batch is still
worth launching.

Measured same day, same model (Sonnet 5): the refine tier converted 5/16 (31%) of the
STUCK near-miss backlog at ~107K tok/landed - 2.5x cheaper per landed function than fresh
fan-out on the drained band, and its misses come back with precise floor diagnoses that
feed the DB and the codegen notes. At this stage of the project run refine batches first,
and save fresh fan-out for genuinely fresh bands. The category-routed permuter crunch
(tools/permuter/crunch.py --category "register allocation,instruction reorder") added
1/19 for free on the same day - keep it running when the PC is idle.

## The floor (do not grind)

Misses parked in nonmatching.jsonl are the codegen floor: base-materialization RMW,
instruction-ordering / scheduling, and value-numbering CSE residues. Documented unreachable
from C across compiler versions and the permuter. See [[sm64ds-materialization]],
[[sm64ds-ordering-floor]]. Route these to hand-fix, never the permuter.
