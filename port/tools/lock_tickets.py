#!/usr/bin/env python3
"""Shared arrival-ticket fairness queue for the machine-wide test locks.

WHY THIS EXISTS (run link100, lane SLOTQ). port/tools/build_lock.py (the one
full port build) grew a "first in, first served" waiting queue: a directory of
ticket files named <arrival_epoch_ns>-<pid>.json beside the lockfile, read
oldest-arrival-first, with a dead-pid or past-cap ticket broken exactly the
way a dead-pid or past-cap LOCK is broken. port/tools/slot_lock.py (the one
windowed test slot) needed the identical fairness fix for the identical
reason: a lane running a proof that takes the slot once per rung lost the
poll race against other lanes' batteries for 100+ minutes and never completed
a rung. Rather than fork build_lock.py's ~180 lines of read/stale/write/
describe logic a second time with the labels changed, this module is that
logic, factored out once and shared. build_lock.py and slot_lock.py each keep
their own acquire()/CLI/docstrings -- what moved here is only the part that
was byte-for-byte (or parameter-for-parameter) identical between them.

WHAT DIFFERS PER CALLER, so every function below takes it as a parameter
rather than assuming it:
  - EXTRA TICKET FIELDS beyond pid/host/label/arrived: build_lock's tickets
    also carry `root` (which worktree is waiting); slot_lock's do not.
  - THE STALE-AGE CAP: build_lock reads it fresh from max_hold_seconds() (env-
    overridable) on every call; slot_lock passes its module constant. Callers
    pass the number, not a policy, so this module has no opinion on where it
    comes from.
  - THE LOG TAG: messages are prefixed "[build_lock] ..." or "[slot_lock] ..."
    so a mixed log still says which lock a line is about.

pid_alive() -- the Windows/POSIX liveness probe -- is shared for the same
reason: build_lock.py and slot_lock.py had byte-identical copies of it.

None of this changes either lock's FILE FORMAT or its stale rules for the LOCK
itself (see build_lock.py's _stale_reason / slot_lock.py's _is_stale, which
stay in their own modules untouched) -- only the WAITING QUEUE logic is here.
"""

import json
import os
import socket
import sys
import time

# The ticket directory sits beside the lockfile, named after it:
# <name>.lock -> <name>.tickets/. Shared so a lockfile and its queue always
# move together and two different lockfiles in one directory never share one.
TICKETS_SUFFIX = ".tickets"


def pid_alive(pid):
    """True if a process with this pid currently exists.

    Windows os.kill(pid, 0) does NOT probe -- for a non-CTRL signal it calls
    TerminateProcess, which would kill the holder -- so query the process
    object directly through the Win32 API. POSIX uses signal 0.
    """
    if pid is None or pid <= 0:
        return False
    if os.name == "nt":
        import ctypes
        from ctypes import wintypes
        PROCESS_QUERY_LIMITED_INFORMATION = 0x1000
        STILL_ACTIVE = 259
        ERROR_INVALID_PARAMETER = 87
        kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
        kernel32.OpenProcess.restype = wintypes.HANDLE
        kernel32.OpenProcess.argtypes = (wintypes.DWORD, wintypes.BOOL,
                                         wintypes.DWORD)
        handle = kernel32.OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,
                                      False, int(pid))
        if not handle:
            err = ctypes.get_last_error()
            # Invalid parameter == no such pid == dead. Access denied and the
            # like mean the process exists but we may not open it == alive.
            return err != ERROR_INVALID_PARAMETER
        try:
            code = wintypes.DWORD()
            if kernel32.GetExitCodeProcess(handle, ctypes.byref(code)):
                return code.value == STILL_ACTIVE
            return True
        finally:
            kernel32.CloseHandle(handle)
    else:
        try:
            os.kill(pid, 0)
        except ProcessLookupError:
            return False
        except PermissionError:
            return True
        return True


def held_for(acquired):
    """Seconds since an epoch timestamp field, or None if unknown/unparseable."""
    if not acquired:
        return None
    try:
        return time.time() - float(acquired)
    except (TypeError, ValueError):
        return None


def tickets_dir(lock_path, suffix=TICKETS_SUFFIX):
    """The queue directory beside a lockfile: <name>.lock -> <name><suffix>/."""
    base = os.path.basename(lock_path)
    stem = os.path.splitext(base)[0] or base
    return os.path.join(os.path.dirname(lock_path), stem + suffix)


def ticket_name(arrival_ns, pid):
    return f"{int(arrival_ns)}-{int(pid)}.json"


def parse_ticket_name(name):
    """(arrival_ns, pid) from '<ns>-<pid>.json', or None if not shaped like one."""
    if not name.endswith(".json"):
        return None
    arrival, sep, pid = name[:-len(".json")].partition("-")
    if not sep:
        return None
    try:
        return int(arrival), int(pid)
    except ValueError:
        return None


def _ticket_body(full):
    """The ticket's JSON fields, or {} for one that is missing or half-written.

    A body we cannot read is not a broken ticket: the pid and the arrival come
    from the filename, so an empty body costs the messages a label, nothing
    more.
    """
    try:
        with open(full, "r", encoding="utf-8") as f:
            data = json.load(f)
    except (OSError, ValueError):
        return {}
    return data if isinstance(data, dict) else {}


def ticket_stale_reason(t, cap_seconds):
    """Why this ticket is stale, or None if it is a live waiter's place in line.

    The same two rules the lock itself uses, for the same reason -- a killed
    waiter must not wedge the queue behind it: a dead pid (the primary,
    immediate check), then an age past cap_seconds (the backstop for a pid the
    OS has reused, or a waiter frozen rather than killed). A ticket naming our
    own pid is never stale: this process is demonstrably alive and polling,
    and self-breaking would only print noise.
    """
    if t["pid"] == os.getpid():
        return None
    if t["pid"] is None:
        return "the ticket names no waiter (bad filename)"
    if not pid_alive(t["pid"]):
        return f"waiter pid {t['pid']} is gone"
    try:
        age = time.time() - os.path.getmtime(t["path"])
    except OSError:
        return None  # it was released while we looked; nothing to break
    if age > cap_seconds:
        return f"queued {age:.0f}s, past the {cap_seconds:.0f}s cap"
    return None


def read_tickets(lock_path, extra_fields=(), cap_seconds=1800):
    """Every ticket beside lock_path, oldest arrival first, each with 'stale'.

    Read-only: nothing here removes anything, so a `status` caller can show
    the queue -- a stale entry included, marked as such -- without changing
    who gets the lock next. extra_fields names any JSON fields beyond
    pid/host/label/arrived a caller's tickets carry (build_lock: ("root",);
    slot_lock: ()).
    """
    tdir = tickets_dir(lock_path)
    try:
        names = os.listdir(tdir)
    except OSError:
        return []
    out = []
    for name in names:
        if not name.endswith(".json"):
            continue  # not a ticket (a scratch file); not ours to judge
        full = os.path.join(tdir, name)
        parsed = parse_ticket_name(name)
        arrival_ns, pid = parsed if parsed else (-1, None)
        body = _ticket_body(full)
        t = {"name": name, "path": full, "arrival_ns": arrival_ns, "pid": pid,
             "label": body.get("label", ""), "arrived": body.get("arrived")}
        for f in extra_fields:
            t[f] = body.get(f, "")
        t["stale"] = ticket_stale_reason(t, cap_seconds)
        out.append(t)
    out.sort(key=lambda t: (t["arrival_ns"], t["pid"] or 0, t["name"]))
    return out


def describe_ticket(t, extra_fields=()):
    """A one-line 'pid N (label L[, field V...]), queued Ns' for messages."""
    bits = f"pid {t['pid']}"
    rest = [(f, t.get(f)) for f in extra_fields if t.get(f)]
    if t.get("label"):
        bits += f" (label {t['label']!r}"
        bits += ("".join(f", {name} {val!r}" for name, val in rest) + ")")
    elif rest:
        bits += " (" + ", ".join(f"{name} {val!r}" for name, val in rest) + ")"
    waited = held_for(t.get("arrived"))
    if waited is not None:
        bits += f", queued {waited:.0f}s"
    return bits


def break_stale_tickets(lock_path, extra_fields=(), cap_seconds=1800, tag="lock"):
    """The queue with every stale ticket broken (loudly), oldest first.

    Prints '[<tag>] BROKE STALE TICKET (...)' the way the lock itself prints
    'BROKE STALE LOCK' -- a killed waiter must not wedge the queue behind it.
    """
    live = []
    for t in read_tickets(lock_path, extra_fields, cap_seconds):
        if not t["stale"]:
            live.append(t)
            continue
        try:
            os.remove(t["path"])
        except OSError:
            # Another waiter broke it first, or it was released between the
            # read and here. Either way it is not a place in line any more,
            # and the one thing we must not do is leave a dead ticket at the
            # head.
            continue
        print(f"[{tag}] BROKE STALE TICKET ({t['stale']}): was "
              f"{describe_ticket(t, extra_fields)} -- ticket {t['path']}. A "
              f"killed waiter must not wedge the queue behind it.",
              file=sys.stderr)
    return live


def write_ticket(lock_path, label, extra=None, arrival_ns=None):
    """Join the queue. Returns (ticket_dict, None), or (None, exception).

    Passing arrival_ns re-creates a ticket that was broken under us while
    still polling, keeping our ORIGINAL place in line. A caller whose queue
    directory cannot be written gets (None, the OSError) back rather than a
    raise, so fairness can never fail CLOSED and wedge a waiter behind a turn
    it can never take -- see each caller's own fallback-to-unfair handling.
    """
    tdir = tickets_dir(lock_path)
    if arrival_ns is None:
        arrival_ns = time.time_ns()
    name = ticket_name(arrival_ns, os.getpid())
    full = os.path.join(tdir, name)
    payload = {"pid": os.getpid(), "host": socket.gethostname(),
               "label": label or "", "arrived": arrival_ns / 1e9}
    for k, v in (extra or {}).items():
        payload[k] = v or ""
    try:
        os.makedirs(tdir, exist_ok=True)
        with open(full, "w", encoding="utf-8") as f:
            f.write(json.dumps(payload))
    except OSError as e:
        return None, e
    return {"name": name, "path": full, "arrival_ns": arrival_ns,
            "pid": os.getpid()}, None


def remove_ticket(ticket):
    """Leave the queue. Idempotent, and safe on a ticket already broken."""
    if not ticket:
        return
    try:
        os.remove(ticket["path"])
    except OSError:
        pass


def queue_position(live, ticket):
    """Our 1-based place in `live`, or None if we are not in it."""
    if ticket is None:
        return None
    for i, t in enumerate(live, 1):
        if t["name"] == ticket["name"]:
            return i
    return None
