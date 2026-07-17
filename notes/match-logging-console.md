# Match logging for tangOS Console (required tools)

This tree includes the EP-compatible logging stack. **Console only works if these tools exist** — `matchConventions` is not enough.

| Tool | Script | When |
|------|--------|------|
| **log_attempt** | `tools/log_attempt.py` | After **every** try |
| **stamp_provenance** | `tools/stamp_provenance.py` | After MATCH for final how (AI fields required) |
| **bank** | `tools/bank.py` | Fan-out batch verify only — **not** provenance |

Stores: `config/match_attempts.jsonl`, `config/match_provenance.jsonl`, tip C in `nearmiss/db.jsonl`.

Open this branch (or a main that contains these files) in **lunavyqo/tangOS**.
