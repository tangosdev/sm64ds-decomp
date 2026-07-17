# Match attempts (attempt tree)

Append-only history of **every matching try** for a function (including
`no_progress` / failed). Separate from:

| Store | Role |
|-------|------|
| `nearmiss/db.jsonl` | Best **tip C** only (sm64ds spine) |
| `config/match_attempts.jsonl` | **All tries** as tree metadata (no full C) |
| `config/match_provenance.jsonl` | Final **how** when stamped |

## Status (set from tools)

| status | When |
|--------|------|
| `matched` | Only after `match.py` reports MATCH |
| `near_miss` | Scored tip that improves best (set `divergences`; pass `--src` for tip C) |
| `no_progress` | Tried; no useful score **or** did not beat best tip |
| `compile_error` / `failed` / `skipped` | As named |

Do **not** log wall-clock times.  
`tools/bank.py` remains the **fan-out** banker. Final how-records use
`tools/stamp_provenance.py` (not a new try).

```bash
python tools/log_attempt.py --func NAME --module arm9 --addr 0x… \
  --status no_progress --kind ai \
  --model grok-4.5 --reasoning high --harness grok-build \
  --session-scope focused --batch-size 1
```

See electroplankton-decomp `notes/adopting-match-conventions.md` for the full stack.
