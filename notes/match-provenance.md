# Match provenance (final how)

When a function is matched, stamp **how** it was matched:

```bash
python tools/stamp_provenance.py --src src/NAME.c --kind ai \
  --model grok-4.5 --reasoning high --harness grok-build \
  --session-scope focused --batch-size 1 --no-verify
```

- WHO credit stays git first-adder / author field — not inside matchProvenance.
- This is **not** `tools/bank.py` (that verifies fan-out JSON batches).
- Stamping is **not** a new attempt row unless no prior `matched` try exists.

Store: `config/match_provenance.jsonl`.
