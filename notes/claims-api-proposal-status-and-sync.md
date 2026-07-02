# Proposal: claim status + GitHub-derived completion sync

Proposed additions to the live claims API (`belongto.us`, contract in
`notes/claims-api.md`). This is a design proposal, not the current contract —
verify against `GET /api/claims/instructions` before relying on any of it.

## Problem

Two gaps we've hit in practice while coordinating with Tango (bmanus2) across
concurrent agents:

1. A claim is currently binary: **active** or **released**. There's no way to
   signal "I finished matching this and I'm about to commit/push" vs. "I'm
   still actively grinding it" vs. "I gave up, it's free again." An agent
   deciding whether to route around a held claim can't tell which of these is
   true, and a claim that's done-but-not-yet-pushed looks identical to one
   still being worked.
2. There's no cheap way to ask "has this range already been matched and
   merged upstream?" without cloning/pulling the repo and diffing committed
   `src/`. Schedulers currently derive this locally from git state, which
   works but means every agent re-derives it and can go stale mid-batch if a
   PR lands while a fan-out is running.

## Proposal 1: claim status field

Add a `status` field to a claim, alongside the existing active/released
lifecycle:

- `working` (default on `try-lock`) — actively being attempted.
- `done_pending_commit` — matched locally, not yet pushed/merged. Set via a
  new small write, e.g. `POST /api/claims/{id}/status` body
  `{handle, status: "done_pending_commit"}` (same auth as renew/release: the
  key that created the lock).
- Released (existing `release` endpoint) — either because it landed
  upstream, or because the agent gave up. Consider an optional `reason` on
  release (`"landed"` vs `"abandoned"`) purely for the note/audit trail, not
  for locking logic — release already frees the range either way.

`GET /api/claims` and `GET /api/claims/{id}` should surface `status` in the
response so a scheduler previewing a range can distinguish "someone's mid-work"
from "someone's about to land this, don't bother."

This does NOT change conflict rules — a claim in any non-released status still
blocks overlapping `try-lock` on that module. It's a visibility improvement,
not a new locking mode.

## Proposal 2: GitHub-derived completed-ranges cache

Have the claims service maintain its own record of what's actually landed on
`main`, derived from real merge events rather than self-reported completion —
this is the part that keeps it from drifting into a second source of truth:

- Preferred: register a GitHub webhook (push / PR-merged) on the repo the
  service already trusts, and update a `completed` set keyed by
  module+function (or address range, if that's easier to compute from the
  diff) whenever a merge lands on `main`.
- Fallback if webhook registration isn't available to the service operator:
  poll the public GitHub API (e.g. commits on `main`, or just re-fetch
  `src/` file listing) every few minutes. Latency of a few minutes is fine —
  match commits aren't high-frequency.
- Do NOT parse `#sm64ds-github` Discord messages for this. That channel is a
  downstream mirror of the same GitHub events; polling/webhooking GitHub
  directly is one hop closer to the source and doesn't depend on Discord
  message formatting staying stable.

Expose this as a new read endpoint, e.g. `GET /api/claims/completed?module=`,
returning whatever granularity is cheap to derive (function name and/or
address range) plus the commit SHA it was last synced against. This is
strictly a convenience cache for schedulers to skip already-matched work
before even attempting a `try-lock` — it should never be treated as
authoritative over the actual git history, and claim conflict logic should
keep working exactly as it does today regardless of this cache's state.

## Non-goals

- This is not meant to replace `progress.py` / the local match ledger as the
  source of truth for percentage-complete — those stay authoritative for the
  project's own accounting. The claims-service cache is only for schedulers
  deciding what to attempt next.
- No change to the auth model (`X-Api-Key`), TTL (24h), or the per-module
  overlap conflict rule.
