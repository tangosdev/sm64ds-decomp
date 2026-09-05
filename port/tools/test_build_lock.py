#!/usr/bin/env python3
"""Unit tests for the full-build lock (port/tools/build_lock.py).

These test the LOCK MECHANISM directly and need no ROM, no compiler and no
build: acquire blocks a second acquire, release frees it, the timeout fires and
names the holder, and a stale lock (dead holder pid, or a lockfile older than
the max-hold cap) is detected and broken with a loud line. Run with:
pytest port/tools/test_build_lock.py

The sibling of port/tools/test_slot_lock.py, which does the same for the one
windowed test slot; the two locks are independent (different lockfiles) so a
lane compiling never blocks a lane running a windowed test.
"""
import io
import json
import os
import subprocess
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import build_lock

# Runnable with pytest (the repo convention for tools/test_*.py) OR, where
# pytest is not installed, as a plain script -- `python port/tools/test_build_lock.py`
# -- via the tiny fixture/assert shim at the bottom of this file. Both paths run
# the identical test bodies.
try:
    import pytest
    _HAVE_PYTEST = True
except ImportError:
    _HAVE_PYTEST = False

    class _Shim:
        """A pytest.fixture / pytest.raises stand-in for the no-pytest runner."""
        @staticmethod
        def fixture(fn):
            fn._is_fixture = True
            return fn

        class raises:
            def __init__(self, exc):
                self.exc = exc
                self.value = None

            def __enter__(self):
                return self

            def __exit__(self, et, ev, tb):
                assert et is not None, f"expected {self.exc.__name__}, none raised"
                assert issubclass(et, self.exc), \
                    f"expected {self.exc.__name__}, got {et.__name__}"
                self.value = ev
                return True

    pytest = _Shim()


@pytest.fixture
def lockfile(tmp_path, monkeypatch):
    """Point the lock at a private per-test file so tests never touch the real
    machine-wide build lock and never collide with each other."""
    path = tmp_path / "port_build.lock"
    monkeypatch.setenv("SM64DS_BUILD_LOCK_PATH", str(path))
    # A clean opt-in default; individual tests override as needed.
    monkeypatch.delenv("SM64DS_BUILD_LOCK", raising=False)
    monkeypatch.delenv("SM64DS_BUILD_LOCK_TIMEOUT", raising=False)
    monkeypatch.delenv("SM64DS_BUILD_LOCK_MAX_HOLD", raising=False)
    yield str(path)
    # release() only removes a file this pid owns; clean up any test-written one.
    if os.path.exists(path):
        os.remove(path)


def _write_lock(path, pid, acquired=None, label="planted", root="C:/tmp/planted"):
    with open(path, "w", encoding="utf-8") as f:
        json.dump({"pid": pid, "host": "test",
                   "acquired": acquired if acquired is not None else time.time(),
                   "label": label, "root": root}, f)


# --- enabled(): the opt-in gate ------------------------------------------

def test_enabled_reads_the_optin_flag(lockfile, monkeypatch):
    monkeypatch.delenv("SM64DS_BUILD_LOCK", raising=False)
    assert build_lock.enabled() is False
    for on in ("1", "true", "YES", "on"):
        monkeypatch.setenv("SM64DS_BUILD_LOCK", on)
        assert build_lock.enabled() is True
    for off in ("0", "false", "no", "off", ""):
        monkeypatch.setenv("SM64DS_BUILD_LOCK", off)
        assert build_lock.enabled() is False


def test_lock_path_honours_override(lockfile):
    assert build_lock.lock_path() == lockfile


def test_default_lock_path_sits_beside_the_slot_lock(lockfile, monkeypatch):
    # The two machine-wide locks share a directory on purpose (one place to
    # look) but are DIFFERENT files, so a build never blocks a windowed test.
    monkeypatch.delenv("SM64DS_BUILD_LOCK_PATH", raising=False)
    path = build_lock.lock_path()
    assert os.path.basename(path) == "port_build.lock"
    assert os.path.basename(os.path.dirname(path)) == "sm64ds-test-slot"


def test_enabled_without_explicit_path_is_refused(lockfile, monkeypatch):
    # Opting into locking (SM64DS_BUILD_LOCK) without an explicit shared path
    # must FAIL LOUDLY, not silently lock a private/default file. This is the
    # footgun: a lane that forgets the path is the one that will not notice
    # four builds running at once.
    monkeypatch.setenv("SM64DS_BUILD_LOCK", "1")
    monkeypatch.delenv("SM64DS_BUILD_LOCK_PATH", raising=False)
    with pytest.raises(build_lock.BuildLockMisconfigured):
        build_lock.acquire(timeout=1)


def test_enabled_with_explicit_path_is_allowed(lockfile, monkeypatch):
    # The same opt-in WITH an explicit path (the fixture set one) is fine.
    monkeypatch.setenv("SM64DS_BUILD_LOCK", "1")
    path = build_lock.acquire(label="ok", timeout=1)
    assert os.path.exists(path)
    build_lock.release(path)


# --- the tunables --------------------------------------------------------

def test_timeout_and_cap_defaults_and_overrides(lockfile, monkeypatch):
    assert build_lock.acquire_timeout() == 3600      # 60 minutes
    assert build_lock.max_hold_seconds() == 5400     # 90 minutes
    monkeypatch.setenv("SM64DS_BUILD_LOCK_TIMEOUT", "12")
    monkeypatch.setenv("SM64DS_BUILD_LOCK_MAX_HOLD", "34")
    assert build_lock.acquire_timeout() == 12.0
    assert build_lock.max_hold_seconds() == 34.0
    # Garbage falls back to the default rather than crashing a build wrapper.
    monkeypatch.setenv("SM64DS_BUILD_LOCK_TIMEOUT", "soon")
    assert build_lock.acquire_timeout() == 3600


# --- acquire / release basics --------------------------------------------

def test_acquire_records_pid_label_and_root(lockfile):
    path = build_lock.acquire(label="w7", root="C:/tmp/w7", timeout=2)
    assert os.path.exists(path)
    pid, acquired, label, root, _ = build_lock._read_holder(path)
    assert pid == os.getpid()
    assert label == "w7"
    assert root == "C:/tmp/w7"       # so `status` can name the worktree
    assert acquired is not None
    build_lock.release(path)


def test_default_root_is_the_worktree(lockfile):
    # <root>/port/tools/build_lock.py -> <root>; a lane that does not pass
    # --root still records something that identifies its checkout.
    path = build_lock.acquire(label="t", timeout=2)
    _, _, _, root, _ = build_lock._read_holder(path)
    assert os.path.isdir(os.path.join(root, "port", "tools"))
    build_lock.release(path)


def test_acquire_blocks_a_second_acquire(lockfile):
    # First acquire holds the build; the holder pid is this live process, so the
    # lock is NOT stale and a second acquire cannot get in -- it times out.
    build_lock.acquire(label="first", timeout=2)
    t0 = time.time()
    with pytest.raises(build_lock.BuildLockTimeout):
        build_lock.acquire(label="second", timeout=0.5, poll=0.05)
    assert time.time() - t0 >= 0.4  # it actually waited, did not fall through
    build_lock.release(lockfile)


def test_release_frees_the_lock(lockfile):
    build_lock.acquire(label="first", timeout=2)
    build_lock.release(lockfile)
    assert not os.path.exists(lockfile)
    # Now a fresh acquire succeeds immediately.
    path = build_lock.acquire(label="second", timeout=1)
    assert os.path.exists(path)
    build_lock.release(path)


def test_release_only_removes_our_own_lock(lockfile):
    # A lock held by another (live) pid must NOT be removed by our release --
    # otherwise a lane could delete a lock another lane legitimately holds and
    # put two builds on the box.
    sleeper = subprocess.Popen([sys.executable, "-c", "import time; time.sleep(30)"])
    try:
        _write_lock(lockfile, sleeper.pid)
        build_lock.release(lockfile)
        assert os.path.exists(lockfile), "release deleted a foreign live lock"
    finally:
        sleeper.terminate()
        sleeper.wait()


def test_timeout_message_names_the_holder(lockfile):
    build_lock.acquire(label="holder-lane", root="C:/tmp/holder", timeout=2)
    with pytest.raises(build_lock.BuildLockTimeout) as ei:
        build_lock.acquire(timeout=0.3, poll=0.05)
    msg = str(ei.value)
    assert str(os.getpid()) in msg
    assert "holder-lane" in msg and "C:/tmp/holder" in msg
    assert "full port build" in msg
    build_lock.release(lockfile)


# --- stale-lock break ----------------------------------------------------

def test_stale_lock_dead_pid_is_broken_loudly(lockfile):
    # A pid that cannot exist -> holder is dead -> stale -> broken and acquired,
    # with a line on stderr naming what was broken.
    import contextlib
    _write_lock(lockfile, 0x7FFFFFFF, label="crashed-lane")
    assert build_lock._is_stale(lockfile) is True
    err = io.StringIO()
    with contextlib.redirect_stderr(err):
        path = build_lock.acquire(label="breaker", timeout=1)
    pid, _, _, _, _ = build_lock._read_holder(path)
    assert pid == os.getpid()  # we took it over
    assert "BROKE STALE LOCK" in err.getvalue()
    assert "crashed-lane" in err.getvalue()
    build_lock.release(path)


def test_stale_lock_past_the_cap_is_broken(lockfile, monkeypatch):
    # A holder that is alive (this very process) but whose lockfile is older
    # than the cap is stale via the age backstop, and is broken.
    monkeypatch.setenv("SM64DS_BUILD_LOCK_MAX_HOLD", "5")
    old = time.time() - 60
    _write_lock(lockfile, os.getpid(), acquired=old, label="wedged")
    os.utime(lockfile, (old, old))
    reason = build_lock._stale_reason(lockfile)
    assert reason is not None and "cap" in reason
    path = build_lock.acquire(label="breaker", timeout=1)
    assert os.path.exists(path)
    build_lock.release(path)


def test_a_long_but_healthy_hold_is_not_stale(lockfile):
    # 20 minutes into a build is nowhere near the 90-minute cap: the backstop
    # must not break a holder that is simply slow.
    sleeper = subprocess.Popen([sys.executable, "-c", "import time; time.sleep(30)"])
    try:
        old = time.time() - 20 * 60
        _write_lock(lockfile, sleeper.pid, acquired=old)
        os.utime(lockfile, (old, old))
        assert build_lock._stale_reason(lockfile) is None
    finally:
        sleeper.terminate()
        sleeper.wait()


def test_live_foreign_holder_is_not_stale_and_blocks(lockfile):
    # A lock held by a real, alive, recent foreign process is NOT stale, so a
    # waiter must block on it (and time out), never break it.
    sleeper = subprocess.Popen([sys.executable, "-c", "import time; time.sleep(30)"])
    try:
        _write_lock(lockfile, sleeper.pid)
        assert build_lock._is_stale(lockfile) is False
        with pytest.raises(build_lock.BuildLockTimeout):
            build_lock.acquire(timeout=0.4, poll=0.05)
        # the foreign lock is untouched
        pid, _, _, _, _ = build_lock._read_holder(lockfile)
        assert pid == sleeper.pid
    finally:
        sleeper.terminate()
        sleeper.wait()


def test_dead_foreign_holder_becomes_stale_after_a_kill(lockfile):
    # End to end: a live holder blocks, then its build is killed (we kill it)
    # and the lock becomes stale and is broken -- a killed build must never
    # wedge the box for the rest of the night.
    sleeper = subprocess.Popen([sys.executable, "-c", "import time; time.sleep(30)"])
    _write_lock(lockfile, sleeper.pid)
    assert build_lock._is_stale(lockfile) is False
    sleeper.terminate()
    sleeper.wait()
    assert build_lock._is_stale(lockfile) is True
    path = build_lock.acquire(label="after-kill", timeout=1)
    pid, _, _, _, _ = build_lock._read_holder(path)
    assert pid == os.getpid()
    build_lock.release(path)


def test_unparseable_lockfile_is_broken(lockfile):
    # A half-written or truncated lockfile names nobody; treat it as dead-held
    # rather than blocking every build on the box for an hour.
    with open(lockfile, "w", encoding="utf-8") as f:
        f.write("{not json")
    assert build_lock._is_stale(lockfile) is True
    path = build_lock.acquire(label="breaker", timeout=1)
    pid, _, _, _, _ = build_lock._read_holder(path)
    assert pid == os.getpid()
    build_lock.release(path)


# --- context manager -----------------------------------------------------

def test_build_context_manager_acquires_and_releases(lockfile):
    with build_lock.build(label="ctx", root="C:/tmp/ctx", timeout=2) as path:
        assert os.path.exists(path)
        pid, _, _, root, _ = build_lock._read_holder(path)
        assert pid == os.getpid()
        assert root == "C:/tmp/ctx"
    assert not os.path.exists(lockfile)  # released on exit


def test_build_context_manager_releases_on_exception(lockfile):
    with pytest.raises(ValueError):
        with build_lock.build(label="ctx", timeout=2):
            raise ValueError("boom")
    assert not os.path.exists(lockfile)


# --- the CLI: two real processes contend ---------------------------------

def test_cli_run_holds_for_the_children_lifetime(lockfile):
    # The shape lanes actually use: `run` takes the lock, holds it for the whole
    # child command, and releases when the child exits. While it holds, this
    # process cannot get in; once it exits, this process can.
    me = os.path.join(os.path.dirname(os.path.abspath(__file__)), "build_lock.py")
    env = dict(os.environ)
    env["SM64DS_BUILD_LOCK_PATH"] = lockfile
    holder = subprocess.Popen(
        [sys.executable, me, "run", "--label", "cli-holder",
         "--root", "C:/tmp/holder", "--timeout", "30", "--",
         sys.executable, "-c", "import time; time.sleep(5)"],
        env=env, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    try:
        waited = 0.0
        while not os.path.exists(lockfile) and waited < 20:
            time.sleep(0.1)
            waited += 0.1
        assert os.path.exists(lockfile), "the child never took the lock"
        pid, _, label, root, _ = build_lock._read_holder(lockfile)
        assert pid == holder.pid and label == "cli-holder"
        assert root == "C:/tmp/holder"
        # blocked while the child runs
        with pytest.raises(build_lock.BuildLockTimeout):
            build_lock.acquire(label="loser", timeout=0.5, poll=0.05)
        # and it waits, then proceeds once the child exits
        t0 = time.time()
        path = build_lock.acquire(label="second", timeout=45, poll=0.1)
        assert time.time() - t0 >= 0.5
        assert build_lock._read_holder(path)[0] == os.getpid()
        assert holder.wait(timeout=20) == 0
        build_lock.release(path)
    finally:
        if holder.poll() is None:
            holder.terminate()
            holder.wait()


def test_cli_run_if_enabled_is_a_no_op_when_the_flag_is_off(lockfile, monkeypatch):
    # The one-line adoption in build-port.cmd must be inert with the flag unset:
    # the child still runs, and no lockfile is created at all.
    monkeypatch.delenv("SM64DS_BUILD_LOCK", raising=False)
    me = os.path.join(os.path.dirname(os.path.abspath(__file__)), "build_lock.py")
    env = dict(os.environ)
    env["SM64DS_BUILD_LOCK_PATH"] = lockfile
    env.pop("SM64DS_BUILD_LOCK", None)
    rc = subprocess.call([sys.executable, me, "run", "--if-enabled",
                          "--label", "unlocked", "--",
                          sys.executable, "-c", "pass"], env=env)
    assert rc == 0
    assert not os.path.exists(lockfile)


def test_cli_status_free_and_held(lockfile):
    # status reads FREE with no lockfile, and names the holder with one.
    import contextlib
    out = io.StringIO()
    with contextlib.redirect_stdout(out):
        assert build_lock.main(["status"]) == 0
    assert "FREE" in out.getvalue()
    path = build_lock.acquire(label="statused", root="C:/tmp/statused", timeout=2)
    out = io.StringIO()
    with contextlib.redirect_stdout(out):
        assert build_lock.main(["status"]) == 0
    text = out.getvalue()
    assert "HELD" in text and str(os.getpid()) in text
    assert "statused" in text and "C:/tmp/statused" in text
    build_lock.release(path)


def test_cli_options_after_the_separator_are_the_childs(lockfile):
    # A build command that itself contains --label must not steal ours.
    argv = ["run", "--label", "ours", "--", "cmd", "/c", "x.cmd", "--label", "theirs"]
    head = argv[:argv.index("--")]
    assert head[head.index("--label") + 1] == "ours"


# --- pid liveness --------------------------------------------------------

def test_pid_alive_true_for_self_false_for_dead(lockfile):
    assert build_lock._pid_alive(os.getpid()) is True
    assert build_lock._pid_alive(0x7FFFFFFF) is False
    assert build_lock._pid_alive(None) is False
    assert build_lock._pid_alive(-1) is False


# --- no-pytest standalone runner -----------------------------------------
# When pytest is not installed this drives the same test bodies with a minimal
# tmp_path + monkeypatch, so the lock logic can be proven anywhere Python runs.

def _standalone():
    import pathlib
    import tempfile
    import traceback

    class _Monkeypatch:
        def __init__(self):
            self._env = []

        def setenv(self, k, v):
            self._env.append((k, os.environ.get(k)))
            os.environ[k] = v

        def delenv(self, k, raising=True):
            self._env.append((k, os.environ.get(k)))
            os.environ.pop(k, None)

        def undo(self):
            for k, old in reversed(self._env):
                if old is None:
                    os.environ.pop(k, None)
                else:
                    os.environ[k] = old

    import inspect
    tests = [(n, o) for n, o in sorted(globals().items())
             if n.startswith("test_") and callable(o)]
    passed = failed = 0
    for name, fn in tests:
        with tempfile.TemporaryDirectory() as td:
            mp = _Monkeypatch()
            it = lockfile(pathlib.Path(td), mp)
            lf = next(it)
            params = inspect.signature(fn).parameters
            kwargs = {}
            if "lockfile" in params:
                kwargs["lockfile"] = lf
            if "monkeypatch" in params:
                kwargs["monkeypatch"] = mp
            try:
                fn(**kwargs)
                # drain fixture teardown
                for _ in it:
                    pass
                print(f"  PASS {name}")
                passed += 1
            except Exception:
                print(f"  FAIL {name}")
                traceback.print_exc()
                failed += 1
            finally:
                mp.undo()
    print(f"\n{passed} passed, {failed} failed")
    return 1 if failed else 0


if __name__ == "__main__":
    if _HAVE_PYTEST:
        sys.exit(pytest.main([__file__, "-q"]))
    print("pytest not installed -- running the standalone lock-logic harness:\n")
    sys.exit(_standalone())
