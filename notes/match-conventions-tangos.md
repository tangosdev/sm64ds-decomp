# Match conventions (tangOS / Chaos Viewer)

SM64DS keeps the **matching spine**: `src/` = matches only, near-miss tips in
`nearmiss/db.jsonl`.

`tangos.json` declares `project.matchConventions` so **tangOS Console**
(builds with match-convention support, e.g. lunavyqo/tangOS until upstream) can:

- Treat near-miss tips as tool-based (not paste) via `nearMissDb`
- Prefill default AI provenance slugs
- Keep **`attemptTree: false`** — full try-diary logging is optional and not
  required for SM64DS

To adopt EP-style attempt trees later (every try in `config/match_attempts.jsonl`),
see:

https://github.com/lunavyqo/electroplankton-decomp/blob/main/notes/adopting-match-conventions.md

Do **not** put non-matching C in `src/`.
