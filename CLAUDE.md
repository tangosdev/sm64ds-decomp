# Claude project instructions

This is a matching decompilation of Super Mario 64 DS. Do not touch generated
outputs, source files, symbol files, config, or progress files unless the user
explicitly asks for matching/decompilation work.

## Coordination

Before starting any matching batch or hand-matching range, coordinate the range. Use
the live claims API when you have a key; for first-run setup or a local trial batch
without a key, add a manual row in `CLAIMS.md` and stay inside that range. Fetch
`GET https://belongto.us/api/claims/instructions` first for the authoritative live
contract, then:

1. (optional) Preview the range — read-only, NOT an acquire:
   `GET https://belongto.us/api/claims/check?module=<module>&start=<hex>&end=<hex>`
2. Acquire — the single atomic acquire op (NOT `/lock`, NOT bare `POST /api/claims`):
   `POST https://belongto.us/api/claims/try-lock` with header `X-Api-Key: <key>`
   body `{module, start, end, handle, note?}` → 201 created | 200 idempotent repeat |
   401 missing/invalid key | 409 conflict | 429 rate-limited. Save the returned `claim.id`.
3. Renew while working (24h TTL): `POST .../api/claims/<id>/renew` (same `X-Api-Key`) body `{handle}`
4. Release when done or stopping: `POST .../api/claims/<id>/release` (same `X-Api-Key`) body `{handle}`

**Auth (verified 2026-07-01):** `try-lock`/`renew`/`release` ALL require the `X-Api-Key`
header — a write without it returns `401`. The key is a personal key tied to a Discord
account (get one via the project's Discord bot: `/apikey get` or DM `key`). Reads
(`/check`, `/instructions`, `GET /api/claims`) need no key. For tooling, set
`CLAIMS_API_KEY` or put the key in gitignored `tools/claims_key.txt`; never commit it.
Renew/release must use the same key that created the lock.

Use a stable handle such as `andrewboudreau/claude-sm64ds` (display label only; the key
proves identity). One handle may hold MANY concurrent locks — lock every range in a batch
under the same handle; no per-range suffix needed. Modules: `arm9` (main binary; `main` is
normalized to `arm9`), overlays `ov000`..`ov199`. See `notes/claims-api.md` for the full
contract and curl examples.

`CLAIMS.md` remains the human-readable fallback and historical convention. The
live API is preferred for real-time multi-agent work because it avoids git races.
`tools/crackloop.py prep` leaves its generated worklist intact when no key is
configured, then prints the Workflow launch line for the local batch.

## Matching Safety

- Work only inside the locked module/range.
- Do not modify another agent's active batch files.
- Keep unrelated dirty worktree changes intact.
- Verify matching changes with the repo tools before reporting success.
- For naming-only research, write notes under `notes/` instead of changing
  `config/**/symbols.txt` or `symbols/verified.tsv`.

## Agent Knowledge Base

Before starting agent-assisted matching batches, cleanup passes, or subsystem
documentation, read `notes/agent-decomp-knowledge-base.md`. It records the current
workflow lessons for scheduling, context selection, attempt logging, runtime
validation, and long-tail cleanup.

## Naming Notes

Prefer evidence in this order:

1. Mangled C++ names already present in symbols or binaries.
2. Nearby call graph, vtables, constructors, destructors, and class layout.
3. Strings, file IDs, level/object tables, actor IDs, and save flags.
4. Known NitroSDK/GX/FS/sound APIs.
5. N64 SM64 comparison only when behavior is clearly analogous.

Track speculative names with confidence and evidence before applying them.
