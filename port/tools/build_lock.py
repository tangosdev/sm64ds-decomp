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
holder, and both stale rules break a lock. port/tools/test_build_lock.py is the
same coverage as unit tests (pytest, or standalone where pytest is absent).
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


def acquire(label="", root=None, timeout=None, poll=POLL_SECONDS):
    """Block until this process holds the full-build lock, or time out.

    Returns the lockfile path on success. Raises BuildLockTimeout if the build
    could not be had within `timeout` seconds (default from acquire_timeout()),
    naming the holder -- it never returns without the lock. A stale lock (dead
    holder pid, or older than max_hold_seconds()) is broken and re-acquired,
    loudly; the break is O_EXCL-raced so only one waiter wins it.
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
    while True:
        if _try_create(path, label, root):
            return path
        # Somebody holds it. Break it if it is stale, otherwise wait.
        if _break_stale(path) and _try_create(path, label, root):
            return path
        now = time.time()
        if now >= deadline:
            raise BuildLockTimeout(
                f"could not acquire the full port build within {timeout:.0f}s: "
                f"it is held by {_describe_holder(path)} (lockfile {path}). "
                f"This is an infra wait, not a build fault; another lane is "
                f"building. Re-run when the box is idle, or raise "
                f"SM64DS_BUILD_LOCK_TIMEOUT. Building anyway is what this lock "
                f"exists to prevent: concurrent builds take 70+ minutes and get "
                f"their cl.exe killed under memory pressure.")
        if now >= next_notice:
            waited = now - started
            print(f"[build_lock] waiting {waited:.0f}s for the full build; "
                  f"held by {_describe_holder(path)}", file=sys.stderr)
            next_notice = now + WAIT_NOTICE_SECONDS
        time.sleep(poll)


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

def _cli_status():
    path = lock_path()
    if not os.path.exists(path):
        print(f"build: FREE ({path} does not exist)")
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

def selftest():
    """The lock mechanism against real processes: no ROM, compiler or build.

    Covers, in order: a free lock reads FREE; acquire records our pid, label and
    root; TWO PROCESSES CONTEND -- a child holds the lock through `run` while
    this process (a) is refused by a short timeout whose message names the
    holder's pid, label and root, and (b) then waits on a long timeout and
    proceeds only after the child exits; both STALE rules break a lock (a dead
    holder pid, and an age past the cap) with a loud line; release never removes
    a foreign live lock; the opt-in flag without an explicit path is refused;
    and the context manager releases on an exception.
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
