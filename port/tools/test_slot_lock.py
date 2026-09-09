#!/usr/bin/env python3
"""Unit tests for the windowed test slot lock (port/tools/slot_lock.py).

These test the LOCK MECHANISM directly and need no ROM, no compiler and no
walk_window.exe: acquire blocks a second acquire, release frees it, the timeout
fires, and a stale lock (dead holder pid, or a lockfile older than the max-hold
bound) is detected and broken, and the wait is FAIR -- waiters are served in
arrival order, a dead waiter's ticket does not block a live one behind it, and
an old (pre-ticket) client can still barge the plain lock the moment it is
free, which is the accepted compatibility shape (run link100, lane SLOTQ; see
FAIRNESS in slot_lock.py's module docstring). Run with:
pytest port/tools/test_slot_lock.py

The sibling of port/tools/test_build_lock.py, which does the same for the one
full-build lock; both locks share their ticket-queue engine through
port/tools/lock_tickets.py.
"""
import io
import json
import os
import subprocess
import sys
import threading
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import slot_lock
import lock_tickets

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
    # and any queue ticket a test planted, so nothing leaks into the next test.
    tdir = slot_lock.tickets_dir(str(path))
    if os.path.isdir(tdir):
        for name in os.listdir(tdir):
            try:
                os.remove(os.path.join(tdir, name))
            except OSError:
                pass


def _plant_ticket(lockfile, arrival_ns, pid, label="planted"):
    """Plant a queue ticket somebody else is supposed to own."""
    tdir = slot_lock.tickets_dir(lockfile)
    os.makedirs(tdir, exist_ok=True)
    full = os.path.join(tdir, lock_tickets.ticket_name(arrival_ns, pid))
    with open(full, "w", encoding="utf-8") as f:
        json.dump({"pid": pid, "host": "test", "label": label,
                   "arrived": arrival_ns / 1e9}, f)
    return full


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


# --- held() and the re-entrant phase hold (run link100, lane SLOT) --------
# The per-launch lock serialises a lane against ITSELF, which is why a battery
# could not run two of its own rows at once. slot_reentrant is the way out, and
# these pin both halves of its contract: nesting our own hold is free, and
# acquire() is NOT re-entrant -- test_acquire_blocks_a_second_acquire above
# stays true, which is what makes a lost release show up as a hang rather than
# as two runs quietly sharing the slot.

def test_held_is_true_only_while_we_hold_it(lockfile):
    assert slot_lock.held(lockfile) is False
    slot_lock.acquire(label="mine", timeout=2)
    assert slot_lock.held(lockfile) is True
    slot_lock.release(lockfile)
    assert slot_lock.held(lockfile) is False


def test_held_is_false_for_a_foreign_holder(lockfile):
    with open(lockfile, "w", encoding="utf-8") as f:
        json.dump({"pid": 0x7FFFFFFF, "host": "elsewhere",
                   "acquired": time.time(), "label": "theirs"}, f)
    assert slot_lock.held(lockfile) is False


def test_slot_reentrant_nests_without_releasing_early(lockfile):
    with slot_lock.slot_reentrant(label="phase", timeout=2) as outer:
        assert os.path.exists(lockfile)
        with slot_lock.slot_reentrant(label="row", timeout=2) as inner:
            assert str(inner) == str(outer)
            assert os.path.exists(lockfile)
        # the INNER exit must not have freed the slot
        assert os.path.exists(lockfile)
        assert slot_lock.held(lockfile) is True
    assert not os.path.exists(lockfile)


def test_slot_reentrant_alone_behaves_like_slot(lockfile):
    with slot_lock.slot_reentrant(label="solo", timeout=2) as p:
        assert os.path.exists(p)
        pid, _, _ = slot_lock._read_holder(p)
        assert pid == os.getpid()
    assert not os.path.exists(lockfile)


def test_slot_reentrant_waits_for_a_foreign_holder(lockfile):
    # Nesting is free only for OUR OWN hold. A live foreign holder still shuts
    # this process out, which is the property the whole module exists for.
    with open(lockfile, "w", encoding="utf-8") as f:
        json.dump({"pid": os.getppid() or 1, "host": "x",
                   "acquired": time.time(), "label": "theirs"}, f)
    with pytest.raises(slot_lock.SlotLockTimeout):
        with slot_lock.slot_reentrant(label="mine", timeout=0.4, poll=0.05):
            pass


def test_slot_reentrant_nesting_is_thread_safe(lockfile):
    # The phase hold is taken on one thread and the rows run on others, so the
    # depth is mutated from all of them; a drifted counter would release the
    # slot with children still inside it.
    seen = []

    def row(i):
        with slot_lock.slot_reentrant(label="row%d" % i, timeout=2):
            seen.append(os.path.exists(lockfile))
            time.sleep(0.05)

    with slot_lock.slot_reentrant(label="phase", timeout=2):
        ts = [threading.Thread(target=row, args=(i,)) for i in range(4)]
        for t in ts:
            t.start()
        for t in ts:
            t.join()
        assert os.path.exists(lockfile)
    assert seen == [True] * 4
    assert not os.path.exists(lockfile)
    assert slot_lock._nest_depth == 0


# --- a hold may declare its own bound (run link100, lane SLOT) ------------
# One constant had to be two things at once: small enough to recover the box
# from a lane that wedged holding the slot, and large enough never to fire on a
# hold that is legitimately long. Both happened on this box on the same
# afternoon. So a long hold declares itself and an undeclared one still gets the
# short leash.

def test_a_declared_bound_keeps_a_long_hold_alive(lockfile):
    long_hold = slot_lock.MAX_HOLD_SECONDS * 3
    slot_lock.acquire(label="a long phase", timeout=2, max_hold=long_hold)
    old = time.time() - (slot_lock.MAX_HOLD_SECONDS + 300)
    os.utime(lockfile, (old, old))
    d = json.loads(open(lockfile, encoding="utf-8").read())
    assert d["max_hold"] == long_hold
    # Past the DEFAULT bound, inside the declared one -> not stale.
    assert slot_lock._is_stale(lockfile) is False
    slot_lock.release(lockfile)


def test_a_declared_bound_still_expires(lockfile):
    slot_lock.acquire(label="a long phase", timeout=2,
                      max_hold=slot_lock.MAX_HOLD_SECONDS * 2)
    old = time.time() - (slot_lock.MAX_HOLD_SECONDS * 2 + 300)
    os.utime(lockfile, (old, old))
    assert slot_lock._is_stale(lockfile) is True
    slot_lock.release(lockfile)


def test_an_undeclared_hold_keeps_the_short_leash(lockfile):
    # The wedged case: nothing declared, so the default still breaks it. This is
    # the property that must not be lost to make long holds workable.
    slot_lock.acquire(label="wedged", timeout=2)
    old = time.time() - (slot_lock.MAX_HOLD_SECONDS + 120)
    os.utime(lockfile, (old, old))
    assert "max_hold" not in json.loads(open(lockfile, encoding="utf-8").read())
    assert slot_lock._is_stale(lockfile) is True
    slot_lock.release(lockfile)


def test_a_declaration_cannot_shorten_the_leash(lockfile):
    # A hold that claims it will be over in a second must not become breakable
    # a second later; the default is a floor.
    slot_lock.acquire(label="optimist", timeout=2, max_hold=1)
    assert slot_lock._declared_max_hold(lockfile) == slot_lock.MAX_HOLD_SECONDS
    assert slot_lock._is_stale(lockfile) is False
    slot_lock.release(lockfile)


# --- FAIRNESS: the ticket queue (run link100, lane SLOTQ) -----------------
# A lane running a proof that took the slot once per rung (ipc_proof) lost the
# poll race against other lanes' batteries for 100+ minutes and never
# completed a rung: a release went to whichever process happened to poll next,
# not to the one that had waited longest. slot_lock.py ports build_lock.py's
# arrival-ticket fix onto the windowed slot via the shared
# port/tools/lock_tickets.py engine -- these tests are the slot-specific half
# of test_build_lock.py's fairness coverage, plus the compatibility case that
# is unique to this lock: other lanes are running the OLD (pre-ticket)
# slot_lock.py against this exact lockfile tonight.

def test_tickets_dir_sits_beside_the_lockfile(lockfile):
    tdir = slot_lock.tickets_dir(lockfile)
    assert os.path.dirname(tdir) == os.path.dirname(lockfile)
    assert os.path.basename(tdir) == "windowed_test.tickets"


def test_uncontended_acquire_writes_no_ticket(lockfile):
    # The no-ticket path must be exactly what it was before the queue existed:
    # nobody waiting, so the slot is taken on the first attempt and the ticket
    # directory is not even created.
    path = slot_lock.acquire(label="alone", timeout=2)
    assert os.path.exists(path)
    assert slot_lock.queue(lockfile) == []
    assert not os.path.isdir(slot_lock.tickets_dir(lockfile))
    slot_lock.release(path)


def test_three_waiters_acquire_the_slot_in_arrival_order(lockfile):
    # THE GATE CASE. Three real processes queue behind a holder (this process)
    # and are released together. The FIRST arrival deliberately polls SLOWEST
    # (0.5s) and the two behind it poll fastest (0.02s), so under the old
    # re-poll rule the first arrival would essentially never win the create;
    # under the ticket rule it goes first every time -- this is the exact
    # shape of the starvation lane WM6 measured against ipc_proof.py.
    here = os.path.dirname(os.path.abspath(__file__))
    worker = os.path.join(os.path.dirname(lockfile), "slot_fair_worker.py")
    with open(worker, "w", encoding="utf-8") as f:
        f.write(
            "import os, sys, time\n"
            "sys.path.insert(0, sys.argv[5])\n"
            "import slot_lock\n"
            "label, delay, out = sys.argv[1], float(sys.argv[2]), sys.argv[4]\n"
            "poll = float(sys.argv[3])\n"
            "time.sleep(delay)\n"
            "p = slot_lock.acquire(label=label, timeout=90, poll=poll)\n"
            "with open(out, 'a', encoding='utf-8') as fh:\n"
            "    fh.write(repr(time.time()) + ' ' + label + chr(10))\n"
            "    fh.flush()\n"
            "    os.fsync(fh.fileno())\n"
            "time.sleep(0.2)\n"
            "slot_lock.release(p)\n")
    order_file = os.path.join(os.path.dirname(lockfile), "slot_fair_order.txt")
    env = dict(os.environ)
    env["SM64DS_TEST_LOCK_PATH"] = lockfile
    path = slot_lock.acquire(label="fair-holder", timeout=5)
    waiters = []
    try:
        for i, (label, poll) in enumerate((("fair-w1", "0.5"), ("fair-w2", "0.02"),
                                           ("fair-w3", "0.02")), start=1):
            waiters.append(subprocess.Popen(
                [sys.executable, worker, label, "0", poll, order_file, here],
                env=env, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL))
            spent = 0.0
            while len(slot_lock.queue(lockfile)) < i and spent < 30:
                time.sleep(0.05)
                spent += 0.05
            assert len(slot_lock.queue(lockfile)) == i, f"{label} never queued"
        assert [t["label"] for t in slot_lock.queue(lockfile)] == \
            ["fair-w1", "fair-w2", "fair-w3"]
        slot_lock.release(path)          # the holder lets go; the queue drains
        for w in waiters:
            assert w.wait(timeout=90) == 0
        rows = [ln.split() for ln in
                open(order_file, encoding="utf-8").read().splitlines() if ln.strip()]
        assert [r[1] for r in rows] == ["fair-w1", "fair-w2", "fair-w3"]
        stamps = [float(r[0]) for r in rows]
        assert stamps == sorted(stamps)
        assert slot_lock.queue(lockfile) == []    # each removed its own ticket
        assert not os.path.exists(lockfile)       # and the last one released
    finally:
        for w in waiters:
            if w.poll() is None:
                w.terminate()
                w.wait()
        slot_lock.release(lockfile)


def test_a_fresh_arrival_does_not_barge_past_an_older_ticket(lockfile):
    # THE DEFECT (mirrors test_build_lock.py). The slot is FREE, but a live
    # waiter arrived first and holds a ticket. A fresh NEW-STYLE arrival must
    # wait its turn and time out rather than take the free slot from under it.
    sleeper = subprocess.Popen([sys.executable, "-c", "import time; time.sleep(30)"])
    try:
        _plant_ticket(lockfile, time.time_ns() - 5 * 10**9, sleeper.pid,
                      label="arrived-first")
        assert not os.path.exists(lockfile)
        t0 = time.time()
        with pytest.raises(slot_lock.SlotLockTimeout):
            slot_lock.acquire(label="barger", timeout=0.6, poll=0.05)
        assert time.time() - t0 >= 0.5
        assert not os.path.exists(lockfile)       # left free for the waiter
        assert [t["label"] for t in slot_lock.queue(lockfile)] == ["arrived-first"]
    finally:
        sleeper.terminate()
        sleeper.wait()


def test_stale_ticket_dead_pid_does_not_block_a_live_waiter(lockfile):
    # A killed waiter must not wedge the queue behind it: its ticket is stale
    # by the dead-pid rule and is broken with a loud line naming it, so a
    # younger LIVE waiter gets the slot instead of waiting on a dead one.
    _plant_ticket(lockfile, time.time_ns() - 5 * 10**9, 0x7FFFFFFF,
                 label="killed-waiter")
    assert len(slot_lock.queue(lockfile)) == 1
    err = io.StringIO()
    import contextlib
    with contextlib.redirect_stderr(err):
        path = slot_lock.acquire(label="younger", timeout=2, poll=0.05)
    assert slot_lock._read_holder(path)[0] == os.getpid()
    assert "BROKE STALE TICKET" in err.getvalue()
    assert "killed-waiter" in err.getvalue()
    assert slot_lock.queue(lockfile) == []
    slot_lock.release(path)


def test_timed_out_waiter_removes_its_ticket(lockfile):
    sleeper = subprocess.Popen([sys.executable, "-c", "import time; time.sleep(30)"])
    try:
        with open(lockfile, "w", encoding="utf-8") as f:
            json.dump({"pid": sleeper.pid, "host": "test", "acquired": time.time(),
                       "label": "foreign-holder"}, f)
        with pytest.raises(slot_lock.SlotLockTimeout):
            slot_lock.acquire(label="gives-up", timeout=0.6, poll=0.05)
        assert slot_lock.queue(lockfile) == []
    finally:
        sleeper.terminate()
        sleeper.wait()


def test_status_lists_the_queue_after_the_holder(lockfile):
    # `status` is how a lane sees WHY it is waiting and how far down it is.
    import contextlib
    sleeper = subprocess.Popen([sys.executable, "-c", "import time; time.sleep(30)"])
    try:
        with open(lockfile, "w", encoding="utf-8") as f:
            json.dump({"pid": sleeper.pid, "host": "test", "acquired": time.time(),
                       "label": "the-holder"}, f)
        _plant_ticket(lockfile, time.time_ns() - 2 * 10**9, sleeper.pid,
                      label="waiter-one")
        _plant_ticket(lockfile, time.time_ns(), sleeper.pid, label="waiter-two")
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            assert slot_lock.main(["status"]) == 0
        text = out.getvalue()
        assert "the-holder" in text
        assert text.index("the-holder") < text.index("waiter-one") < \
            text.index("waiter-two")
        assert "2 waiting" in text
        assert len(slot_lock.queue(lockfile)) == 2   # reading did not change it
    finally:
        sleeper.terminate()
        sleeper.wait()


def test_a_waiting_ticket_holder_prints_its_queue_position(lockfile, monkeypatch):
    # THE GATE CASE: "a ticket holder's wait line prints its position."
    # WAIT_NOTICE_SECONDS is normally 60s; shrink it so the periodic notice
    # fires inside a short test.
    monkeypatch.setattr(slot_lock, "WAIT_NOTICE_SECONDS", 0.05)
    sleeper = subprocess.Popen([sys.executable, "-c", "import time; time.sleep(30)"])
    try:
        with open(lockfile, "w", encoding="utf-8") as f:
            json.dump({"pid": sleeper.pid, "host": "test", "acquired": time.time(),
                       "label": "the-holder"}, f)
        # An older ticket ahead of us, so our own queue position is 2.
        _plant_ticket(lockfile, time.time_ns() - 5 * 10**9, sleeper.pid,
                      label="ahead-of-us")
        err = io.StringIO()
        import contextlib
        with contextlib.redirect_stderr(err):
            with pytest.raises(slot_lock.SlotLockTimeout):
                slot_lock.acquire(label="behind", timeout=0.5, poll=0.05)
        out = err.getvalue()
        assert "waiting" in out and "windowed test slot" in out
        assert "queue position 2 of 2" in out
    finally:
        sleeper.terminate()
        sleeper.wait()


def test_old_style_client_barges_a_free_lock_past_ticket_holders(lockfile):
    # THE GATE CASE: "an old-style client holding the plain lock is honoured
    # by ticket holders." An old (pre-ticket) slot_lock.py never reads or
    # writes the ticket queue -- its acquire loop is exactly slot_lock.py's
    # own _try_create(), the primitive that both old and new clients share and
    # that this refactor left byte-for-byte unchanged. So an old client acting
    # while a new client's ticket is queued is simulated here by calling
    # _try_create directly (bypassing the fairness gate entirely, as old code
    # does): it still takes a free lock even though a ticket is ahead of it in
    # line -- the accepted compatibility shape documented in slot_lock.py's
    # FAIRNESS section, not a bug.
    _plant_ticket(lockfile, time.time_ns() - 5 * 10**9, os.getpid() + 1,
                  label="new-style-waiter")
    assert not os.path.exists(lockfile)
    ok = slot_lock._try_create(lockfile, "old-style-client", None)
    assert ok is True
    pid, _, _ = slot_lock._read_holder(lockfile)
    assert pid == os.getpid()
    # The ticket holder's place in line is untouched by the old client barging.
    assert [t["label"] for t in slot_lock.queue(lockfile)] == ["new-style-waiter"]
    slot_lock.release(lockfile)


def test_a_queue_that_cannot_be_written_fails_open(lockfile, monkeypatch):
    # Fairness must never fail CLOSED: if the ticket directory cannot be
    # written, a waiter that could not take a place in line goes back to
    # first-to-poll-wins rather than waiting out a turn it can never get.
    sleeper = subprocess.Popen([sys.executable, "-c", "import time; time.sleep(30)"])
    try:
        _plant_ticket(lockfile, time.time_ns() - 5 * 10**9, sleeper.pid,
                      label="arrived-first")
        monkeypatch.setattr(lock_tickets, "write_ticket",
                            lambda *a, **k: (None, OSError("nope")))
        path = slot_lock.acquire(label="ticketless", timeout=2, poll=0.05)
        assert os.path.exists(path)                 # it acquired, unfairly
        slot_lock.release(path)
    finally:
        sleeper.terminate()
        sleeper.wait()


# --- no-pytest standalone runner -----------------------------------------
# When pytest is not installed this drives the same test bodies with a minimal
# tmp_path + monkeypatch, so the lock logic can be proven anywhere Python runs.

def _standalone():
    import pathlib
    import tempfile
    import traceback

    import builtins

    class _Monkeypatch:
        def __init__(self):
            self._env = []
            self._attrs = []

        def setenv(self, k, v):
            self._env.append((k, os.environ.get(k)))
            os.environ[k] = v

        def delenv(self, k, raising=True):
            self._env.append((k, os.environ.get(k)))
            os.environ.pop(k, None)

        def setattr(self, obj, name, value):
            self._attrs.append((obj, name, getattr(obj, name)))
            builtins.setattr(obj, name, value)

        def undo(self):
            for obj, name, old in reversed(self._attrs):
                builtins.setattr(obj, name, old)
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
