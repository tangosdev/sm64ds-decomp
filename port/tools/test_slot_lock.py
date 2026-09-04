#!/usr/bin/env python3
"""Unit tests for the windowed test slot lock (port/tools/slot_lock.py).

These test the LOCK MECHANISM directly and need no ROM, no compiler and no
walk_window.exe: acquire blocks a second acquire, release frees it, the timeout
fires, and a stale lock (dead holder pid, or a lockfile older than the max-hold
bound) is detected and broken. Run with: pytest port/tools/test_slot_lock.py
"""
import json
import os
import subprocess
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import slot_lock

# Runnable with pytest (the repo convention for tools/test_*.py) OR, where
# pytest is not installed, as a plain script -- `python port/tools/test_slot_lock.py`
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
    machine-wide slot and never collide with each other."""
    path = tmp_path / "windowed_test.lock"
    monkeypatch.setenv("SM64DS_TEST_LOCK_PATH", str(path))
    # A clean opt-in default; individual tests override as needed.
    monkeypatch.delenv("SM64DS_TEST_LOCK", raising=False)
    monkeypatch.delenv("SM64DS_TEST_LOCK_TIMEOUT", raising=False)
    yield str(path)
    # release() only removes a file this pid owns; clean up any test-written one.
    if os.path.exists(path):
        os.remove(path)


def _write_lock(path, pid, acquired=None, label="planted"):
    with open(path, "w", encoding="utf-8") as f:
        json.dump({"pid": pid, "host": "test",
                   "acquired": acquired if acquired is not None else time.time(),
                   "label": label}, f)


# --- enabled(): the opt-in gate ------------------------------------------

def test_enabled_reads_the_optin_flag(lockfile, monkeypatch):
    monkeypatch.delenv("SM64DS_TEST_LOCK", raising=False)
    assert slot_lock.enabled() is False
    for on in ("1", "true", "YES", "on"):
        monkeypatch.setenv("SM64DS_TEST_LOCK", on)
        assert slot_lock.enabled() is True
    for off in ("0", "false", "no", "off", ""):
        monkeypatch.setenv("SM64DS_TEST_LOCK", off)
        assert slot_lock.enabled() is False


def test_lock_path_honours_override(lockfile):
    assert slot_lock.lock_path() == lockfile


def test_enabled_without_explicit_path_is_refused(lockfile, monkeypatch):
    # Opting into locking (SM64DS_TEST_LOCK) without an explicit shared path
    # must FAIL LOUDLY, not silently lock a private/default file. This is the
    # footgun: a lane that forgets the path is the one that will not notice a
    # silent non-serialisation.
    monkeypatch.setenv("SM64DS_TEST_LOCK", "1")
    monkeypatch.delenv("SM64DS_TEST_LOCK_PATH", raising=False)
    with pytest.raises(slot_lock.SlotLockMisconfigured):
        slot_lock.acquire(timeout=1)


def test_enabled_with_explicit_path_is_allowed(lockfile, monkeypatch):
    # The same opt-in WITH an explicit path (the fixture set one) is fine.
    monkeypatch.setenv("SM64DS_TEST_LOCK", "1")
    path = slot_lock.acquire(label="ok", timeout=1)
    assert os.path.exists(path)
    slot_lock.release(path)


# --- acquire / release basics --------------------------------------------

def test_acquire_creates_lockfile_with_our_pid(lockfile):
    path = slot_lock.acquire(label="t", timeout=2)
    assert os.path.exists(path)
    pid, acquired, _ = slot_lock._read_holder(path)
    assert pid == os.getpid()
    assert acquired is not None
    slot_lock.release(path)


def test_acquire_blocks_a_second_acquire(lockfile):
    # First acquire holds the slot; the holder pid is this live process, so the
    # lock is NOT stale and a second acquire cannot get in -- it times out.
    slot_lock.acquire(label="first", timeout=2)
    t0 = time.time()
    with pytest.raises(slot_lock.SlotLockTimeout):
        slot_lock.acquire(label="second", timeout=0.5, poll=0.05)
    assert time.time() - t0 >= 0.4  # it actually waited, did not fall through
    slot_lock.release(lockfile)


def test_release_frees_the_slot(lockfile):
    slot_lock.acquire(label="first", timeout=2)
    slot_lock.release(lockfile)
    assert not os.path.exists(lockfile)
    # Now a fresh acquire succeeds immediately.
    path = slot_lock.acquire(label="second", timeout=1)
    assert os.path.exists(path)
    slot_lock.release(path)


def test_release_only_removes_our_own_lock(lockfile):
    # A lock held by another (live) pid must NOT be removed by our release --
    # otherwise a lane could delete a slot another lane legitimately holds.
    sleeper = subprocess.Popen([sys.executable, "-c", "import time; time.sleep(30)"])
    try:
        _write_lock(lockfile, sleeper.pid)
        slot_lock.release(lockfile)
        assert os.path.exists(lockfile), "release deleted a foreign live lock"
    finally:
        sleeper.terminate()
        sleeper.wait()


def test_timeout_message_names_the_holder(lockfile):
    slot_lock.acquire(label="holder", timeout=2)
    with pytest.raises(slot_lock.SlotLockTimeout) as ei:
        slot_lock.acquire(timeout=0.3, poll=0.05)
    msg = str(ei.value)
    assert str(os.getpid()) in msg and "windowed test slot" in msg
    slot_lock.release(lockfile)


# --- stale-lock break ----------------------------------------------------

def test_stale_lock_dead_pid_is_broken(lockfile):
    # A pid that cannot exist -> holder is dead -> stale -> broken and acquired.
    _write_lock(lockfile, 0x7FFFFFFF)
    assert slot_lock._is_stale(lockfile) is True
    path = slot_lock.acquire(label="breaker", timeout=1)
    pid, _, _ = slot_lock._read_holder(path)
    assert pid == os.getpid()  # we took it over
    slot_lock.release(path)


def test_stale_lock_old_mtime_is_broken(lockfile):
    # A holder that is alive (this very process) but whose lockfile is older
    # than MAX_HOLD_SECONDS is stale via the mtime backstop, and is broken.
    old = time.time() - (slot_lock.MAX_HOLD_SECONDS + 120)
    _write_lock(lockfile, os.getpid(), acquired=old)
    os.utime(lockfile, (old, old))
    assert slot_lock._is_stale(lockfile) is True
    path = slot_lock.acquire(label="breaker", timeout=1)
    assert os.path.exists(path)
    slot_lock.release(path)


def test_live_foreign_holder_is_not_stale_and_blocks(lockfile):
    # A lock held by a real, alive, recent foreign process is NOT stale, so a
    # waiter must block on it (and time out), never break it.
    sleeper = subprocess.Popen([sys.executable, "-c", "import time; time.sleep(30)"])
    try:
        _write_lock(lockfile, sleeper.pid)
        assert slot_lock._is_stale(lockfile) is False
        with pytest.raises(slot_lock.SlotLockTimeout):
            slot_lock.acquire(timeout=0.4, poll=0.05)
        # the foreign lock is untouched
        pid, _, _ = slot_lock._read_holder(lockfile)
        assert pid == sleeper.pid
    finally:
        sleeper.terminate()
        sleeper.wait()


def test_dead_foreign_holder_becomes_stale_after_crash(lockfile):
    # End to end: a live holder blocks, then it 'crashes' (we kill it) and the
    # lock becomes stale and is broken -- a crashed lane must not wedge the box.
    sleeper = subprocess.Popen([sys.executable, "-c", "import time; time.sleep(30)"])
    _write_lock(lockfile, sleeper.pid)
    assert slot_lock._is_stale(lockfile) is False
    sleeper.terminate()
    sleeper.wait()
    assert slot_lock._is_stale(lockfile) is True
    path = slot_lock.acquire(label="after-crash", timeout=1)
    pid, _, _ = slot_lock._read_holder(path)
    assert pid == os.getpid()
    slot_lock.release(path)


# --- context manager -----------------------------------------------------

def test_slot_context_manager_acquires_and_releases(lockfile):
    with slot_lock.slot(label="ctx", timeout=2) as path:
        assert os.path.exists(path)
        pid, _, _ = slot_lock._read_holder(path)
        assert pid == os.getpid()
    assert not os.path.exists(lockfile)  # released on exit


def test_slot_context_manager_releases_on_exception(lockfile):
    with pytest.raises(ValueError):
        with slot_lock.slot(label="ctx", timeout=2):
            raise ValueError("boom")
    assert not os.path.exists(lockfile)


# --- pid liveness --------------------------------------------------------

def test_pid_alive_true_for_self_false_for_dead(lockfile):
    assert slot_lock._pid_alive(os.getpid()) is True
    assert slot_lock._pid_alive(0x7FFFFFFF) is False
    assert slot_lock._pid_alive(None) is False
    assert slot_lock._pid_alive(-1) is False


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
