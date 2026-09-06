#!/usr/bin/env python3
"""A machine-wide exclusive lock for the ONE full port build.

THE PROBLEM THIS EXISTS FOR. The port's full build is a single, machine-wide
resource in everything but name. Measured on this box: a full build from a cold
tree is 5m22s when it is the only build running, and 72 to 85 MINUTES when four
to six lanes build at once on six cores. The slowdown is not a fair share of the
CPU -- it is oversubscription plus memory pressure, and under that pressure the
OS starts killing cl.exe outright. Those kills surface as a compile error on a
random file that compiles fine the moment it is retried alone, so the lane that
loses does not read it as contention; it reads it as a broken source file and
starts debugging the wrong thing. Serialising the builds turns >70 minutes of
mutual interference and mystery reds back into a queue of ~5 minute builds.
port/tools/slot_lock.py already does exactly this for the ONE windowed test
slot; this is its sibling for the ONE full build, and deliberately mirrors it.

WHAT IT IS. A single lockfile at a fixed machine-wide path. Whoever holds it
owns the full build; everyone else waits. It is a plain file plus an atomic
O_CREAT|O_EXCL create, so it needs no server, no docker and no infra -- unlike
tools/agentlock.py, which is Redis-backed. The tradeoff is that it is a
whole-machine mutex over one resource rather than a general lock manager, which
is exactly the resource that was colliding. It lives beside the test-slot lock
(same directory, different filename) so the two serialise independently: a lane
compiling does not block a lane running a windowed test, and vice versa.

OPT-IN. Nothing here changes behaviour unless a caller asks for it. There is no
implicit hook into port/build-port.cmd or port/tools/battery.py -- this module
does not edit or import them, and with SM64DS_BUILD_LOCK unset a build runs
exactly as it did before, byte for byte. A caller opts in either by wrapping its
build through the CLI (`run`, below -- invoking it IS the opt-in) or by gating on
enabled(), which reads SM64DS_BUILD_LOCK. See "ADOPTING IT".

THE LOCKFILE PATH is machine-wide and NOT repo-relative -- every checkout and
worktree on the box must resolve to the SAME file or the lock does not serialise
across lanes. Resolution order:

  1. $SM64DS_BUILD_LOCK_PATH, if set -- an explicit override. Lanes that run
     with a PRIVATE %TEMP% (see below) MUST export the same value here or they
     will not see each other's lock.
  2. else <system temp>/sm64ds-test-slot/port_build.lock, where <system temp>
     is tempfile.gettempdir() (honours %TEMP%/%TMP%). The directory is shared
     with slot_lock.py on purpose: one place to look for both locks.

On this box %TEMP% is pointed at C:\\tmp for every lane (the AppData-Temp exe
block workaround), so (2) resolves to one shared file. BUT because a lane with a
private %TEMP% would silently lock its OWN file and reintroduce the collision as
a slow build with random cl.exe kills, opting into locking now REQUIRES the
explicit path: when SM64DS_BUILD_LOCK is set, acquire() raises
BuildLockMisconfigured unless $SM64DS_BUILD_LOCK_PATH is also set. The default in
(2) remains for direct/hand CLI use, where a single human on the box is not
racing another lane. acquire() also prints the resolved path once per process so
a wrong path shows in the log instead of failing silently.

THE LOCKFILE CONTENTS are JSON: pid, host, an epoch acquire time, a label, and
the build root (the worktree being built). The pid and time are what the
stale-break below reads; the label and root are what `status` and the timeout
message report, so a waiting lane can see WHICH worktree is hogging the build
and for how long, not just a bare pid.

TIMEOUT -- FAIL, NOT PROCEED. A caller that waits DEFAULT_ACQUIRE_TIMEOUT
seconds (SM64DS_BUILD_LOCK_TIMEOUT overrides) and still cannot get the build
raises BuildLockTimeout rather than building anyway. Proceeding would reintroduce
the exact oversubscription this module removes and hand back another mystery
compile error; failing produces a deterministic, clearly labelled infra message
naming the holder's pid, label, root and hold time. The default of one hour is
sized well above a serialised queue of several ~5 minute builds, and it is a
bound rather than an infinite hang.

STALE-LOCK BREAK -- a killed build must not wedge the box. A held lock is stale,
and is broken and re-acquired, when EITHER:
  * the holder pid is dead (the primary, immediate check -- a lane that is
    killed, or whose build is OOM-killed and aborts, loses its pid at once), OR
  * the lockfile is older than max_hold_seconds() (default 90 minutes,
    SM64DS_BUILD_LOCK_MAX_HOLD overrides): the backstop for a wedged-but-alive
    holder, or a pid the OS has since reused for something else.
The cap is well above any legitimate serialised build (5-6 minutes), so the
backstop never fires on a healthy holder; the pid check is what recovers a crash
promptly. Breaking is itself race-safe: the breaker unlinks and then re-creates
with O_EXCL, and if two lanes race to break the same stale lock only one wins the
create and the other loops back to waiting. Every break prints a loud line naming
what was broken (pid, label, root, hold time, and which rule fired), because a
silent break would hide a lane that is actually still building.

FAIRNESS -- FIRST IN, FIRST SERVED. Waiters used to re-poll the same O_CREAT|
O_EXCL create, so a release went to whichever process happened to poll next
rather than to the one that had waited longest. Measured in production: one
session took the build back to back (label tail2, then globals-baseline) while
another session's waiter (label bfix-battery) sat through 32+ minutes of its
60-minute budget, and a waiter that runs out its budget does not just wait
longer -- it FAILS, so the starved lane pays for the queue-jumper with a red
build. So a waiter now takes a ticket:

    <lockdir>/port_build.tickets/<arrival_epoch_ns>-<pid>.json

named for its arrival (nanoseconds, so the order over the queue is total) and
its pid (so a dead waiter is recognisable from the NAME, without reading the
file), carrying the same {pid, host, label, root, arrived} fields the lockfile
does, so `status` can name the queue the way it already names the holder. The
ticket is written when a waiter starts waiting and is removed by its owner on
acquire, on timeout, and on every other way out of acquire(): the CLI's existing
SIGINT/SIGTERM/SIGBREAK handlers raise through acquire(), so an interrupted lane
takes its ticket with it instead of leaving a hole in the queue.

THE RULE: a process may create the lockfile only when it holds the OLDEST live
ticket, or when there are no tickets at all. The no-ticket path is exactly what
it was: a direct `run` on an idle box takes the lock on its first attempt and
never writes a ticket or even creates the ticket directory. What changes is the
process that arrives while others are queued -- it does not attempt the create
at all, it goes to the back of the queue -- and that arrival is precisely the
barge that starved bfix-battery.

A STALE TICKET is broken like a stale lock and for the same reason: a killed
waiter must not wedge the queue behind it. Two rules, the same shape, and anyone
who sees one breaks it with a loud line -- the ticket's pid is dead (the
primary, immediate check), or the ticket file is older than max_hold_seconds()
(the backstop for a pid the OS has reused, or a waiter frozen rather than
killed). A live waiter whose ticket is broken under it re-creates the ticket
with its ORIGINAL arrival stamp on the next poll, so the backstop never costs a
polling waiter its place. The queue therefore cannot outlive its waiters: a dead
one is broken by pid, and a live one is bounded by its own timeout, at which
point it removes its own ticket on the way out.

ADOPTING IT. Neither of these files is edited by this module; both changes are
one line at the call site, to be made by whoever owns that file.

  a lane / a script -- wrap the build through the CLI, no file edits at all:

                        python port/tools/build_lock.py run --label w7 \\
                            --root C:/tmp/w7 -- cmd /c "port\\build-port.cmd"

                    `run` always takes the lock (invoking it IS the opt-in), so
                    it does not consult SM64DS_BUILD_LOCK. Pass --if-enabled to
                    make it consult the flag: with SM64DS_BUILD_LOCK unset the
                    child then runs unlocked, exactly as today.

  build-port.cmd  -- a self-wrap at the top of the script, opt-in via the flag,
                    so an unset environment is byte-for-byte today's behaviour:

                        if not defined SM64DS_BUILD_LOCK_HELD (
                            set "SM64DS_BUILD_LOCK_HELD=1"
                            python "%~dp0tools\\build_lock.py" run --if-enabled ^
                                --label build-port -- "%~f0" %*
                            exit /b
                        )

                    (bare `exit /b` propagates the child's errorlevel; the
                    HELD guard is what stops the re-exec recursing.)

  battery.py      -- for a python caller, the context manager, same shape as its
                    existing slot_lock use:

                        import build_lock
                        with build_lock.build(label="battery rebuild"):
                            subprocess.run(["cmd", "/c", str(build_port_cmd)])

                    or gate it the way battery gates the windowed slot:
                    `if build_lock.enabled(): ...`.

SELFTEST. `python port/tools/build_lock.py --selftest` proves the mechanism with
real processes and no ROM, compiler or build: two processes contend, the second
waits and proceeds when the first exits, the timeout path fires and names the
holder, both stale rules break a lock, and the fairness queue holds -- three
waiters acquire in ARRIVAL order (the first arrival deliberately polls slowest,
so poll luck cannot be what wins it), a fresh arrival will not barge a free lock
past an older live ticket, a stale ticket does not block a younger live waiter,
a timed-out waiter removes its ticket, and the no-ticket path is untouched.
port/tools/test_build_lock.py is the same coverage as unit tests (pytest, or
standalone where pytest is absent).
"""

import contextlib
import json
import os
import socket
import sys
import tempfile
import time

# The longest a full build should ever hold the lock is a handful of minutes
# (5m22s measured, serialised). MAX_HOLD is far above that so the age backstop
# never breaks a live build; the pid-dead check is what recovers a killed lane
# quickly, and this only covers a holder that is alive but wedged, or pid reuse.
MAX_HOLD_SECONDS = 90 * 60

# How long a waiter blocks before giving up. Above a queue of several serialised
# builds; a bound, never an infinite wait. On timeout the caller FAILS with the
# holder named rather than proceeding into the oversubscription.
DEFAULT_ACQUIRE_TIMEOUT = 60 * 60

# Poll cadence while waiting for the build. Builds are minutes long, so a
# one-second poll is plenty and costs nothing.
POLL_SECONDS = 1.0

# While waiting, say so this often, so a queued lane's log shows a queue rather
# than looking hung for an hour.
WAIT_NOTICE_SECONDS = 60

# The fairness queue lives beside the lockfile, in a directory named after it:
# port_build.lock -> port_build.tickets/. See FAIRNESS in the module docstring.
TICKETS_SUFFIX = ".tickets"


class BuildLockTimeout(TimeoutError):
    """Raised when the full-build lock could not be acquired within the timeout.

    This is an infra condition (another lane held the build too long), not a
    broken source file. The message names the holder's pid, label, root and hold
    time so the wait is attributable instead of mysterious.
    """


class BuildLockMisconfigured(RuntimeError):
    """SM64DS_BUILD_LOCK is on but no explicit SM64DS_BUILD_LOCK_PATH is set.

    A lane that opts into locking without an explicit shared path falls back to
    the default under %TEMP%, which does NOT serialise across lanes that run a
    private %TEMP% -- and that failure is SILENT: the lane looks locked, then
    four builds run at once and it surfaces as a 70-minute build with a random
    cl.exe kill, never as a missing lock. The lane that forgets the path is
    exactly the one that will not notice, so refuse loudly at acquire time
    instead of locking a private file.
    """


# Announce the resolved lockfile path once per process (a log makes a wrong or
# private path visible instead of silent). See acquire().
_path_announced = False

# Say once, not once a second, that this process could not take a queue ticket.
_ticket_warned = False


def _truthy(v):
    return str(v).strip().lower() not in ("", "0", "false", "no", "off")


def enabled():
    """Is the opt-in flag set? Callers that gate automatic locking read this."""
    return _truthy(os.environ.get("SM64DS_BUILD_LOCK", ""))


def lock_path():
    """The one machine-wide lockfile path. See the module docstring."""
    override = os.environ.get("SM64DS_BUILD_LOCK_PATH")
    if override:
        return override
    return os.path.join(tempfile.gettempdir(), "sm64ds-test-slot",
                        "port_build.lock")


def acquire_timeout():
    v = os.environ.get("SM64DS_BUILD_LOCK_TIMEOUT")
    if v:
        try:
            return float(v)
        except ValueError:
            pass
    return DEFAULT_ACQUIRE_TIMEOUT


def max_hold_seconds():
    """The hard cap after which even a live holder's lock is stale (seconds)."""
    v = os.environ.get("SM64DS_BUILD_LOCK_MAX_HOLD")
    if v:
        try:
            return float(v)
        except ValueError:
            pass
    return MAX_HOLD_SECONDS


def default_root():
    """The build root to record when a caller does not name one.

    This file lives at <root>/port/tools/build_lock.py, so two directories up is
    the worktree being built -- which is the useful thing to report, since every
    lane has its own worktree and the pid alone does not say which.
    """
    here = os.path.dirname(os.path.abspath(__file__))
    return os.path.abspath(os.path.join(here, os.pardir, os.pardir))


def _pid_alive(pid):
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


def _read_holder(path):
    """(pid, acquired_epoch, label, root, raw_text) of the holder.

    A missing or unparseable lockfile reads back as (None, None, "", "", text) --
    a lock nobody can be identified from is treated as dead-held, which the
    stale rules then break.
    """
    try:
        with open(path, "r", encoding="utf-8") as f:
            text = f.read()
    except OSError:
        return None, None, "", "", ""
    try:
        data = json.loads(text)
        return (data.get("pid"), data.get("acquired"),
                data.get("label", ""), data.get("root", ""), text)
    except (ValueError, AttributeError):
        return None, None, "", "", text


def _held_for(acquired):
    """Seconds the recorded holder has held it, or None if unknown."""
    if not acquired:
        return None
    try:
        return time.time() - float(acquired)
    except (TypeError, ValueError):
        return None


def _stale_reason(path):
    """Why the lock on disk is stale, or None if it is a healthy live hold.

    Two rules, in the order they are cheap and certain: a dead holder pid (the
    killed-build case), then an age past the hard cap (the wedged-but-alive and
    pid-reuse case). Returning the REASON rather than a bool is what lets the
    break print a line that says which rule fired.
    """
    pid, acquired, _, _, _ = _read_holder(path)
    if pid is None:
        # Unreadable or half-written: it names nobody, so nobody can ever be
        # shown to have finished with it. Treat it as dead-held rather than
        # blocking every build on the box for an hour behind a corrupt file.
        return "the lockfile names no holder (unreadable or truncated)"
    if not _pid_alive(pid):
        return f"holder pid {pid} is gone"
    # mtime is the backstop; also honour the recorded acquire time if present.
    try:
        age = time.time() - os.path.getmtime(path)
    except OSError:
        return "lockfile vanished while being read"
    held = _held_for(acquired)
    if held is not None:
        age = max(age, held)
    cap = max_hold_seconds()
    if age > cap:
        return f"held {age:.0f}s, past the {cap:.0f}s cap"
    return None


def _is_stale(path):
    """Is the lock currently on disk stale? See _stale_reason for the rules."""
    return _stale_reason(path) is not None


def _write_locked(fd, label, root):
    payload = json.dumps({
        "pid": os.getpid(),
        "host": socket.gethostname(),
        "acquired": time.time(),
        "label": label or "",
        "root": root or "",
    })
    os.write(fd, payload.encode("utf-8"))


def _try_create(path, label, root):
    """One atomic attempt to create the lockfile. True if we now hold it."""
    os.makedirs(os.path.dirname(path), exist_ok=True)
    try:
        fd = os.open(path, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
    except FileExistsError:
        return False
    try:
        _write_locked(fd, label, root)
    finally:
        os.close(fd)
    return True


def _describe_holder(path):
    """A one-line 'pid N (label L, root R), held Ns' for messages."""
    pid, acquired, label, root, _ = _read_holder(path)
    held = _held_for(acquired)
    bits = f"pid {pid}"
    if label:
        bits += f" (label {label!r}"
        bits += f", root {root!r})" if root else ")"
    elif root:
        bits += f" (root {root!r})"
    if held is not None:
        bits += f", held {held:.0f}s"
    return bits


def _break_stale(path):
    """Break a stale lock, loudly. True if a file was removed by us.

    Re-checks staleness under the same read the unlink relies on, so a lock that
    became healthy between the first check and here is left alone, and tolerates
    another breaker having already removed it. The subsequent _try_create in
    acquire() is the real race arbiter -- exactly one O_EXCL create can win.
    """
    reason = _stale_reason(path)
    if not reason:
        return False
    who = _describe_holder(path)
    try:
        os.remove(path)
    except OSError:
        return False
    print(f"[build_lock] BROKE STALE LOCK ({reason}): was {who} "
          f"-- lockfile {path}. A killed build must not wedge the box; if that "
          f"build is somehow still running, this box now has two.",
          file=sys.stderr)
    return True


# --- the fairness queue ---------------------------------------------------
# One file per waiter, named <arrival_epoch_ns>-<pid>.json. Everything the
# ordering and the stale check need is in the NAME, so the queue can be read and
# a dead waiter recognised without opening a single file; the body only carries
# what the messages print (label, root, host, arrival as seconds). See FAIRNESS.

def tickets_dir(path=None):
    """The queue directory beside the lockfile.

    Derived from the lockfile so the two always move together: the default
    <temp>/sm64ds-test-slot/port_build.lock gives
    <temp>/sm64ds-test-slot/port_build.tickets/. It is derived from the lock's
    own name rather than hardcoded so two different lockfiles in one directory
    (a test lockfile beside the real one) never share a queue.
    """
    if path is None:
        path = lock_path()
    base = os.path.basename(path)
    stem = os.path.splitext(base)[0] or base
    return os.path.join(os.path.dirname(path), stem + TICKETS_SUFFIX)


def _ticket_name(arrival_ns, pid):
    return f"{int(arrival_ns)}-{int(pid)}.json"


def _parse_ticket_name(name):
    """(arrival_ns, pid) from '<ns>-<pid>.json', or None if it is not one."""
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
    from the filename, so an empty body costs the messages a label, nothing more.
    """
    try:
        with open(full, "r", encoding="utf-8") as f:
            data = json.load(f)
    except (OSError, ValueError):
        return {}
    return data if isinstance(data, dict) else {}


def _ticket_stale_reason(t):
    """Why this ticket is stale, or None if it is a live waiter's place in line.

    The same two rules as the lock, for the same reason -- a killed waiter must
    not wedge the queue behind it: a dead pid (immediate, the killed-lane case),
    then an age past the same hard cap (pid reuse, or a waiter frozen rather
    than killed). Our OWN ticket is never stale: this process is demonstrably
    alive and polling, and self-breaking would only print noise.
    """
    if t["pid"] == os.getpid():
        return None
    if t["pid"] is None:
        return "the ticket names no waiter (bad filename)"
    if not _pid_alive(t["pid"]):
        return f"waiter pid {t['pid']} is gone"
    try:
        age = time.time() - os.path.getmtime(t["path"])
    except OSError:
        return None  # it was released while we looked; nothing to break
    cap = max_hold_seconds()
    if age > cap:
        return f"queued {age:.0f}s, past the {cap:.0f}s cap"
    return None


def _read_tickets(path=None):
    """Every ticket, oldest arrival first, each with a stale reason or None.

    Read-only: nothing here removes anything, so `status` can show the queue --
    a stale entry included, marked as such -- without changing who gets the
    build next. acquire() uses _live_queue(), which breaks the stale ones.
    """
    tdir = tickets_dir(path)
    try:
        names = os.listdir(tdir)
    except OSError:
        return []
    out = []
    for name in names:
        if not name.endswith(".json"):
            continue  # not a ticket (a scratch file); not ours to judge
        full = os.path.join(tdir, name)
        parsed = _parse_ticket_name(name)
        arrival_ns, pid = parsed if parsed else (-1, None)
        body = _ticket_body(full)
        t = {"name": name, "path": full, "arrival_ns": arrival_ns, "pid": pid,
             "label": body.get("label", ""), "root": body.get("root", ""),
             "arrived": body.get("arrived")}
        t["stale"] = _ticket_stale_reason(t)
        out.append(t)
    out.sort(key=lambda t: (t["arrival_ns"], t["pid"] or 0, t["name"]))
    return out


def queue(path=None):
    """The wait queue, oldest arrival first: who gets the build next, in order.

    Read-only, and stale entries are included with their reason in ["stale"] so
    a reader sees what is actually on disk. This is what `status` prints.
    """
    return _read_tickets(path)


def _describe_ticket(t):
    """A one-line 'pid N (label L, root R), queued Ns' for messages."""
    bits = f"pid {t['pid']}"
    if t["label"]:
        bits += f" (label {t['label']!r}"
        bits += f", root {t['root']!r})" if t["root"] else ")"
    elif t["root"]:
        bits += f" (root {t['root']!r})"
    waited = _held_for(t.get("arrived"))
    if waited is not None:
        bits += f", queued {waited:.0f}s"
    return bits


def _live_queue(path=None):
    """The queue with every stale ticket broken (loudly), oldest first."""
    live = []
    for t in _read_tickets(path):
        if not t["stale"]:
            live.append(t)
            continue
        try:
            os.remove(t["path"])
        except OSError:
            # Another waiter broke it first, or it was released between the read
            # and here. Either way it is not a place in line any more, and the
            # one thing we must not do is leave a dead ticket at the head.
            continue
        print(f"[build_lock] BROKE STALE TICKET ({t['stale']}): was "
              f"{_describe_ticket(t)} -- ticket {t['path']}. A killed waiter "
              f"must not wedge the queue behind it.", file=sys.stderr)
    return live


def _write_ticket(path, label, root, arrival_ns=None):
    """Join the queue. Returns our ticket, or None if it could not be written.

    Passing arrival_ns re-creates a ticket that was broken under us while we
    were still polling, keeping our ORIGINAL place in line. A queue we cannot
    write to (a read-only or vanished directory) must never wedge a build, so
    this returns None and acquire() falls back to the old free-for-all rather
    than raising, or waiting out a place in line it can never take. Fairness is
    an improvement on that behaviour, not something to fail a build over.
    """
    global _ticket_warned
    tdir = tickets_dir(path)
    if arrival_ns is None:
        arrival_ns = time.time_ns()
    name = _ticket_name(arrival_ns, os.getpid())
    full = os.path.join(tdir, name)
    payload = json.dumps({
        "pid": os.getpid(),
        "host": socket.gethostname(),
        "label": label or "",
        "root": root or "",
        "arrived": arrival_ns / 1e9,
    })
    try:
        os.makedirs(tdir, exist_ok=True)
        with open(full, "w", encoding="utf-8") as f:
            f.write(payload)
    except OSError as e:
        if not _ticket_warned:
            print(f"[build_lock] could not take a queue ticket in {tdir} ({e}); "
                  f"waiting UNFAIRLY (first to poll wins) rather than not at "
                  f"all", file=sys.stderr)
            _ticket_warned = True
        return None
    return {"name": name, "path": full, "arrival_ns": arrival_ns,
            "pid": os.getpid()}


def _ensure_ticket(ticket, path, label, root):
    """Our ticket, written on the first wait and re-created if it goes missing."""
    if ticket is None:
        return _write_ticket(path, label, root)
    if not os.path.exists(ticket["path"]):
        # Somebody broke it (the age backstop, or a race with a break). Keep our
        # arrival stamp so the break does not cost us our place in the queue.
        return _write_ticket(path, label, root, arrival_ns=ticket["arrival_ns"])
    return ticket


def _remove_ticket(ticket):
    """Leave the queue. Idempotent, and safe on a ticket already broken."""
    if not ticket:
        return
    try:
        os.remove(ticket["path"])
    except OSError:
        pass


def _queue_position(live, ticket):
    """Our 1-based place in the queue, or None if we are not in it."""
    if ticket is None:
        return None
    for i, t in enumerate(live, 1):
        if t["name"] == ticket["name"]:
            return i
    return None


def acquire(label="", root=None, timeout=None, poll=POLL_SECONDS):
    """Block until this process holds the full-build lock, or time out.

    Returns the lockfile path on success. Raises BuildLockTimeout if the build
    could not be had within `timeout` seconds (default from acquire_timeout()),
    naming the holder -- it never returns without the lock. A stale lock (dead
    holder pid, or older than max_hold_seconds()) is broken and re-acquired,
    loudly; the break is O_EXCL-raced so only one waiter wins it.

    Waiting is FIRST IN, FIRST SERVED: a waiter takes an arrival ticket and the
    lockfile is only created by the oldest live ticket, so a process that
    arrives while others are queued cannot take the build from under them. With
    nobody waiting this is the old path exactly -- one create attempt, no ticket
    written, not even a ticket directory. The ticket is removed on the way out
    of this function however it ends: acquired, timed out, or raised through
    (which is how the CLI's signal handlers take a ticket with them). See
    FAIRNESS in the module docstring.
    """
    global _path_announced
    # HARD REFUSAL: if a caller opted into locking (SM64DS_BUILD_LOCK) but did
    # not pin an explicit shared path, refuse rather than lock a private/default
    # file that silently fails to serialise across lanes. See
    # BuildLockMisconfigured.
    if enabled() and not os.environ.get("SM64DS_BUILD_LOCK_PATH"):
        raise BuildLockMisconfigured(
            "SM64DS_BUILD_LOCK is set but SM64DS_BUILD_LOCK_PATH is not. Locking "
            "without an explicit shared path silently fails to serialise across "
            "lanes that run a private %TEMP%. Export SM64DS_BUILD_LOCK_PATH to "
            "one machine-wide file in every lane, e.g. "
            "C:\\tmp\\sm64ds-test-slot\\port_build.lock")
    path = lock_path()
    if not _path_announced:
        print(f"[build_lock] full build lockfile: {path}", file=sys.stderr)
        _path_announced = True
    if timeout is None:
        timeout = acquire_timeout()
    if root is None:
        root = default_root()
    started = time.time()
    deadline = started + timeout
    next_notice = started + WAIT_NOTICE_SECONDS
    ticket = None
    # A queue we cannot write to is a queue we ignore: fairness must never be
    # able to fail CLOSED and leave a lane waiting for a turn it can never get.
    ticketless = False
    try:
        while True:
            # THE FAIRNESS GATE. An empty queue is today's path exactly: try the
            # create straight away, touching nothing. Otherwise only the oldest
            # live ticket may create the lockfile, so a fresh arrival queues
            # behind the waiters instead of taking the build from under them.
            live = _live_queue(path)
            head = live[0] if live else None
            our_turn = (head is None or ticketless
                        or (ticket is not None
                            and head["name"] == ticket["name"]))
            if our_turn:
                if _try_create(path, label, root):
                    return path
                # Somebody holds it. Break it if it is stale, otherwise wait.
                if _break_stale(path) and _try_create(path, label, root):
                    return path
            elif os.path.exists(path):
                # Not our turn, but a dead holder is everyone's problem: break
                # it now so the head of the queue finds a free lock on its next
                # poll instead of waiting for the one process that may take it.
                _break_stale(path)
            now = time.time()
            if now >= deadline:
                pos = _queue_position(live, ticket)
                place = ""
                if pos is not None:
                    place = (f" You were {pos} of {len(live)} in the queue, "
                             f"which is served in arrival order, so {pos - 1} "
                             f"lane(s) were ahead of you the whole time.")
                raise BuildLockTimeout(
                    f"could not acquire the full port build within {timeout:.0f}s: "
                    f"it is held by {_describe_holder(path)} (lockfile {path})."
                    f"{place} "
                    f"This is an infra wait, not a build fault; another lane is "
                    f"building. Re-run when the box is idle, or raise "
                    f"SM64DS_BUILD_LOCK_TIMEOUT. Building anyway is what this lock "
                    f"exists to prevent: concurrent builds take 70+ minutes and get "
                    f"their cl.exe killed under memory pressure.")
            # Entering (or still in) the wait: hold a place in line.
            ticket = _ensure_ticket(ticket, path, label, root)
            ticketless = ticket is None
            if now >= next_notice:
                waited = now - started
                pos = _queue_position(live, ticket)
                place = f", queue position {pos} of {len(live)}" if pos else ""
                print(f"[build_lock] waiting {waited:.0f}s for the full build; "
                      f"held by {_describe_holder(path)}{place}",
                      file=sys.stderr)
                next_notice = now + WAIT_NOTICE_SECONDS
            time.sleep(poll)
    finally:
        # Every exit path: acquired, timed out, or raised through (the CLI's
        # SIGINT/SIGTERM/SIGBREAK handlers land here). A ticket left behind
        # would hold up the queue until its pid was noticed dead.
        _remove_ticket(ticket)


def release(path=None):
    """Release the build lock IF this process holds it. Safe to call twice.

    Only removes the lockfile when its recorded pid is ours, so a lock that was
    broken as stale and re-acquired by another lane is never deleted out from
    under that lane.
    """
    if path is None:
        path = lock_path()
    pid, _, _, _, _ = _read_holder(path)
    if pid == os.getpid():
        try:
            os.remove(path)
        except OSError:
            pass


@contextlib.contextmanager
def build(label="", root=None, timeout=None, poll=POLL_SECONDS):
    """Context manager: hold the full-build lock for the block, release on exit.

        with build_lock.build(label="battery rebuild", root=r"C:/tmp/w7"):
            subprocess.run(["cmd", "/c", "port\\build-port.cmd"])

    Releases on a normal exit and on an exception, and only ever removes a
    lockfile this pid still owns.
    """
    path = acquire(label=label, root=root, timeout=timeout, poll=poll)
    try:
        yield path
    finally:
        release(path)


# --- CLI -----------------------------------------------------------------

def _print_queue(path):
    """The wait queue after the holder, oldest arrival first: who is next.

    Read-only on purpose -- `status` marks a stale ticket rather than breaking
    it, so looking at the queue never changes who gets the build next.
    """
    q = queue(path)
    if not q:
        print("  queue: empty (nobody waiting)")
        return
    print(f"  queue: {len(q)} waiting, oldest arrival first "
          f"(first in, first served)")
    for i, t in enumerate(q, 1):
        stale = f"  STALE: {t['stale']}" if t["stale"] else ""
        print(f"    {i}. {_describe_ticket(t)}{stale}")
    print(f"  tickets: {tickets_dir(path)}")


def _cli_status():
    path = lock_path()
    if not os.path.exists(path):
        print(f"build: FREE ({path} does not exist)")
        _print_queue(path)
        return 0
    pid, acquired, label, root, text = _read_holder(path)
    alive = _pid_alive(pid)
    reason = _stale_reason(path)
    held = _held_for(acquired)
    held_s = f", held {held:.0f}s" if held is not None else ""
    print(f"build: HELD by pid {pid} (alive={alive}, "
          f"stale={reason is not None}{held_s})")
    if label:
        print(f"  label: {label}")
    if root:
        print(f"  root: {root}")
    if reason:
        print(f"  stale because: {reason}")
    print(f"  lockfile: {path}")
    print(f"  contents: {text.strip()}")
    _print_queue(path)
    return 0


def _cli_run(argv, head, opt):
    """Hold the lock for the whole lifetime of a child command."""
    import signal
    import subprocess
    if "--" not in argv:
        print("usage: build_lock.py run [--label L] [--root R] [--timeout N] "
              "[--if-enabled] -- <cmd...>", file=sys.stderr)
        return 2
    child = argv[argv.index("--") + 1:]
    if not child:
        print("build_lock.py run: no command after --", file=sys.stderr)
        return 2
    # --if-enabled makes the wrap itself opt-in: with SM64DS_BUILD_LOCK unset
    # the child runs exactly as it would have, unlocked. Without the flag,
    # invoking `run` IS the opt-in and the lock is always taken.
    if "--if-enabled" in head and not enabled():
        return subprocess.run(child).returncode

    proc = None

    def _bail(signum, _frame):
        # Kill the child, then unwind through the context manager's finally so
        # the lock is released rather than left for the stale-break to clean up.
        if proc is not None and proc.poll() is None:
            try:
                proc.terminate()
            except OSError:
                pass
        raise KeyboardInterrupt(f"build_lock: signal {signum}")

    installed = {}
    for name in ("SIGINT", "SIGTERM", "SIGBREAK"):
        sig = getattr(signal, name, None)
        if sig is None:
            continue
        try:
            installed[sig] = signal.signal(sig, _bail)
        except (ValueError, OSError):
            pass
    try:
        with build(label=opt("--label", "cli run"),
                   root=opt("--root", None),
                   timeout=float(opt("--timeout", acquire_timeout()))):
            proc = subprocess.Popen(child)
            return proc.wait()
    except BuildLockTimeout as e:
        print(f"build: TIMEOUT -- {e}", file=sys.stderr)
        return 1
    except BuildLockMisconfigured as e:
        print(f"build: MISCONFIGURED -- {e}", file=sys.stderr)
        return 2
    except KeyboardInterrupt as e:
        print(f"build: interrupted ({e}); lock released", file=sys.stderr)
        return 130
    finally:
        for sig, old in installed.items():
            try:
                signal.signal(sig, old)
            except (ValueError, OSError):
                pass


# --- selftest ------------------------------------------------------------

# One queued waiter, as a real process: wait for the build, then append the
# instant it got it to a shared file. Running three of these against one holder
# is what proves the queue is served in arrival order rather than in poll order
# -- an in-process test could not, because a single process cannot lose a race
# to itself. Written to a temp file and run with:
#   python fair_worker.py <label> <pre-delay> <poll> <order-file> <tools-dir>
# Shared with port/tools/test_build_lock.py so both harnesses drive the exact
# same waiter, and it stays honest about the poll cadence each waiter uses.
_FAIR_WORKER = '''\
import os, sys, time
sys.path.insert(0, sys.argv[5])
import build_lock

label, delay, out = sys.argv[1], float(sys.argv[2]), sys.argv[4]
poll = float(sys.argv[3])
time.sleep(delay)
p = build_lock.acquire(label=label, root="C:/tmp/" + label, timeout=90, poll=poll)
with open(out, "a", encoding="utf-8") as f:
    f.write(repr(time.time()) + " " + label + "\\n")
    f.flush()
    os.fsync(f.fileno())
time.sleep(0.2)
build_lock.release(p)
'''


def selftest():
    """The lock mechanism against real processes: no ROM, compiler or build.

    Covers, in order: a free lock reads FREE; acquire records our pid, label and
    root; TWO PROCESSES CONTEND -- a child holds the lock through `run` while
    this process (a) is refused by a short timeout whose message names the
    holder's pid, label and root, and (b) then waits on a long timeout and
    proceeds only after the child exits; the FAIRNESS QUEUE -- the no-ticket
    path never creates a ticket, a fresh arrival will not barge a free lock past
    an older live ticket, THREE REAL WAITERS acquire in arrival order (the first
    arrival polls slowest, so poll luck cannot be what wins it), a stale ticket
    does not block a younger live waiter, and a timed-out waiter removes its own
    ticket; both STALE rules break a lock (a dead holder pid, and an age past
    the cap) with a loud line; release never removes a foreign live lock; the
    opt-in flag without an explicit path is refused; and the context manager
    releases on an exception.
    """
    import io
    import pathlib
    import subprocess
    import tempfile

    ok = True

    def expect(cond, what, got=""):
        nonlocal ok
        if cond:
            print(f"  PASS {what}")
        else:
            print(f"  FAIL {what}: {got}")
            ok = False

    def plant(path, pid, acquired=None, label="planted", root="C:/tmp/planted"):
        with open(path, "w", encoding="utf-8") as f:
            json.dump({"pid": pid, "host": "selftest",
                       "acquired": time.time() if acquired is None else acquired,
                       "label": label, "root": root}, f)

    def plant_ticket(arrival_ns, pid, label="planted", root="C:/tmp/planted"):
        """A ticket somebody else left in the queue."""
        tdir = tickets_dir()
        os.makedirs(tdir, exist_ok=True)
        full = os.path.join(tdir, _ticket_name(arrival_ns, pid))
        with open(full, "w", encoding="utf-8") as f:
            json.dump({"pid": pid, "host": "selftest", "label": label,
                       "root": root, "arrived": arrival_ns / 1e9}, f)
        return full

    saved = {k: os.environ.get(k) for k in
             ("SM64DS_BUILD_LOCK", "SM64DS_BUILD_LOCK_PATH",
              "SM64DS_BUILD_LOCK_TIMEOUT", "SM64DS_BUILD_LOCK_MAX_HOLD")}
    me = os.path.abspath(__file__)
    with tempfile.TemporaryDirectory() as td:
        lf = str(pathlib.Path(td) / "port_build.lock")
        os.environ["SM64DS_BUILD_LOCK_PATH"] = lf
        for k in ("SM64DS_BUILD_LOCK", "SM64DS_BUILD_LOCK_TIMEOUT",
                  "SM64DS_BUILD_LOCK_MAX_HOLD"):
            os.environ.pop(k, None)
        try:
            # --- the path, and a free lock ---------------------------------
            expect(lock_path() == lf, "lock_path honours the explicit override",
                   lock_path())
            expect(not os.path.exists(lf), "the lock starts free")

            # --- acquire records who we are --------------------------------
            p = acquire(label="selftest", root="C:/tmp/selftest", timeout=2)
            pid, acq, label, root, _ = _read_holder(p)
            expect(pid == os.getpid() and label == "selftest"
                   and root == "C:/tmp/selftest" and acq is not None,
                   "acquire records pid, label, root and time",
                   (pid, label, root, acq))
            release(p)
            expect(not os.path.exists(lf), "release frees the lock")

            # --- TWO PROCESSES CONTEND -------------------------------------
            # A child takes the lock through `run` and holds it for ~6s while
            # its own grandchild sleeps. This is the real shape lanes use.
            env = dict(os.environ)
            env["SM64DS_BUILD_LOCK_PATH"] = lf
            holder = subprocess.Popen(
                [sys.executable, me, "run", "--label", "selftest-holder",
                 "--root", "C:/tmp/holder", "--timeout", "30", "--",
                 sys.executable, "-c", "import time; time.sleep(6)"],
                env=env, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            try:
                waited = 0.0
                while not os.path.exists(lf) and waited < 20:
                    time.sleep(0.1)
                    waited += 0.1
                expect(os.path.exists(lf),
                       "the child process took the lock", f"{waited:.1f}s")
                hpid, _, hlabel, hroot, _ = _read_holder(lf)
                expect(hpid == holder.pid and hlabel == "selftest-holder"
                       and hroot == "C:/tmp/holder",
                       "the lockfile names the child as holder",
                       (hpid, holder.pid, hlabel, hroot))
                expect(_stale_reason(lf) is None,
                       "a live foreign holder is NOT stale", _stale_reason(lf))

                # (a) the TIMEOUT path: we fail, we do not build anyway.
                t0 = time.time()
                try:
                    acquire(label="loser", timeout=1.0, poll=0.05)
                    expect(False, "a contended acquire times out",
                           "it returned instead")
                except BuildLockTimeout as e:
                    msg = str(e)
                    expect(time.time() - t0 >= 0.9,
                           "the loser actually waited before failing",
                           f"{time.time() - t0:.2f}s")
                    expect(str(holder.pid) in msg and "selftest-holder" in msg
                           and "C:/tmp/holder" in msg,
                           "the timeout names the holder pid, label and root",
                           msg)
                expect(_read_holder(lf)[0] == holder.pid,
                       "the loser left the holder's lock untouched")

                # (b) the WAIT path: block, then proceed once the child exits.
                t0 = time.time()
                p = acquire(label="second", root="C:/tmp/second",
                            timeout=45, poll=0.1)
                waited = time.time() - t0
                expect(waited >= 0.5,
                       "the second acquire waited for the first to finish",
                       f"{waited:.1f}s")
                expect(_read_holder(p)[0] == os.getpid(),
                       "the second acquire then holds the lock")
                expect(holder.wait(timeout=20) == 0,
                       "the holding child exited cleanly and released")
                release(p)
            finally:
                if holder.poll() is None:
                    holder.terminate()
                    holder.wait()

            # --- FAIRNESS: the no-ticket path is untouched -------------------
            # Nobody waiting: the lock is taken on the first attempt and the
            # ticket directory is never even created, so a direct `run` on an
            # idle box does exactly what it did before this queue existed. Done
            # against a FRESH lock path because the timeout above has already
            # made (and emptied) a queue directory for lf.
            fresh = str(pathlib.Path(td) / "fresh" / "port_build.lock")
            os.makedirs(os.path.dirname(fresh), exist_ok=True)
            os.environ["SM64DS_BUILD_LOCK_PATH"] = fresh
            p = acquire(label="uncontended", timeout=2)
            expect(_read_holder(p)[0] == os.getpid(),
                   "an uncontended acquire takes the lock immediately")
            expect(not os.path.isdir(tickets_dir()),
                   "and never creates the ticket directory", tickets_dir())
            release(p)
            os.environ["SM64DS_BUILD_LOCK_PATH"] = lf
            for t in queue():
                os.remove(t["path"])

            # --- FAIRNESS: a fresh arrival does not barge a waiter -----------
            # THE DEFECT. The lock is FREE, but a live waiter arrived first and
            # is holding a ticket. Under the old re-poll rule this process would
            # simply take the build (that is how a back-to-back acquirer starved
            # bfix-battery); it must now wait its turn and time out instead.
            sleeper = subprocess.Popen(
                [sys.executable, "-c", "import time; time.sleep(30)"])
            try:
                plant_ticket(time.time_ns() - 5 * 10**9, sleeper.pid,
                             label="arrived-first", root="C:/tmp/first")
                expect(not os.path.exists(lf), "the lock is free for this one")
                t0 = time.time()
                try:
                    acquire(label="barger", timeout=0.6, poll=0.05)
                    expect(False, "a fresh arrival waits behind an older ticket",
                           "it took the free lock instead")
                except BuildLockTimeout as e:
                    expect(time.time() - t0 >= 0.5,
                           "a fresh arrival waits behind an older ticket",
                           f"{time.time() - t0:.2f}s")
                    expect("in the queue" in str(e),
                           "and the timeout says where in the queue it was",
                           str(e))
                expect(not os.path.exists(lf),
                       "the free lock was left for the older waiter")
                expect([t["label"] for t in queue()] == ["arrived-first"],
                       "the barger removed its own ticket, the waiter kept its",
                       [t["label"] for t in queue()])
            finally:
                sleeper.terminate()
                sleeper.wait()

            # --- FAIRNESS: a stale ticket does not block a live waiter -------
            # The sleeper above is now dead, so its ticket is stale: a killed
            # waiter must not wedge the queue behind it the way a killed holder
            # must not wedge the box.
            err = io.StringIO()
            with contextlib.redirect_stderr(err):
                p = acquire(label="younger", timeout=2, poll=0.05)
            expect(_read_holder(p)[0] == os.getpid(),
                   "a younger live waiter got past the dead waiter's ticket")
            expect("BROKE STALE TICKET" in err.getvalue()
                   and "arrived-first" in err.getvalue(),
                   "the ticket break printed a loud line naming the waiter",
                   err.getvalue().strip())
            expect(queue() == [], "and the dead ticket is gone", queue())
            release(p)

            # --- FAIRNESS: THREE WAITERS ACQUIRE IN ARRIVAL ORDER ------------
            # Three real processes queue behind a holder (this process) and are
            # released together. The FIRST arrival deliberately polls slowest
            # (0.5s) and the two behind it poll fastest (0.02s), so under the
            # old re-poll rule the first arrival would essentially never win the
            # create; under the ticket rule it goes first every time.
            worker = pathlib.Path(td) / "fair_worker.py"
            worker.write_text(_FAIR_WORKER, encoding="utf-8")
            order_file = str(pathlib.Path(td) / "fair_order.txt")
            p = acquire(label="fair-holder", root="C:/tmp/holder", timeout=5)
            waiters, errfiles = [], []
            try:
                for i, (wlabel, wpoll) in enumerate(
                        (("fair-w1", "0.5"), ("fair-w2", "0.02"),
                         ("fair-w3", "0.02")), start=1):
                    ef = str(pathlib.Path(td) / (wlabel + ".err"))
                    errfiles.append(ef)
                    with open(ef, "wb") as fh:
                        waiters.append(subprocess.Popen(
                            [sys.executable, str(worker), wlabel, "0", wpoll,
                             order_file, os.path.dirname(me)], env=env,
                            stdout=subprocess.DEVNULL, stderr=fh))
                    # Wait for THIS waiter to be in the queue before starting the
                    # next: arrival order is then the spawn order, not a race on
                    # how long a python interpreter takes to start.
                    spent = 0.0
                    while len(queue()) < i and spent < 30:
                        time.sleep(0.05)
                        spent += 0.05
                    expect(len(queue()) == i,
                           f"{wlabel} joined the queue", len(queue()))
                expect([t["label"] for t in queue()]
                       == ["fair-w1", "fair-w2", "fair-w3"],
                       "the queue lists the three waiters in arrival order",
                       [t["label"] for t in queue()])
                release(p)          # the holder lets go; the queue drains
                for w in waiters:
                    w.wait(timeout=90)
                errs = "".join(open(f, encoding="utf-8", errors="replace").read()
                               for f in errfiles)
                expect(all(w.returncode == 0 for w in waiters),
                       "all three waiters exited cleanly",
                       [w.returncode for w in waiters])
                text = ""
                if os.path.exists(order_file):
                    text = open(order_file, encoding="utf-8").read()
                rows = [ln.split() for ln in text.splitlines() if ln.strip()]
                got = [r[1] for r in rows]
                stamps = [float(r[0]) for r in rows]
                expect(got == ["fair-w1", "fair-w2", "fair-w3"],
                       "the three waiters acquired in ARRIVAL order",
                       f"{got} / stderr: {errs.strip()}")
                expect(stamps == sorted(stamps),
                       "and their acquire timestamps are in that same order",
                       stamps)
                expect(queue() == [],
                       "every waiter removed its ticket on acquire", queue())
                expect(not os.path.exists(lf),
                       "and the last one released the lock")
            finally:
                for w in waiters:
                    if w.poll() is None:
                        w.terminate()
                        w.wait()
                release(lf)

            # --- FAIRNESS: a timed-out waiter takes its ticket with it -------
            # A ticket outliving its waiter would hold the queue up behind a
            # process that has already given up and gone home.
            sleeper = subprocess.Popen(
                [sys.executable, "-c", "import time; time.sleep(30)"])
            try:
                plant(lf, sleeper.pid, label="foreign-holder")
                try:
                    acquire(label="gives-up", timeout=0.6, poll=0.05)
                    expect(False, "the waiter timed out", "it acquired instead")
                except BuildLockTimeout:
                    expect(True, "the waiter timed out")
                expect(queue() == [],
                       "a timed-out waiter removed its own ticket",
                       [t["name"] for t in queue()])
            finally:
                sleeper.terminate()
                sleeper.wait()
            os.remove(lf)

            # --- STALE: a dead holder pid ----------------------------------
            plant(lf, 0x7FFFFFFF, label="crashed", root="C:/tmp/crashed")
            expect(_stale_reason(lf) is not None,
                   "a dead holder pid reads stale")
            err = io.StringIO()
            with contextlib.redirect_stderr(err):
                p = acquire(label="breaker", timeout=2)
            expect(_read_holder(p)[0] == os.getpid(),
                   "the stale lock was broken and taken over")
            expect("BROKE STALE LOCK" in err.getvalue()
                   and "crashed" in err.getvalue(),
                   "the break printed a loud line naming what it broke",
                   err.getvalue().strip())
            release(p)

            # --- STALE: past the hard cap ----------------------------------
            # The holder is THIS process (very much alive), so only the age rule
            # can fire. Ninety minutes is the default; override it to keep the
            # selftest instant.
            os.environ["SM64DS_BUILD_LOCK_MAX_HOLD"] = "5"
            old = time.time() - 60
            plant(lf, os.getpid(), acquired=old, label="wedged")
            os.utime(lf, (old, old))
            reason = _stale_reason(lf)
            expect(reason is not None and "cap" in reason,
                   "an over-cap lock reads stale by age even when alive", reason)
            err = io.StringIO()
            with contextlib.redirect_stderr(err):
                p = acquire(label="breaker2", timeout=2)
            expect("BROKE STALE LOCK" in err.getvalue()
                   and "past the" in err.getvalue(),
                   "the age break says which rule fired",
                   err.getvalue().strip())
            release(p)
            expect(max_hold_seconds() == 5.0,
                   "the cap is env-overridable", max_hold_seconds())
            os.environ.pop("SM64DS_BUILD_LOCK_MAX_HOLD", None)
            expect(max_hold_seconds() == MAX_HOLD_SECONDS
                   and MAX_HOLD_SECONDS == 5400,
                   "and defaults to 90 minutes", max_hold_seconds())

            # --- release never touches a foreign live lock -----------------
            sleeper = subprocess.Popen(
                [sys.executable, "-c", "import time; time.sleep(30)"])
            try:
                plant(lf, sleeper.pid, label="foreign")
                release(lf)
                expect(os.path.exists(lf),
                       "release leaves a foreign live lock alone")
            finally:
                sleeper.terminate()
                sleeper.wait()
            os.remove(lf)

            # --- the opt-in flag needs an explicit path ---------------------
            os.environ["SM64DS_BUILD_LOCK"] = "1"
            os.environ.pop("SM64DS_BUILD_LOCK_PATH", None)
            try:
                acquire(timeout=1)
                expect(False, "opt-in without a path is refused",
                       "it acquired instead")
            except BuildLockMisconfigured:
                expect(True, "opt-in without a path is refused")
            os.environ["SM64DS_BUILD_LOCK_PATH"] = lf
            p = acquire(label="flagged", timeout=2)
            expect(os.path.exists(p), "opt-in WITH a path is allowed")
            release(p)
            expect(enabled() is True, "enabled() reads the flag")
            os.environ["SM64DS_BUILD_LOCK"] = "off"
            expect(enabled() is False, "and 'off' is falsy")
            os.environ.pop("SM64DS_BUILD_LOCK", None)

            # --- the context manager ---------------------------------------
            with build(label="ctx", root="C:/tmp/ctx", timeout=2) as p:
                expect(_read_holder(p)[0] == os.getpid(),
                       "the context manager holds the lock")
            expect(not os.path.exists(lf),
                   "the context manager released on exit")
            try:
                with build(label="ctx", timeout=2):
                    raise ValueError("boom")
            except ValueError:
                pass
            expect(not os.path.exists(lf),
                   "the context manager released on an exception")

            # --- timeouts are env-overridable ------------------------------
            os.environ["SM64DS_BUILD_LOCK_TIMEOUT"] = "7"
            expect(acquire_timeout() == 7.0,
                   "the acquire timeout is env-overridable", acquire_timeout())
            os.environ.pop("SM64DS_BUILD_LOCK_TIMEOUT", None)
            expect(acquire_timeout() == DEFAULT_ACQUIRE_TIMEOUT
                   and DEFAULT_ACQUIRE_TIMEOUT == 3600,
                   "and defaults to 60 minutes", acquire_timeout())
        finally:
            for k, v in saved.items():
                if v is None:
                    os.environ.pop(k, None)
                else:
                    os.environ[k] = v

    print("selftest %s" % ("PASS" if ok else "FAIL"))
    return 0 if ok else 1


def main(argv=None):
    argv = list(sys.argv[1:] if argv is None else argv)
    if not argv:
        print(__doc__)
        return 0
    cmd = argv[0]
    # Only OUR options are parsed: everything after `--` belongs to the child
    # command, so a build command that itself contains a --label never steals
    # this tool's flag.
    head = argv[:argv.index("--")] if "--" in argv else argv

    def opt(name, default=None):
        return head[head.index(name) + 1] if name in head else default

    if cmd in ("--selftest", "selftest"):
        return selftest()

    if cmd == "status":
        return _cli_status()

    if cmd == "acquire":
        # Take the build and print it. Mostly for scripted/debug use; a caller
        # that acquires this way owns releasing it (by pid, via `release`).
        try:
            path = acquire(label=opt("--label", "cli"),
                           root=opt("--root", None),
                           timeout=float(opt("--timeout", acquire_timeout())))
        except BuildLockTimeout as e:
            print(f"build: TIMEOUT -- {e}", file=sys.stderr)
            return 1
        except BuildLockMisconfigured as e:
            print(f"build: MISCONFIGURED -- {e}", file=sys.stderr)
            return 2
        print(f"build: ACQUIRED (pid {os.getpid()}), lockfile {path}")
        return 0

    if cmd == "release":
        release()
        print("build: released (if this pid held it)")
        return 0

    if cmd == "run":
        return _cli_run(argv, head, opt)

    print(__doc__)
    return 0


if __name__ == "__main__":
    sys.exit(main())
