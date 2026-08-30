"""Local lock service (backed by Redis in local-infra/) so agents working the same
worktree don't step on each other's files or address ranges. Sibling of tools/claims.py
but LOCAL and SHORT-LIVED: claims.py coordinates with tangos.dev across contributors
over the network (address ranges only); this coordinates parallel agents/forks on THIS
machine only, and covers both address ranges and plain filenames. Start it with:

    docker compose -f local-infra/docker-compose.yml up -d

Lock flow: acquire a set of resources (files and/or one address range) -> renew while
working -> release when done. All resources in one `acquire` call succeed or fail
together. Every lock has a TTL and expires on its own if an agent dies without
releasing, so a crash never wedges another agent forever.

Identity: AGENTLOCK_HOLDER env var, else the OS username (see _default_holder below).
Parallel agents/forks under the same OS user should each export a distinct
AGENTLOCK_HOLDER (e.g. a worktree or agent name) -- otherwise they can renew/release
each other's locks, since holder is the only ownership check (this is a local
convenience lock, not an auth system).

CLI:
  python tools/agentlock.py acquire --files src/actors/Actor.cpp include/dActor_c.h --note "migrating Actor" [--ttl 1800] [--wait 60]
  python tools/agentlock.py acquire --range ov006 0x020f0000 0x020f0100 --note "DetectClsn rewrite"
  python tools/agentlock.py acquire --files src/actors/Actor.cpp --range ov006 0x020f0000 0x020f0100
  python tools/agentlock.py renew   --files src/actors/Actor.cpp include/dActor_c.h
  python tools/agentlock.py renew   --range ov006 0x020f0000 0x020f0100
  python tools/agentlock.py release --files src/actors/Actor.cpp include/dActor_c.h
  python tools/agentlock.py release --range ov006 0x020f0000 0x020f0100 [--force]
  python tools/agentlock.py check   --files src/actors/Actor.cpp
  python tools/agentlock.py check   --range ov006 0x020f0000 0x020f0100
  python tools/agentlock.py list    [--module ov006]
"""
import json
import os
import pathlib
import sys
import time
import uuid

try:
    import redis
except ImportError:
    print("pip install redis  # the redis-py client, not the server", file=sys.stderr)
    raise

REPO = pathlib.Path(__file__).resolve().parent.parent
REDIS_URL = os.environ.get("AGENTLOCK_REDIS_URL", "redis://127.0.0.1:6379/0")
DEFAULT_TTL_S = 1800


def _default_holder():
    h = os.environ.get("AGENTLOCK_HOLDER")
    if h:
        return h.strip()
    import getpass
    try:
        return getpass.getuser()
    except Exception:
        return "anonymous"


HOLDER = _default_holder()

# ---------------------------------------------------------------------------
# Lua scripts. All acquire/release/renew are single EVALSHA round trips so a
# multi-resource lock is genuinely atomic (all-or-nothing), not a client-side
# loop that can race against another agent between checks.
# ---------------------------------------------------------------------------

# KEYS = exact "lock:file:<path>" keys. ARGV = [ttl_ms, holder, note, acquired_at]
_ACQUIRE_EXACT = """
local conflicts = {}
for i, k in ipairs(KEYS) do
  local v = redis.call('GET', k)
  if v then table.insert(conflicts, k .. '=' .. v) end
end
if #conflicts > 0 then return conflicts end
local payload = cjson.encode({holder=ARGV[2], note=ARGV[3], acquired_at=ARGV[4]})
for i, k in ipairs(KEYS) do redis.call('SET', k, payload, 'PX', ARGV[1]) end
return {}
"""

# KEYS = exact keys. ARGV = [holder, force]
_RELEASE_EXACT = """
local released, kept = {}, {}
for i, k in ipairs(KEYS) do
  local v = redis.call('GET', k)
  if not v then
    table.insert(released, k)
  else
    local ok, data = pcall(cjson.decode, v)
    if ok and (data.holder == ARGV[1] or ARGV[2] == '1') then
      redis.call('DEL', k)
      table.insert(released, k)
    else
      table.insert(kept, k)
    end
  end
end
return {released, kept}
"""

# KEYS = exact keys. ARGV = [ttl_ms, holder]
_RENEW_EXACT = """
local renewed, missing = {}, {}
for i, k in ipairs(KEYS) do
  local v = redis.call('GET', k)
  if not v then
    table.insert(missing, k)
  else
    local ok, data = pcall(cjson.decode, v)
    if ok and data.holder == ARGV[2] then
      redis.call('PEXPIRE', k, ARGV[1])
      table.insert(renewed, k)
    else
      table.insert(missing, k)
    end
  end
end
return {renewed, missing}
"""

# No KEYS (pattern scan inside the script). ARGV = [module, start, end, ttl_ms, holder, note, id, acquired_at]
# Small local keyspace only (tens of locks) -- KEYS is fine here, avoid it at scale.
_ACQUIRE_RANGE = """
local pattern = 'lock:addr:' .. ARGV[1] .. ':*'
local existing = redis.call('KEYS', pattern)
local new_s, new_e = tonumber(ARGV[2]), tonumber(ARGV[3])
local conflicts = {}
for i, k in ipairs(existing) do
  local s, e = string.match(k, ':(%d+)-(%d+):[^:]+$')
  if s and e then
    s, e = tonumber(s), tonumber(e)
    if s < new_e and new_s < e then
      local v = redis.call('GET', k)
      table.insert(conflicts, k .. '=' .. (v or ''))
    end
  end
end
if #conflicts > 0 then return conflicts end
local key = 'lock:addr:' .. ARGV[1] .. ':' .. ARGV[2] .. '-' .. ARGV[3] .. ':' .. ARGV[7]
local payload = cjson.encode({module=ARGV[1], start=new_s, stop=new_e, holder=ARGV[5], note=ARGV[6], acquired_at=ARGV[8]})
redis.call('SET', key, payload, 'PX', ARGV[4])
return {}
"""

# ARGV = [module, start, end, holder, force]
_RELEASE_RANGE = """
local pattern = 'lock:addr:' .. ARGV[1] .. ':' .. ARGV[2] .. '-' .. ARGV[3] .. ':*'
local existing = redis.call('KEYS', pattern)
local released = {}
for i, k in ipairs(existing) do
  local v = redis.call('GET', k)
  if v then
    local ok, data = pcall(cjson.decode, v)
    if ok and (data.holder == ARGV[4] or ARGV[5] == '1') then
      redis.call('DEL', k)
      table.insert(released, k)
    end
  end
end
return released
"""

# ARGV = [module, start, end, ttl_ms, holder]
_RENEW_RANGE = """
local pattern = 'lock:addr:' .. ARGV[1] .. ':' .. ARGV[2] .. '-' .. ARGV[3] .. ':*'
local existing = redis.call('KEYS', pattern)
local renewed = {}
for i, k in ipairs(existing) do
  local v = redis.call('GET', k)
  if v then
    local ok, data = pcall(cjson.decode, v)
    if ok and data.holder == ARGV[5] then
      redis.call('PEXPIRE', k, ARGV[4])
      table.insert(renewed, k)
    end
  end
end
return renewed
"""


class LockError(RuntimeError):
    pass


def _client():
    c = redis.from_url(REDIS_URL, decode_responses=True)
    try:
        c.ping()
    except redis.exceptions.ConnectionError as e:
        raise LockError(
            f"can't reach redis at {REDIS_URL} -- start it with "
            f"'docker compose -f local-infra/docker-compose.yml up -d' ({e})"
        )
    return c


def _norm_file(p):
    path = pathlib.Path(p)
    try:
        path = path.resolve().relative_to(REPO)
    except ValueError:
        path = pathlib.Path(p)
    return "lock:file:" + path.as_posix()


def _now():
    return time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())


def acquire(c, files=None, addr_range=None, holder=HOLDER, note="", ttl=DEFAULT_TTL_S,
            wait=0, poll=3):
    """addr_range = (module, start, end) ints. Returns (ok, conflicts:list[str])."""
    file_keys = [_norm_file(f) for f in (files or [])]
    ttl_ms = str(int(ttl * 1000))
    deadline = time.time() + wait

    while True:
        conflicts = []
        range_acquired = False

        if addr_range:
            module, start, end = addr_range
            rid = uuid.uuid4().hex[:8]
            res = c.eval(_ACQUIRE_RANGE, 0, module, start, end, ttl_ms, holder, note,
                        rid, _now())
            if res:
                conflicts.extend(res)
            else:
                range_acquired = True

        if not conflicts and file_keys:
            res = c.eval(_ACQUIRE_EXACT, len(file_keys), *file_keys, ttl_ms, holder,
                        note, _now())
            if res:
                conflicts.extend(res)
                if range_acquired:
                    # roll back so the whole acquire() call is all-or-nothing
                    module, start, end = addr_range
                    c.eval(_RELEASE_RANGE, 0, module, start, end, holder, "1")

        if not conflicts:
            return True, []
        if time.time() >= deadline:
            return False, conflicts
        time.sleep(poll)


def release(c, files=None, addr_range=None, holder=HOLDER, force=False):
    released = []
    if files:
        file_keys = [_norm_file(f) for f in files]
        rel, kept = c.eval(_RELEASE_EXACT, len(file_keys), *file_keys, holder,
                           "1" if force else "0")
        released.extend(rel)
        if kept:
            print(f"  not released (held by someone else): {kept}", file=sys.stderr)
    if addr_range:
        module, start, end = addr_range
        rel = c.eval(_RELEASE_RANGE, 0, module, start, end, holder, "1" if force else "0")
        released.extend(rel)
    return released


def renew(c, files=None, addr_range=None, holder=HOLDER, ttl=DEFAULT_TTL_S):
    ttl_ms = str(int(ttl * 1000))
    renewed = []
    if files:
        file_keys = [_norm_file(f) for f in files]
        r, missing = c.eval(_RENEW_EXACT, len(file_keys), *file_keys, ttl_ms, holder)
        renewed.extend(r)
        if missing:
            print(f"  not renewed (expired or not yours): {missing}", file=sys.stderr)
    if addr_range:
        module, start, end = addr_range
        r = c.eval(_RENEW_RANGE, 0, module, start, end, ttl_ms, holder)
        renewed.extend(r)
    return renewed


def _display_key(k):
    """lock:addr:<module>:<start>-<end>:<id> -> same with start/end in hex, for
    printing only; the decimal form in Redis is untouched."""
    parts = k.split(":")
    if len(parts) == 5 and parts[0] == "lock" and parts[1] == "addr":
        span = parts[3]
        if "-" in span:
            s, e = span.split("-", 1)
            if s.isdigit() and e.isdigit():
                parts[3] = f"0x{int(s):08x}-0x{int(e):08x}"
                return ":".join(parts)
    return k


def list_locks(c, module=None):
    rows = []
    for k in c.scan_iter("lock:*"):
        v = c.get(k)
        ttl = c.pttl(k)
        if v is None:
            continue
        try:
            data = json.loads(v)
        except ValueError:
            data = {}
        if module and not (k.startswith(f"lock:addr:{module}:") or module == "*"):
            continue
        rows.append((_display_key(k), data.get("holder", "?"), data.get("note", ""),
                    round(ttl / 1000, 1) if ttl and ttl > 0 else "?"))
    return rows


def _parse_addr(s):
    return int(s, 0)


def main():
    a = sys.argv
    if len(a) < 2:
        print(__doc__)
        return

    def opt(name, default=None):
        return a[a.index(name) + 1] if name in a else default

    def flag(name):
        return name in a

    def multi(name):
        if name not in a:
            return []
        out, i = [], a.index(name) + 1
        while i < len(a) and not a[i].startswith("--"):
            out.append(a[i])
            i += 1
        return out

    cmd = a[1]
    files = multi("--files") or None
    range_args = multi("--range")
    addr_range = (range_args[0], _parse_addr(range_args[1]), _parse_addr(range_args[2])) \
        if len(range_args) == 3 else None
    holder = opt("--holder", HOLDER)
    note = opt("--note", "")
    ttl = float(opt("--ttl", DEFAULT_TTL_S))
    wait = float(opt("--wait", 0))
    force = flag("--force")

    if not files and not addr_range and cmd in ("acquire", "release", "renew", "check"):
        print("need --files <path...> and/or --range <module> <start> <end>", file=sys.stderr)
        sys.exit(2)

    try:
        c = _client()
    except LockError as e:
        print(f"error: {e}", file=sys.stderr)
        sys.exit(1)

    if cmd == "acquire":
        ok, conflicts = acquire(c, files, addr_range, holder, note, ttl, wait)
        if ok:
            print(f"acquired (holder={holder}, ttl={ttl:.0f}s)")
        else:
            print("CONFLICT -- held by someone else:")
            for line in conflicts:
                key, _, rest = line.partition("=")
                print(f"  {_display_key(key)}={rest}")
            sys.exit(1)
    elif cmd == "release":
        released = [_display_key(k) for k in release(c, files, addr_range, holder, force)]
        print(f"released: {released}" if released else "nothing released")
    elif cmd == "renew":
        renewed = [_display_key(k) for k in renew(c, files, addr_range, holder, ttl)]
        print(f"renewed (ttl={ttl:.0f}s): {renewed}" if renewed else "nothing renewed")
    elif cmd == "check":
        rows = list_locks(c, addr_range[0] if addr_range else None)
        keys_of_interest = set([_norm_file(f) for f in (files or [])])
        for k, h, note_, ttl_left in rows:
            if files and k not in keys_of_interest:
                continue
            print(f"  {k}  holder={h}  ttl={ttl_left}s  note={note_!r}")
        if not rows:
            print("  free")
    elif cmd == "list":
        module = opt("--module", "*")
        for k, h, note_, ttl_left in list_locks(c, module):
            print(f"  {k}  holder={h}  ttl={ttl_left}s  note={note_!r}")
    else:
        print(__doc__)


if __name__ == "__main__":
    main()
