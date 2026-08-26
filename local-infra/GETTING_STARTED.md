# Local agent lock — getting started

A tiny Redis container, local to your machine, so parallel agents in this repo don't
edit the same file or the same ROM address range at the same time. Not for coordinating
with other contributors — that's `tools/claims.py` / tangos.dev.

## 1. Start Redis

```
docker compose -f local-infra/docker-compose.yml up -d
```

That's it. No password, no persistence — it's a scratch lock table for this session.
Bound to `127.0.0.1` only, so it's not reachable off your machine.

Check it's up:

```
docker compose -f local-infra/docker-compose.yml ps
```

Stop it when you're done (locks vanish with it — that's intended):

```
docker compose -f local-infra/docker-compose.yml down
```

## 2. Take a lock

```
python tools/agentlock.py acquire --files src/Actor.cpp src/Actor.h --note "migrating Actor" --ttl 1800
```

or an address range:

```
python tools/agentlock.py acquire --range ov006 0x020f0000 0x020f0100 --note "DetectClsn rewrite"
```

If someone else already holds it, you get told who and what — nothing is locked
half-way.

## 3. Do the work, then release it

```
python tools/agentlock.py release --files src/Actor.cpp src/Actor.h
```

Locks also expire on their own after the `--ttl` (default 1800s / 30 min), so a crashed
agent never wedges everyone else — but release explicitly when you're done, don't rely
on the timer.

## 4. See what's held

```
python tools/agentlock.py list
```

or watch it live in a browser — a plain-HTML dashboard, no JS, refreshes itself every
3s:

```
python tools/agentlock_web.py
```

then open http://127.0.0.1:8787/. Two reports: every active lock, and address-range
locks grouped by module (start/end sorted, so an overlap or gap is visible at a
glance). It's read-only and just queries Redis directly per request — nothing to
configure.

## Running two agents at once? Give each one a name

```
export AGENTLOCK_HOLDER=agent-actor-migration
```

Without this, two agents running as the same OS user can renew/release *each other's*
locks — the holder name is the only ownership check.

## Full command reference

See the header of `tools/agentlock.py` (`python tools/agentlock.py` with no args prints
it), or the `agent-lock` skill for when/how agents should use this in orchestrated work.
