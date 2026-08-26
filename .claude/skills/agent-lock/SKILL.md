---
name: agent-lock
description: Take a short-lived local lock on a set of filenames and/or an address range before editing them, so parallel agents/forks in this repo don't grind the same files or ROM range at once. Use before touching shared files (headers, config/rombuild-versions.txt, build/) in any multi-agent or orchestrator workflow, before migrating a class whose address range another agent might also be working, or when a build/edit fails in a way that looks like another process touched the same files concurrently. NOT for coordinating with other human contributors over the network - that is tools/claims.py / CLAIMS.md / tangos.dev.
---

# Local agent locking (Redis, this machine only)

`tools/agentlock.py` is a **local** lock, backed by a Redis container in
`local-infra/`. It exists for the case `tools/claims.py` doesn't cover: several agents
(forks, worktrees, a Workflow's parallel stages) running on **this machine** in **this
session**, about to edit the same file or the same address range at the same time.

It is not a substitute for `tools/claims.py` — that one coordinates with other
contributors over the network and only understands address ranges. This one also locks
plain filenames, has near-zero latency, and every lock expires on its own (crash-safe).

## When to reach for it

- An orchestrator is about to fan out several agents/forks that each edit files —
  especially shared ones like headers, `config/rombuild-versions.txt`, `CLAIMS.md`, or
  anything under `build/` (concurrent builds there already fake failures — see memory
  `concurrent-builds-fake-failures`).
- Two agents might independently pick the same class/function address range to migrate
  or byte-match in the same run.
- You're not sure two agents' file sets overlap — cheaper to lock and find out than to
  debug a corrupted merge afterward.

Skip it for read-only work (matching research, reading source, running `fdiff`) and for
a single agent working alone — it's overhead with no payoff there.

## Setup (once per machine, or after a restart)

```
docker compose -f local-infra/docker-compose.yml up -d
```

No persistence by design — a restart starts with an empty lock table, never resurrects
stale locks. If `agentlock.py` can't reach Redis it says so and names this command.

## Identity

Locks are owned by a `holder` string: `AGENTLOCK_HOLDER` env var, else the OS username.
**If you are one of several agents/forks running concurrently under the same OS user,
set a distinct `AGENTLOCK_HOLDER` per agent** (its worktree name or agent name is a good
choice) — otherwise it can renew or release a sibling agent's locks by accident, since
holder identity is the only check (this is a local convenience lock, not an auth
system).

## Core flow

Acquire is all-or-nothing across every resource in one call — if any file or the range
is already held, nothing is locked and you get back exactly what's holding it.

```
python tools/agentlock.py acquire --files src_tu/actors/Actor.cpp include/dActor_c.h --note "migrating Actor" --ttl 1800
python tools/agentlock.py acquire --range ov006 0x020f0000 0x020f0100 --note "DetectClsn rewrite"
python tools/agentlock.py acquire --files src_tu/actors/Actor.cpp --range ov006 0x020f0000 0x020f0100
```

`--wait N` polls for up to N seconds instead of failing immediately (useful when you'd
rather queue behind another agent than pick different work).

Long edits should renew before the TTL (default 1800s) runs out — `list`/`check` show
remaining TTL:

```
python tools/agentlock.py renew --files src_tu/actors/Actor.cpp include/dActor_c.h --ttl 1800
```

Release when done — always, even if the edit failed, so you don't sit on a lock for the
next 30 minutes:

```
python tools/agentlock.py release --files src_tu/actors/Actor.cpp include/dActor_c.h
python tools/agentlock.py release --range ov006 0x020f0000 0x020f0100
```

Check before planning work, or list everything currently held:

```
python tools/agentlock.py check --files src_tu/actors/Actor.cpp
python tools/agentlock.py list --module ov006
python tools/agentlock.py list
```

For a human watching several agents at once, `python tools/agentlock_web.py` serves a
live plain-HTML view (two reports: all active locks, and ranges grouped by module) at
http://127.0.0.1:8787/ — read-only, no setup.

## Address ranges

`--range <module> <start> <end>` takes a half-open `[start, end)` byte range, hex or
decimal (`0x020f0000` or `34537472`). Overlap is checked precisely — two ranges that
merely share a Redis key bucket are not treated as conflicting, and two that share even
one byte are. Locking `ov006 0x020f0000 0x020f0100` blocks anyone requesting any range
that intersects it; a neighboring `0x020f0100 0x020f0200` is unaffected.

## Orchestrator usage (Workflow / Agent fan-out)

Before dispatching a batch of agents that each own a class or file set, acquire each
agent's resources from the orchestrator BEFORE spawning it (not inside the agent — a
conflict should mean "give this agent different work," not "let it start and then
fail"). Pass the resource list and a distinct `AGENTLOCK_HOLDER` into each agent's
prompt/env, and release once that agent reports done (or on any error path — don't let
a failed agent leave the lock TTL as the only cleanup, since 30 minutes idle blocks
everyone else who wanted that file).

## What it does NOT do

- Does not coordinate across machines or with other human contributors — for that,
  `tools/claims.py` / `CLAIMS.md` / tangos.dev remain authoritative.
- Does not stop a human editing a file by hand outside these tools.
- Does not persist across a Redis restart — treat locks as session-scoped, not a
  durable record of who-did-what (that's git history / CLAIMS.md).
