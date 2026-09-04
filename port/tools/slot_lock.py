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

THE LOCKFILE CONTENTS are JSON: pid, host, an epoch acquire time, and a label.
The pid and time are what the stale-break below reads.

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
  * the lockfile is older than MAX_HOLD_SECONDS (the backstop: a wedged-but-
    alive holder, or a pid that the OS has since reused for something else).
MAX_HOLD_SECONDS is well above any single windowed run, so the backstop never
fires on a legitimately long-held slot; the pid check is what recovers a crash
promptly. Breaking is itself raced-safe: the breaker unlinks and then re-creates
with O_EXCL, and if two lanes race to break the same stale lock, only one wins
the create and the other loops back to waiting.

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
import time

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
    """Is the lock currently on disk stale (dead holder OR older than MAX_HOLD)?"""
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
    return age > MAX_HOLD_SECONDS


def _write_locked(fd, label):
    payload = json.dumps({
        "pid": os.getpid(),
        "host": socket.gethostname(),
        "acquired": time.time(),
        "label": label or "",
    })
    os.write(fd, payload.encode("utf-8"))


def _try_create(path, label):
    """One atomic attempt to create the lockfile. True if we now hold it."""
    os.makedirs(os.path.dirname(path), exist_ok=True)
    try:
        fd = os.open(path, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
    except FileExistsError:
        return False
    try:
        _write_locked(fd, label)
    finally:
        os.close(fd)
    return True


def acquire(label="", timeout=None, poll=POLL_SECONDS):
    """Block until this process holds the windowed slot, or time out.

    Returns the lockfile path on success. Raises SlotLockTimeout if the slot
    could not be had within `timeout` seconds (default from acquire_timeout()).
    A stale lock -- dead holder pid or older than MAX_HOLD_SECONDS -- is broken
    and re-acquired; the break is O_EXCL-raced so only one waiter wins it.
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
    deadline = time.time() + timeout
    while True:
        if _try_create(path, label):
            return path
        # Somebody holds it. Break it if it is stale, otherwise wait.
        if _is_stale(path):
            # Re-check under the same read the unlink relies on: only remove a
            # file that still looks stale, and tolerate another breaker having
            # already removed it. The subsequent _try_create is the real race
            # arbiter -- exactly one O_EXCL create can win.
            try:
                if _is_stale(path):
                    os.remove(path)
            except OSError:
                pass
            if _try_create(path, label):
                return path
        if time.time() >= deadline:
            pid, acquired, _ = _read_holder(path)
            held = ""
            if acquired:
                try:
                    held = f", held {time.time() - float(acquired):.0f}s"
                except (TypeError, ValueError):
                    held = ""
            raise SlotLockTimeout(
                f"could not acquire the windowed test slot within "
                f"{timeout:.0f}s: it is held by pid {pid}{held} "
                f"(lockfile {path}). This is an infra wait, not a game fault; "
                f"another lane is running a windowed test. Re-run when the box "
                f"is idle, or raise SM64DS_TEST_LOCK_TIMEOUT.")
        time.sleep(poll)


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
def slot(label="", timeout=None, poll=POLL_SECONDS):
    """Context manager: hold the windowed slot for the block, release on exit."""
    path = acquire(label=label, timeout=timeout, poll=poll)
    try:
        yield path
    finally:
        release(path)


def _cli_status():
    path = lock_path()
    if not os.path.exists(path):
        print(f"slot: FREE ({path} does not exist)")
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
    return 0


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
            print("usage: slot_lock.py run [--label L] [--timeout N] -- <cmd...>",
                  file=sys.stderr)
            return 2
        child = argv[argv.index("--") + 1:]
        if not child:
            print("slot_lock.py run: no command after --", file=sys.stderr)
            return 2
        import subprocess
        try:
            with slot(label=opt("--label", "cli run"),
                      timeout=float(opt("--timeout", acquire_timeout()))):
                return subprocess.run(child).returncode
        except SlotLockTimeout as e:
            print(f"slot: TIMEOUT -- {e}", file=sys.stderr)
            return 1

    print(__doc__)
    return 0


if __name__ == "__main__":
    sys.exit(main())
