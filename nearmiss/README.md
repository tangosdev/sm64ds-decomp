# Near-miss database

`db.jsonl` is the persistent store of close-but-not-matched function attempts -- compiling
C from the fan-out (and hand-cracking) that did not byte-match, kept forever and ranked by
how few instructions diverge from the ROM. These are the most valuable byproduct of the
matching effort: logically correct, already compiling, often 1-4 instructions from done.

Why keep them: re-deriving a near-match costs LLM tokens; finishing one is cheap (a small
fix, the permuter, or a stronger future model). This pre-positions the hard residue at ~95%
so the final push is fast and cheap instead of starting from scratch.

One record per (module, addr), keeping the CLOSEST candidate:
  {module, addr, name, size, target_hex, lang, divergences, c_source, source}
plus `cand_size` (the candidate's assembled byte size; `size` is the TARGET's) and
`evaluator` (the fingerprint of the compiler+metric that produced `divergences`, e.g.
`2004/b56|m1`) on every row the evaluator has touched. "Closest" is the combined key
(divergences, |cand_size - size|): edit distance leads and an upsert never regresses
it, but of two equally-close drafts the one assembling nearer the target size wins --
edit distance alone is non-monotonic in candidate size (a ten-instruction-short draft
has scored 225 while the one-instruction-off draft of the same function scored 267),
and the permuter cannot add or remove instructions, so the size-closer draft is the
better fuel. Worklists (`export-close`, `crunch.py`, `refine_wl.py`) rank by the same
key.

Managed by `tools/nearmiss_db.py` (ingest / stats / list / export-close / bank-matches
/ reeval). Run `python tools/permuter/crunch.py` to grind the closest ones through the
permuter locally (free), banking any that reach a match.

## Score integrity: the eval pin

Stored divergences are only comparable while the evaluator that produced them still
exists. `include/` churn under the stored sources, a canonical-compiler bump, or a
metric change all re-score unchanged rows silently (2026-08-30 audit: 5 of 8 sampled
rows re-scored differently -- one recorded at 230 re-evaluated to 354 -- and two rows
recorded at divergence 13 no longer compiled at all, poisoned bait atop any
closest-first worklist).

- `eval_pin.json` (committed, next to the DB) records the evaluator of the last full
  re-evaluation pass. Every ranking consumer warns on stderr when the live evaluator
  no longer matches it, and `tools/test_nearmiss_db.py` fails CI when
  `match.CANONICAL`, `match.DEFAULT_FLAGS` or `nearmiss_db.METRIC_REV` moves without a
  fresh pass. Flags are gated as well as the compiler and the metric: `-O4,p` -> `-O2`
  re-scores every stored source far harder than a compiler bump does.
- `python tools/nearmiss_db.py reeval` is that pass: it re-scores every row, stamps
  each with the evaluator fingerprint, and rewrites the pin. Run it on a MAIN-TIP
  checkout only (stale-lane rule) and commit `db.jsonl` + `eval_pin.json` together.
- A row whose stored source no longer scores is KEPT but marked -- `status`
  ("noncompile" / "func-absent"), `error`, `divergences: null`, the last good score in
  `stale_divergences` -- so it ranks last everywhere instead of topping the queue. A
  later strictly-improving ingest replaces it and clears the mark.
- Union merges can resurrect a pre-reeval copy of a corrected row (this file is
  `merge=union`); the dedupe collapse prefers a row stamped by the current pin over
  any unstamped or stale-stamped copy, so corrections survive the next refresh.
  Scope, precisely: `merge=union` is a **local** merge driver. GitHub does not honour
  it (`.gitattributes` says so), so on the server two PRs touching this file conflict
  visibly. The stamp-first collapse therefore protects the *local resolve-and-push*
  path -- someone running `git merge origin/main` here, or the update-chaos-data
  workflow rewriting `db.jsonl` on main -- not the PR merge itself.
- One consumer is outside that net: `tools/refine_wl.py` reads `db.jsonl` with a raw
  `json.loads` per line instead of `load_db`, so after a local union merge it sees
  BOTH copies and orders them with `seed_rank`, which has no stamp term. Run
  `python tools/nearmiss_db.py dedupe` after any union merge before trusting its pool.

## Standing rule: every batch feeds this DB

A harvest/fan-out batch must save its misses, not just its matches. Do not let this starve.

1. The fan-out agent prompt MUST ask each agent to emit a SECOND file of near-misses next to
   its matches: compiling, structurally-correct C that did not byte-match, as
   `{name, module, addr, c_source}`. The permuter only fixes register allocation and
   instruction ordering, so the seed must have the right instructions in the right shape -- a
   draft missing instructions (e.g. base-materialization floor) is not useful fuel.
2. After banking the matches, ingest the misses:
   `python tools/nearmiss_db.py ingest --seeds <file> --label <batch>`.
3. Work the backlog: `list --max-div 4` is the closest-first by-hand queue; `export-close` +
   `tools/permuter/batch.py --seeds` (or `crunch.py`) grinds them; `bank-matches` re-checks all
   entries and banks any that now reach a byte match.

Why: re-deriving a 95%-done attempt costs tokens; finishing one (a small fix, the permuter, or a
stronger model later) is cheap. The misses are the most valuable byproduct of every batch.
