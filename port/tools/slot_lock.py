#!/usr/bin/env python3
"""A machine-wide exclusive lock for the ONE windowed test slot.

THE PROBLEM THIS EXISTS FOR. walk_window.exe is a windowed test: even run
minimized-and-unactivated (SW_SHOWMINNOACTIVE + SM64DS_NO_FOCUS, as
port/tools/battery.py launches it) it still opens a real OS window and drives
the port's single windowed render/input path. Two of them on the same machine
at the same time -- two agent lanes each running battery.py, or a battery
alongside a hand-launched walk_window -- collide over that one slot and throw a
random rc=1 on a DIFFERENT level or scene each run. Every one of those reds
comes back clean when the run is repeated in isolation. tools/agentlock.py
covers files and address ranges; it does NOT cover exclusive use of the
windowed slot, which is what this module adds.

WHAT IT IS. A single lockfile at a fixed machine-wide path. Whoever holds it
owns the windowed test slot; everyone else waits. It is a plain file plus an
atomic O_CREAT|O_EXCL create, so it needs no server, no docker, and no infra --
unlike agentlock.py, which is Redis-backed. The tradeoff is that it is a
whole-machine mutex over one resource rather than a general lock manager, which
is exactly the resource that was colliding.

OPT-IN. Nothing here changes behaviour unless a caller asks for it. battery.py
takes the lock only when SM64DS_TEST_LOCK is set to a truthy value in the
environment; with it unset the battery launches walk_window exactly as it did
before, byte for byte. A direct/hand launch adopts the lock explicitly, by
wrapping the launch (see "ADOPTING IT" below). So an in-flight run that does
not opt in is never disrupted, and turning the lock on is a per-lane choice.

THE LOCKFILE PATH is machine-wide and NOT repo-relative -- every checkout and
worktree on the box must resolve to the SAME file or the lock does not serialise
across lanes. Resolution order:

  1. $SM64DS_TEST_LOCK_PATH, if set  -- an explicit override. Lanes that run
     with a PRIVATE %TEMP% (see below) MUST export the same value here or they
     will not see each other's lock.
  2. else <system temp>/sm64ds-test-slot/windowed_test.lock, where <system
     temp> is tempfile.gettempdir() (honours %TEMP%/%TMP%).

On this box %TEMP% is pointed at C:\\tmp for every lane (the AppData-Temp exe
block workaround), so (2) resolves to one shared file. BUT because a lane with a
private %TEMP% would silently lock its OWN file and reintroduce the collision as
a flaky red, opting into locking now REQUIRES the explicit path: when
SM64DS_TEST_LOCK is set, acquire() raises SlotLockMisconfigured unless
$SM64DS_TEST_LOCK_PATH is also set. The default in (2) remains for direct/hand
CLI use, where a single human on the box is not racing another lane. acquire()
also prints the resolved path once per process so a wrong path shows in the log.

THE LOCKFILE CONTENTS are JSON: pid, host, an epoch acquire time, a label, and
-- only when the holder declared one -- the max_hold it expects to need. The pid
and time are what the stale-break below reads.

TIMEOUT -- FAIL, NOT PROCEED. A caller that waits DEFAULT_ACQUIRE_TIMEOUT
seconds (SM64DS_TEST_LOCK_TIMEOUT overrides) and still cannot get the slot
raises SlotLockTimeout rather than running anyway. Proceeding would reintroduce
the exact collision this module removes and hand back another mystery red;
failing produces a deterministic, clearly labelled infra message that names the
current holder's pid and how long it has held. The default is sized well above
the longest single windowed run (battery's STEP_TIMEOUT is 600s) so a queue of
a few lanes still clears, and it is a bound rather than an infinite hang.

STALE-LOCK BREAK -- a crashed lane must not wedge the machine. A held lock is
stale, and is broken and re-acquired, when EITHER:
  * the holder pid is dead (the primary, immediate check -- a crashed lane's
    pid is gone the moment it dies), OR
  * the lockfile is older than the bound THAT HOLD DECLARED, which is
    MAX_HOLD_SECONDS unless the holder said otherwise (the backstop: a
    wedged-but-alive holder, or a pid the OS has since reused).
MAX_HOLD_SECONDS is well above any single windowed run, so the backstop never
fires on a legitimately long-held slot; the pid check is what recovers a crash
promptly. A caller that KNOWS it will hold for longer -- a phase hold across a
battery's fifty level rows, or `run --max-hold N` around a long child -- says so
and is not broken, while an UNDECLARED long hold still is, because that is the
shape of a mistake; see _is_stale for the afternoon on this box that produced
one of each within the hour. Breaking is itself raced-safe: the breaker unlinks
and then re-creates with O_EXCL, and if two lanes race to break the same stale
lock, only one wins the create and the other loops back to waiting.

HOLDING IT ACROSS A PHASE. acquire() is deliberately NOT re-entrant -- a second
acquire in one process blocks, which is what makes a lost release show up as a
hang instead of as two runs quietly sharing the slot. A caller that wants to run
several of its OWN windowed children at once takes slot_reentrant() around the
whole phase and lets the inner per-launch locks nest for free; see the phase
banner further down and port/tools/battery.py's SM64DS_BATTERY_WORKERS.

FAIRNESS -- FIRST IN, FIRST SERVED (run link100, lane SLOTQ). Waiters used to
re-poll the same O_CREAT|O_EXCL create, so a release went to whichever process
happened to poll next rather than to the one that had waited longest. Measured
in production: a lane running ipc_proof.py (one slot acquisition per rung) lost
that poll race against other lanes' batteries -- each battery takes and drops
this lock roughly ninety times over its run -- for 100+ minutes and never
completed a single rung (lane WM6 measured it; lane BATTLOCK confirmed the
shape). port/tools/build_lock.py already had the fix for the identical problem
on the full-build lock: a waiter takes an arrival ticket, and the lockfile is
only created by the oldest live ticket. This module ports that fix onto the
windowed slot, unchanged in shape (see port/tools/lock_tickets.py, which both
locks now share) and unchanged in every part of THIS lock that other lanes
depend on tonight:

  * THE LOCKFILE PATH, FORMAT, STALE RULES, PID-LIVENESS RULE AND MAX_HOLD ARE
    IDENTICAL to before this change -- byte for byte. A ticket is a SEPARATE
    file in a sibling directory (<lockfile>.tickets/, exactly where
    build_lock.py's queue sits beside its lockfile); nothing about the lockfile
    itself changed.

  * COMPATIBILITY WITH AN OLD CLIENT ON THE SAME LOCKFILE. Other lanes are
    running TONIGHT with the OLD slot_lock.py (pre-tickets) against this exact
    lockfile (C:/tmp/sm64ds-test-slot/slot.lock). An old client never reads or
    writes a ticket -- it does not know the ticket directory exists -- so it
    still does exactly what it always did: try the O_CREAT|O_EXCL create, and
    if that succeeds, it holds the slot. The consequence, stated plainly, is
    that AN OLD CLIENT CAN BARGE AHEAD OF TICKET HOLDERS: if the lock is free
    and an old client happens to poll it first, it takes the slot even though a
    new client's ticket has been waiting longer. This is accepted as the status
    quo -- a mixed fleet cannot enforce fairness onto a participant that does
    not know the protocol exists, and old clients are not adding new load (they
    already ran this way before tonight) -- and it does not regress anything: a
    lane on the OLD client sees precisely the behaviour it always saw, whether
    or not any tickets exist. What tickets fix is fairness AMONG NEW clients,
    which is where the 100+ minute starvation actually happened.

  * A NEW CLIENT NEVER WAITS FOREVER ON A TICKET AN OLD CLIENT NEVER TOUCHES.
    Only new (ticket-aware) clients ever create a ticket, so every ticket ahead
    of a waiter in the queue is demonstrably owned by another new client, which
    will resolve its own ticket by acquiring (and removing it), by dying (the
    dead-pid rule breaks it), or by timing out (it removes its own ticket on
    the way out, exactly as build_lock.py's waiters do). An old client can only
    ever block a new client by literally holding the plain lockfile -- which
    the ordinary is_stale()/wait loop already handles regardless of whether any
    tickets exist -- never by being "ahead in the queue", because it is never
    IN the queue.

  * slot_reentrant() KEEPS ITS SEMANTICS. The fairness gate lives inside
    acquire(); a nested reentrant hold never calls acquire() at all (see
    `nested` below), so the ticket queue is invisible to a phase that already
    holds the slot, exactly as before.

ADOPTING IT.

  battery.py     -- already wired. Set SM64DS_TEST_LOCK=1 in the environment
                    that launches the battery and every walk_window launch it
                    makes (level, scene, retire probes, default boot, shipcfg
                    run) serialises through this lock. Unset = today's exact
                    behaviour.

  a direct run   -- wrap the launch in the context manager:

                        import slot_lock
                        with slot_lock.slot(label="my hand test"):
                            subprocess.run([".../walk_window.exe", ...])

                    or, from a shell / another language, let the CLI hold the
                    slot for the child's whole lifetime:

                        python port/tools/slot_lock.py run --label "hand test" \\
                            -- build/port/walk_window.exe

                    `run` always takes the lock (invoking it IS the opt-in), so
                    it does not consult SM64DS_TEST_LOCK.
"""

import contextlib
import json
import os
import socket
import sys
import tempfile
import threading
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import lock_tickets

# The longest a single windowed run should ever hold the slot is battery's
# STEP_TIMEOUT (600s). MAX_HOLD is comfortably above that so the mtime backstop
# never breaks a live long run; the pid-dead check is what recovers a crash
# quickly, and this only covers a holder that is alive but wedged, or pid reuse.
MAX_HOLD_SECONDS = 1800

# How long a waiter blocks before giving up. Above the longest single run so a
# short queue of lanes clears; a bound, never an infinite wait. On timeout the
# caller FAILS with the holder named rather than proceeding into a collision.
DEFAULT_ACQUIRE_TIMEOUT = 900

# Poll cadence while waiting for the slot.
POLL_SECONDS = 0.5

# While waiting, say so this often, so a queued lane's log shows a queue rather
# than looking hung for fifteen minutes. Mirrors build_lock.py.
WAIT_NOTICE_SECONDS = 60

# The fairness queue lives beside the lockfile, in a directory named after it:
# windowed_test.lock -> windowed_test.tickets/. See FAIRNESS in the module
# docstring. Slot tickets carry no extra fields beyond pid/host/label/arrived
# (build_lock's tickets additionally carry `root`; a slot waiter has no
# separate worktree concept worth naming).
TICKET_EXTRA_FIELDS = ()


class SlotLockTimeout(TimeoutError):
    """Raised when the windowed slot could not be acquired within the timeout.

    This is an infra condition (another lane held the slot too long), not a
    game fault. The message names the current holder's pid and hold time.
    """


class SlotLockMisconfigured(RuntimeError):
    """SM64DS_TEST_LOCK is on but no explicit SM64DS_TEST_LOCK_PATH is set.

    A lane that opts into locking without an explicit shared path falls back to
    the default under %TEMP%, which does NOT serialise across lanes that run a
    private %TEMP% -- and that failure is SILENT: the lane looks locked, then two
    runs collide and it surfaces as a flaky cross-level red, never as a missing
    lock. The lane that forgets the path is exactly the one that will not notice,
    so refuse loudly at acquire time instead of locking a private file.
    """


# Announce the resolved lockfile path once per process (a log makes a wrong or
# private path visible instead of silent). See acquire().
_path_announced = False


def _truthy(v):
    return str(v).strip().lower() not in ("", "0", "false", "no", "off")


def enabled():
    """Is the opt-in flag set? battery.py gates its automatic locking on this."""
    return _truthy(os.environ.get("SM64DS_TEST_LOCK", ""))


def lock_path():
    """The one machine-wide lockfile path. See the module docstring."""
    override = os.environ.get("SM64DS_TEST_LOCK_PATH")
    if override:
        return override
    return os.path.join(tempfile.gettempdir(), "sm64ds-test-slot",
                        "windowed_test.lock")


def acquire_timeout():
    v = os.environ.get("SM64DS_TEST_LOCK_TIMEOUT")
    if v:
        try:
            return float(v)
        except ValueError:
            pass
    return DEFAULT_ACQUIRE_TIMEOUT


# Shared with build_lock.py via lock_tickets.py (run link100, lane SLOTQ): the
# two had byte-identical copies of the Windows/POSIX liveness probe.
_pid_alive = lock_tickets.pid_alive


def _read_holder(path):
    """(pid, acquired_epoch, raw_text) of the current holder, or (None, None, text)."""
    try:
        with open(path, "r", encoding="utf-8") as f:
            text = f.read()
    except OSError:
        return None, None, ""
    try:
        data = json.loads(text)
        return data.get("pid"), data.get("acquired"), text
    except (ValueError, AttributeError):
        return None, None, text


def _is_stale(path):
    """Is the lock stale -- dead holder, or held past the bound it declared?

    THE BOUND IS THE HOLDER'S TO DECLARE (run link100, lane SLOT). It used to be
    one constant for every hold, and that constant has to be two things at once:
    small enough to recover the box from a lane that wedged while holding, and
    large enough never to fire on a hold that is legitimately long. Both cases
    are real and were seen on this box on the same afternoon -- a lane wedged
    for twenty-six minutes inside `slot_lock.py run -- python ipc_proof.py`,
    holding the slot with no windowed process running at all and 3 seconds of
    CPU to show for it, while a battery holding the slot across its whole level
    phase (the reason that phase can now run several rows wide) is a legitimate
    hold of a similar length.

    So a caller that KNOWS it will be long says so -- acquire(max_hold=...)
    writes the number into the lockfile and _is_stale reads it back. A caller
    that declares nothing keeps MAX_HOLD_SECONDS exactly as before, which is
    what recovers the wedged case promptly: an undeclared long hold is still
    the shape of a mistake, and it is only the declared one that is not.
    """
    pid, acquired, _ = _read_holder(path)
    if not _pid_alive(pid):
        return True
    # mtime is the backstop; also honour the recorded acquire time if present.
    try:
        age = time.time() - os.path.getmtime(path)
    except OSError:
        return True
    if acquired:
        age = max(age, time.time() - float(acquired))
    return age > _declared_max_hold(path)


def _declared_max_hold(path):
    """The bound this lockfile declared, or the default when it declared none."""
    try:
        with open(path, "r", encoding="utf-8") as f:
            v = json.loads(f.read()).get("max_hold")
        if v is not None:
            v = float(v)
            # A declaration can lengthen the leash, never shorten it below the
            # default: a hold that claims it will be over in a second would
            # otherwise be breakable a second later, and the whole point of the
            # default is that it is comfortably above one windowed run.
            return max(v, MAX_HOLD_SECONDS)
    except (OSError, ValueError, AttributeError, TypeError):
        pass
    return MAX_HOLD_SECONDS


def _write_locked(fd, label, max_hold=None):
    payload = {
        "pid": os.getpid(),
        "host": socket.gethostname(),
        "acquired": time.time(),
        "label": label or "",
    }
    # Only written when the caller declared one, so an ordinary lockfile is
    # byte-for-byte the shape it always was and reads the same to every tool.
    if max_hold:
        payload["max_hold"] = float(max_hold)
    os.write(fd, json.dumps(payload).encode("utf-8"))


def _try_create(path, label, max_hold=None):
    """One atomic attempt to create the lockfile. True if we now hold it."""
    os.makedirs(os.path.dirname(path), exist_ok=True)
    try:
        fd = os.open(path, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
    except FileExistsError:
        return False
    try:
        _write_locked(fd, label, max_hold)
    finally:
        os.close(fd)
    return True


# --- the fairness queue (run link100, lane SLOTQ) -------------------------
# Thin wrappers around port/tools/lock_tickets.py, the module build_lock.py's
# identical queue logic was factored into so it is not forked a second time.
# See FAIRNESS in the module docstring for the compatibility argument.

# Say once, not once a second, that this process could not take a queue ticket.
_ticket_warned = False


def tickets_dir(path=None):
    """The queue directory beside the lockfile: windowed_test.lock ->
    windowed_test.tickets/. Derived from the lock's own name, like
    build_lock.py's, so a test lockfile beside the real one never shares a
    queue with it."""
    if path is None:
        path = lock_path()
    return lock_tickets.tickets_dir(path)


def queue(path=None):
    """The wait queue, oldest arrival first: who gets the slot next, in order.

    Read-only, and stale entries are included with their reason in ["stale"]
    so a reader sees what is actually on disk. This is what `status` prints.
    """
    if path is None:
        path = lock_path()
    return lock_tickets.read_tickets(path, extra_fields=TICKET_EXTRA_FIELDS,
                                     cap_seconds=MAX_HOLD_SECONDS)


def _live_queue(path=None):
    """The queue with every stale ticket broken (loudly), oldest first."""
    if path is None:
        path = lock_path()
    return lock_tickets.break_stale_tickets(path, extra_fields=TICKET_EXTRA_FIELDS,
                                            cap_seconds=MAX_HOLD_SECONDS,
                                            tag="slot_lock")


def _write_ticket(path, label, arrival_ns=None):
    """Join the queue. Returns our ticket, or None if it could not be written.

    A queue we cannot write to must never wedge a windowed test, so this
    returns None and acquire() falls back to the old free-for-all rather than
    waiting out a place in line it can never take.
    """
    global _ticket_warned
    ticket, err = lock_tickets.write_ticket(path, label, arrival_ns=arrival_ns)
    if err is not None:
        if not _ticket_warned:
            print(f"[slot_lock] could not take a queue ticket in "
                  f"{tickets_dir(path)} ({err}); waiting UNFAIRLY (first to "
                  f"poll wins) rather than not at all", file=sys.stderr)
            _ticket_warned = True
        return None
    return ticket


def _ensure_ticket(ticket, path, label):
    """Our ticket, written on the first wait and re-created if it goes missing."""
    if ticket is None:
        return _write_ticket(path, label)
    if not os.path.exists(ticket["path"]):
        # Somebody broke it (the age backstop, or a race with a break). Keep
        # our arrival stamp so the break does not cost us our place in line.
        return _write_ticket(path, label, arrival_ns=ticket["arrival_ns"])
    return ticket


def _remove_ticket(ticket):
    """Leave the queue. Idempotent, and safe on a ticket already broken."""
    lock_tickets.remove_ticket(ticket)


def _queue_position(live, ticket):
    """Our 1-based place in the queue, or None if we are not in it."""
    return lock_tickets.queue_position(live, ticket)


def acquire(label="", timeout=None, poll=POLL_SECONDS, max_hold=None):
    """Block until this process holds the windowed slot, or time out.

    Returns the lockfile path on success. Raises SlotLockTimeout if the slot
    could not be had within `timeout` seconds (default from acquire_timeout()).
    A stale lock -- dead holder pid or older than MAX_HOLD_SECONDS -- is broken
    and re-acquired; the break is O_EXCL-raced so only one waiter wins it.

    Waiting is FIRST IN, FIRST SERVED (see FAIRNESS in the module docstring):
    a waiter takes an arrival ticket and the lockfile is only created by the
    oldest live ticket, so a process that arrives while others are queued
    cannot take the slot from under them. With nobody waiting this is the old
    path exactly -- one create attempt, no ticket written, not even a ticket
    directory. An OLD (pre-ticket) client never checks the queue, so it can
    still take the lockfile the moment it is free even with tickets
    outstanding -- accepted, and explained in the module docstring. The ticket
    is removed on the way out of this function however it ends: acquired,
    timed out, or raised through.
    """
    global _path_announced
    # HARD REFUSAL: if a caller opted into locking (SM64DS_TEST_LOCK) but did not
    # pin an explicit shared path, refuse rather than lock a private/default file
    # that silently fails to serialise across lanes. See SlotLockMisconfigured.
    if enabled() and not os.environ.get("SM64DS_TEST_LOCK_PATH"):
        raise SlotLockMisconfigured(
            "SM64DS_TEST_LOCK is set but SM64DS_TEST_LOCK_PATH is not. Locking "
            "without an explicit shared path silently fails to serialise across "
            "lanes that run a private %TEMP%. Export SM64DS_TEST_LOCK_PATH to one "
            "machine-wide file in every lane, e.g. "
            "C:\\tmp\\sm64ds-test-slot\\windowed_test.lock")
    path = lock_path()
    if not _path_announced:
        print(f"[slot_lock] windowed test slot lockfile: {path}", file=sys.stderr)
        _path_announced = True
    if timeout is None:
        timeout = acquire_timeout()
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
            # behind the waiters instead of taking the slot from under them. An
            # OLD client never reaches this gate at all -- it has no concept of
            # it -- which is exactly how it keeps barging when the lock is free;
            # see FAIRNESS in the module docstring.
            live = _live_queue(path)
            head = live[0] if live else None
            our_turn = (head is None or ticketless
                        or (ticket is not None
                            and head["name"] == ticket["name"]))
            if our_turn and _try_create(path, label, max_hold):
                return path
            # Somebody holds it (or it was not our turn to try). Break it if it
            # is stale, otherwise wait -- a dead holder is everyone's problem,
            # so break it even when it is not our turn, so the head of the
            # queue finds a free lock on its next poll.
            if _is_stale(path):
                # Re-check under the same read the unlink relies on: only
                # remove a file that still looks stale, and tolerate another
                # breaker having already removed it. The subsequent
                # _try_create is the real race arbiter -- exactly one O_EXCL
                # create can win.
                try:
                    if _is_stale(path):
                        os.remove(path)
                except OSError:
                    pass
                if our_turn and _try_create(path, label, max_hold):
                    return path
            now = time.time()
            if now >= deadline:
                pid, acquired, _ = _read_holder(path)
                held = ""
                if acquired:
                    try:
                        held = f", held {time.time() - float(acquired):.0f}s"
                    except (TypeError, ValueError):
                        held = ""
                pos = _queue_position(live, ticket)
                place = ""
                if pos is not None:
                    place = (f" You were {pos} of {len(live)} in the queue, "
                             f"which is served in arrival order, so {pos - 1} "
                             f"lane(s) were ahead of you the whole time.")
                raise SlotLockTimeout(
                    f"could not acquire the windowed test slot within "
                    f"{timeout:.0f}s: it is held by pid {pid}{held} "
                    f"(lockfile {path}).{place} This is an infra wait, not a "
                    f"game fault; another lane is running a windowed test. "
                    f"Re-run when the box is idle, or raise "
                    f"SM64DS_TEST_LOCK_TIMEOUT.")
            # Entering (or still in) the wait: hold a place in line.
            ticket = _ensure_ticket(ticket, path, label)
            ticketless = ticket is None
            if now >= next_notice:
                waited = now - started
                pid, acquired, _ = _read_holder(path)
                held = ""
                if acquired:
                    try:
                        held = f", held {time.time() - float(acquired):.0f}s"
                    except (TypeError, ValueError):
                        held = ""
                pos = _queue_position(live, ticket)
                place = f", queue position {pos} of {len(live)}" if pos else ""
                print(f"[slot_lock] waiting {waited:.0f}s for the windowed "
                      f"test slot; held by pid {pid}{held}{place}",
                      file=sys.stderr)
                next_notice = now + WAIT_NOTICE_SECONDS
            time.sleep(poll)
    finally:
        # Every exit path: acquired, timed out, or raised through. A ticket
        # left behind would hold up the queue until its pid was noticed dead.
        _remove_ticket(ticket)


def release(path=None):
    """Release the slot IF this process holds it. Safe to call more than once.

    Only removes the lockfile when its recorded pid is ours, so a lock that was
    broken as stale and re-acquired by another lane is never deleted out from
    under that lane.
    """
    if path is None:
        path = lock_path()
    pid, _, _ = _read_holder(path)
    if pid == os.getpid():
        try:
            os.remove(path)
        except OSError:
            pass


@contextlib.contextmanager
def slot(label="", timeout=None, poll=POLL_SECONDS, max_hold=None):
    """Context manager: hold the windowed slot for the block, release on exit."""
    path = acquire(label=label, timeout=timeout, poll=poll,
                   max_hold=max_hold)
    try:
        yield path
    finally:
        release(path)


# ---- HOLDING THE SLOT ACROSS A WHOLE PHASE (run link100, lane SLOT) --------
#
# WHY THE PER-LAUNCH LOCK IS THE BOTTLENECK. battery.py takes and drops this
# lock around EACH walk_window launch, and a battery makes about ninety of them
# over roughly a quarter of an hour. That serialises two different things at
# once: it keeps two LANES apart, which is the job, and it also keeps a lane
# apart from ITSELF, which is not -- a battery physically cannot run two of its
# own rows side by side while every row wants the same lock. With eight lanes on
# the box the queue is the whole cost of the evening.
#
# So a caller that wants several of its own children in flight has to hold the
# slot across the PHASE and launch inside it. acquire() cannot simply become
# re-entrant to allow that: test_acquire_blocks_a_second_acquire pins the
# opposite contract on purpose -- a second acquire in the same process BLOCKS,
# which is what makes a lost release show up as a hang instead of as two runs
# quietly sharing a slot. Re-entrancy is therefore its own function, opted into
# by name at the call site, and acquire()/release() are byte-for-byte what they
# were.
#
# held() is the predicate the nesting is built on and is worth having alone: it
# answers "is the lockfile mine right now" from the file itself rather than from
# memory, so a lock broken as stale underneath us reads as NOT held and the next
# nested call goes and takes it properly.
# The depth is mutated from WORKER THREADS -- the whole point is that a phase
# hold on the main thread lets several launcher threads run at once -- so it is
# guarded. `n += 1` on a module global is not atomic, and a drifted counter here
# would release the slot while a lane still had children in it.
_nest_depth = 0
_nest_guard = threading.Lock()


def held(path=None):
    """Does THIS process hold the slot right now, per the lockfile itself?"""
    if path is None:
        path = lock_path()
    pid, _, _ = _read_holder(path)
    return pid == os.getpid()


@contextlib.contextmanager
def slot_reentrant(label="", timeout=None, poll=POLL_SECONDS, max_hold=None):
    """slot(), except a nested use by a process that already holds it is free.

    The outermost use acquires and releases exactly as slot() does; an inner use
    inside it neither touches the lockfile nor waits, and its exit does not
    release. Other lanes see one O_EXCL lockfile for the whole nested span and
    are shut out for all of it, which is the property the lock exists for.
    """
    global _nest_depth
    path = lock_path()
    with _nest_guard:
        nested = _nest_depth > 0 and held(path)
        if nested:
            _nest_depth += 1
    if nested:
        try:
            yield path
        finally:
            with _nest_guard:
                _nest_depth -= 1
        return
    path = acquire(label=label, timeout=timeout, poll=poll,
                   max_hold=max_hold)
    with _nest_guard:
        _nest_depth += 1
    try:
        yield path
    finally:
        with _nest_guard:
            _nest_depth -= 1
        release(path)


def _cli_status():
    path = lock_path()
    if not os.path.exists(path):
        print(f"slot: FREE ({path} does not exist)")
        _print_queue(path)
        return 0
    pid, acquired, text = _read_holder(path)
    alive = _pid_alive(pid)
    stale = _is_stale(path)
    held = ""
    if acquired:
        try:
            held = f", held {time.time() - float(acquired):.0f}s"
        except (TypeError, ValueError):
            pass
    print(f"slot: HELD by pid {pid} (alive={alive}, stale={stale}{held})")
    print(f"  lockfile: {path}")
    print(f"  contents: {text.strip()}")
    _print_queue(path)
    return 0


def _print_queue(path):
    """The wait queue after the holder, oldest arrival first: who is next.

    Read-only on purpose -- `status` marks a stale ticket rather than breaking
    it, so looking at the queue never changes who gets the slot next. Mirrors
    build_lock.py's `_print_queue`.
    """
    q = queue(path)
    if not q:
        print("  queue: empty (nobody waiting)")
        return
    print(f"  queue: {len(q)} waiting, oldest arrival first "
          f"(first in, first served)")
    for i, t in enumerate(q, 1):
        stale = f"  STALE: {t['stale']}" if t["stale"] else ""
        desc = lock_tickets.describe_ticket(t, extra_fields=TICKET_EXTRA_FIELDS)
        print(f"    {i}. {desc}{stale}")
    print(f"  tickets: {tickets_dir(path)}")


def main(argv=None):
    argv = list(sys.argv[1:] if argv is None else argv)
    if not argv:
        print(__doc__)
        return 0
    cmd = argv[0]

    def opt(name, default=None):
        return argv[argv.index(name) + 1] if name in argv else default

    if cmd == "status":
        return _cli_status()

    if cmd == "acquire":
        # Take the slot and print it. Mostly for scripted/debug use; a caller
        # that acquires this way owns releasing it (by pid, via `release`).
        try:
            path = acquire(label=opt("--label", "cli"),
                          timeout=float(opt("--timeout", acquire_timeout())))
        except SlotLockTimeout as e:
            print(f"slot: TIMEOUT -- {e}", file=sys.stderr)
            return 1
        print(f"slot: ACQUIRED (pid {os.getpid()}), lockfile {path}")
        return 0

    if cmd == "release":
        release()
        print("slot: released (if this pid held it)")
        return 0

    if cmd == "run":
        # Hold the slot for the whole lifetime of a child command.
        if "--" not in argv:
            print("usage: slot_lock.py run [--label L] [--timeout N] "
                  "[--max-hold SECONDS] -- <cmd...>",
                  file=sys.stderr)
            return 2
        child = argv[argv.index("--") + 1:]
        if not child:
            print("slot_lock.py run: no command after --", file=sys.stderr)
            return 2
        import subprocess
        try:
            # --max-hold DECLARES A LONG HOLD. `run` wraps a whole child
            # command, and a child that is a proof script or a battery can
            # legitimately run past the default stale bound -- at which point a
            # waiter breaks the lock and starts a windowed run beside one that
            # is still going. Say the number and it will not. Undeclared, the
            # default still applies, which is what recovers a wedged holder.
            with slot(label=opt("--label", "cli run"),
                      timeout=float(opt("--timeout", acquire_timeout())),
                      max_hold=float(opt("--max-hold", 0)) or None):
                return subprocess.run(child).returncode
        except SlotLockTimeout as e:
            print(f"slot: TIMEOUT -- {e}", file=sys.stderr)
            return 1

    print(__doc__)
    return 0


if __name__ == "__main__":
    sys.exit(main())
