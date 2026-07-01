# Crack-loop runbook (coddog fan-out)

The whole matching loop in commands. State as of this writing: **66.7% matched
(7,593 / 11,390)**, goal 70% (7,973). All tooling below is committed, so any fresh
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
bank_run.py, clone.py, paramclone.py, progress.py) if a batch needs surgery. On a public
clone without tools/claims.py (it is gitignored, holds the claims API key), prep skips
the lock step and says so - coordinate via CLAIMS.md instead.
ALWAYS run `land` (or at minimum `python tools/claims.py release-active`) even on a
stopped or failed batch, so claims do not go stale.

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

## The refine tier (near-miss backlog -> matches)

When fresh bands run dry, the committed near-miss DB is the other paid vein: stored
compiling drafts a few instructions off. The refine specialist fixes STRUCTURAL residuals
the permuter cannot (branch/store order, loop form, && chains, arithmetic idiom, load
width, push-set). Validated: 49 recovered at ~100K tok/landed on the backlog.

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

## Pragmas tested, dead end (2026-07-01)

mwccarm 1.2 accepts `#pragma scheduling off` / `#pragma peephole off` silently and they
change NOTHING (0 divergence changes across 40 closest near-misses x 3 variants). The
SFA-decomp pragma technique does not transfer; the ordering floor stays hand-fix only.

## Model choice

- **Sonnet 5** is the validated default: parity with Opus 4.8 on matching at ~half the
  cost (see [[sm64ds-sonnet5-ab]]). `grind_chunked.js` and `sched_run.js` default to it.
- **Fable 5** (`model:"fable"`, effort `high`): most capable model. Worth it on the HARD
  residue, where borderline register-coloring near-misses may be reachable with more
  reasoning. It will NOT move the true codegen floor (materialization / ordering / CSE) --
  those are model-independent. On the first Fable batch, run a **Fable-vs-Sonnet A/B** on the
  same hard-residue names to measure whether the extra capability (and 3.3x cost) converts
  enough extra matches to justify it. If the lift is small, keep Sonnet 5.

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
