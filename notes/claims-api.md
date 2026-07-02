# Claims API

Use the live claims API to coordinate active decompilation work in real time. It
answers one question: who is actively working on this module/address range right
now?

Base URL: `https://belongto.us`

**Fetch the live spec first:** `GET /api/claims/instructions` returns the current
markdown contract. Do this at startup instead of hardcoding paths — both the create
endpoint AND the auth model have changed since (see history note below).

**Writes require a key (verified 2026-07-01):** `try-lock`/`renew`/`release` all need
an `X-Api-Key` header — a personal key tied to a Discord account (get one from the
project's Discord bot: `/apikey get` or DM `key`). For tooling, set
`CLAIMS_API_KEY` or put the key in gitignored `tools/claims_key.txt`; never commit it.
Reads need no key.

No key is required for first-run setup or local trial batches. Without a key,
`tools/crackloop.py prep` skips live locking, leaves the generated worklist intact,
and tells you to coordinate manually through `CLAIMS.md`. Use the live API once you
are running real concurrent batches or need exact per-function conflict handling.

## Flow

```sh
# 1. (optional) read-only preview — NOT an acquire (no key needed)
curl "https://belongto.us/api/claims/check?module=ov006&start=0x0212318c&end=0x021231ac"

# 2. acquire — this is the single atomic acquire op (NOT /lock, NOT bare POST /api/claims)
curl -sS -X POST "https://belongto.us/api/claims/try-lock" \
  -H "Content-Type: application/json" -H "X-Api-Key: $CLAIMS_KEY" \
  -d '{"module":"ov006","start":"0x0212318c","end":"0x021231ac","handle":"andrewboudreau","note":"matching batch"}'
# -> 201 created | 200 idempotent repeat | 401 missing/invalid key | 409 conflict | 429 rate-limited
# save the returned claim.id

# 3. renew while working (before the 24h TTL elapses)
curl -sS -X POST "https://belongto.us/api/claims/clm_example/renew" \
  -H "Content-Type: application/json" -H "X-Api-Key: $CLAIMS_KEY" -d '{"handle":"andrewboudreau"}'

# 4. release when done/stopping
curl -sS -X POST "https://belongto.us/api/claims/clm_example/release" \
  -H "Content-Type: application/json" -H "X-Api-Key: $CLAIMS_KEY" -d '{"handle":"andrewboudreau"}'
```

## Endpoints (current contract)

- `POST /api/claims/try-lock` — acquire (needs `X-Api-Key`). Body `{module, start, end, handle, note?}`.
  201 created | 200 idempotent repeat | 401 missing/invalid key | 409 conflict (see `conflicts`) | 429 rate-limited.
- `POST /api/claims/{id}/renew` — needs `X-Api-Key`; body `{handle}`. 200 | 401 no/bad key | 403 wrong owner | 404 not found.
- `POST /api/claims/{id}/release` — needs `X-Api-Key`; body `{handle}`. 200 | 401 no/bad key | 403 wrong owner | 404 not found.
- `GET /api/claims/check?module=&start=&end=` — read-only preview, NOT an acquire (no key).
- `GET /api/claims?module=` — list active claims (omit `module` for all; no key).
- `GET /api/claims/{id}` — single claim detail (no key).
- `GET /api/claims/instructions` — live markdown spec; fetch this first (no key).
- `/api/claims/admin/*` — admin-tier routes; need an admin key or `CLAIMS_ADMIN_TOKEN`.

## Rules

- **Writes (try-lock/renew/release) require `X-Api-Key`** — a personal Discord-issued
  key (bot `/apikey get` or DM `key`); missing → `401 "missing X-Api-Key header"`, bad →
  `401 "invalid or revoked API key"`. Reads need no key. The handle is now only a display
  label; the key proves identity, and renew/release require the SAME key that created the lock.
- Without a key, use `CLAIMS.md`. `tools/claims.py lock-worklist` and
  `tools/crackloop.py prep` leave worklists unchanged instead of treating auth failure
  as function conflicts.
- `module`/`start`/`end`/`handle` required; `note` optional. Addresses are hex strings.
- Ranges are half-open `[start, end)`. Touching ranges do not conflict.
- Conflicts are per-module overlaps only; different modules never conflict.
- **A handle may hold MANY concurrent locks.** Lock every module/range in your batch
  under the SAME handle (e.g. `andrewboudreau`) — do NOT suffix the handle per range;
  put human context in `note`. Renew while working, release when done.
- Module names: `arm9` is the main binary (`main` is accepted and normalized to `arm9`);
  overlays are zero-padded `ov000`..`ov199`.
- Retrying the exact same try-lock (same module/start/end/handle) returns the existing lock.
- TTL is 24h; renew before it elapses. Locks expire automatically if not renewed, but
  release explicitly when stopping.

## History / gotchas (why this file was wrong before)

- The create endpoint is **`POST /api/claims/try-lock`**. `POST /api/claims/lock` and
  bare `POST /api/claims` 404/405 and always have — `/lock` is swallowed by the
  `GET /api/claims/{id}` route (id="lock") and returns `405 Allow: GET`. (Bare
  `POST /api/claims` was the create endpoint only for a short window before 2026-06-29.)
- Earlier local notes claimed "one handle = one active lock" — WRONG; a handle holds
  many concurrent locks.
- Auth changed after 2026-06-30: writes used to be open (no token); they now require a
  personal `X-Api-Key`. Any note saying "no auth for try-lock/renew/release" is stale.
