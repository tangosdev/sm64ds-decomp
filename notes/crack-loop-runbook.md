# Crack-loop runbook (coddog fan-out)

The whole matching loop in commands. State as of this writing: **66.7% matched
(7,593 / 11,390)**, goal 70% (7,973). All tooling below is committed, so any fresh
session or the coworker can run it straight from a clone.

## One batch, start to finish

```sh
# 1. Build a coddog-scheduled worklist (fuzzy opcode similarity; ~30s, no LLM cost).
#    Pick a size band; --spread round-robins modules. Rebuild EVERY batch (the matched
#    corpus changes as functions land, which changes the best siblings).
python tools/coddog.py --min 0x100 --max 0x280 --limit 30 --spread --out progress/wl_ab.jsonl

# 2. Claim the module spans (coordination; skips anything the coworker holds).
python tools/claims.py lock-worklist progress/wl_ab.jsonl        # writes progress/claims_active.json
#    Trim the worklist to only the modules we actually locked (drop coworker-held ones):
python - <<'PY'
import json
locked=set(json.load(open("progress/claims_active.json")))
rows=[json.loads(l) for l in open("progress/wl_ab.jsonl")]
keep=[r for r in rows if r['module'] in locked]
open("progress/wl_ab.jsonl","w").write("".join(json.dumps(r)+"\n" for r in keep))
print("names:", json.dumps([r['name'] for r in keep]))
PY

# 3. Fan out (self-verifying). Default Sonnet 5; pass model:"fable" for a capability push.
#    In Claude Code: Workflow({ scriptPath: "tools/sched_run.js", args: <names[]> })
#                    Workflow({ scriptPath: "tools/sched_run.js", args: {names:[...], model:"fable", effort:"high"} })

# 4. Bank the result (independent re-verify + park misses), then free post-pass.
python tools/bank_run.py --output <the Workflow task .output file>
python tools/clone.py && python tools/paramclone.py

# 5. Release the claims. ALWAYS do this, even on a stopped/failed batch.
python tools/claims.py release-active

# 6. Refresh + commit (src is committed; matched.jsonl/nonmatching.jsonl are gitignored).
python tools/progress.py
git add src/ README.md && git commit -m "Match N functions via coddog fan-out (XX.X%)"
```

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
observed this session 71% -> 88% (__sinit vein) -> 60% -> 57% -> 43%, tokens/landed
25K -> 176K. When a band floors (~40% and dropping), do NOT re-grind it -- **pivot to a
fresh size band** coddog has not touched (e.g. 0x200-0x400), which resurfaces high-similarity
clusters (the larger __sinit initializers were still a rich vein at 0x200-0x400). When even
fresh bands come in low, the accessible vein is worked: switch to the free tiers
(clone/paramclone, recurring.py -> new templates) and hand-fix, and stop the paid fan-out.

## The floor (do not grind)

Misses parked in nonmatching.jsonl are the codegen floor: base-materialization RMW,
instruction-ordering / scheduling, and value-numbering CSE residues. Documented unreachable
from C across compiler versions and the permuter. See [[sm64ds-materialization]],
[[sm64ds-ordering-floor]]. Route these to hand-fix, never the permuter.
