"""Canonical match-ledger I/O. The ONE module that touches progress/matched.jsonl
and progress/nonmatching.jsonl.

Everything that used to hand-roll ledger access (sweep, swarm, bank, clone,
paramclone, bank_harvest, crunch, nearmiss_db, ...) goes through here, so:

  * keys are normalized ONCE: every record maps to (module_label, addr_int).
    module missing -> "arm9"; modules.py's internal name "main" IS the public
    label "arm9" and is folded into it; addr accepts int or hex string.
  * appends are serialized by a cross-process lock (atomic-mkdir mutex, the
    nearmiss_db pattern) and the done-set is RE-CHECKED under the lock, so two
    concurrent bankers cannot land the same (module, addr) twice.
  * corrupt ledger lines are reported to stderr with their line number instead
    of being silently swallowed.

Two done-set flavors, because they answer different questions:
  matched_set()  -- byte-exact matches only (matched.jsonl). Use where "done"
                    means "verified match": treemap coloring, coloring study,
                    example selection, clone/paramclone template side, the
                    near-miss DB's is-it-still-pending check.
  load_done()    -- matched PLUS parked (nonmatching.jsonl). Use where "done"
                    means "do not select / re-bank": schedulers, target skips,
                    duplicate-bank pre-filters.
"""
import json
import os
import pathlib
import sys
import time

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import srcpath as SP  # noqa: E402
import asm_policy  # noqa: E402

SRC = REPO / "src"
MATCHED = REPO / "progress" / "matched.jsonl"
NONMATCHING = REPO / "progress" / "nonmatching.jsonl"
NEARMISS = REPO / "nearmiss" / "db.jsonl"
LOCKDIR = REPO / "progress" / ".lock"


# ----------------------------------------------------------------------- keys
def norm_module(module):
    """Public module label: 'arm9' for missing/None/'main', else as-is (ov006...)."""
    if not module or module == "main":
        return "arm9"
    return module


def norm_addr(addr):
    """Address as int; accepts int or a string in any base-0 form ('0x02004000')."""
    return int(addr, 0) if isinstance(addr, str) else int(addr)


def make_key(module, addr):
    return (norm_module(module), norm_addr(addr))


def key_of(record):
    """Canonical (module_label, addr_int) for any ledger/worklist/DB record."""
    return make_key(record.get("module"), record["addr"])


# -------------------------------------------------------------------- readers
def read_records(path):
    """Yield parsed records from a JSONL ledger. A line that fails to parse or
    lacks an addr is reported to stderr with its line number, never swallowed."""
    path = pathlib.Path(path)
    if not path.is_file():
        return
    for lineno, line in enumerate(
            path.read_text(encoding="utf-8", errors="ignore").splitlines(), 1):
        if not line.strip():
            continue
        try:
            yield json.loads(line)
        except Exception as e:
            print(f"ledger: {path}:{lineno}: corrupt line skipped ({e})",
                  file=sys.stderr)


def _key_set(path):
    out = set()
    for rec in read_records(path):
        try:
            out.add(key_of(rec))
        except Exception as e:
            print(f"ledger: {path}: record without usable module/addr skipped ({e})",
                  file=sys.stderr)
    return out


def matched_set():
    """Canonical keys of every byte-exact match in matched.jsonl."""
    return _key_set(MATCHED)


def nonmatching_set():
    """Canonical keys of everything parked in nonmatching.jsonl."""
    return _key_set(NONMATCHING)


def floor_set():
    """Canonical keys of every near-miss carrying a verified `floor` mark.

    nonmatching.jsonl is gitignored, so a scheduler that parks only on THAT filters
    nothing on a fresh clone. nearmiss/db.jsonl is committed, which makes it the only
    portable record of "this one is a verified wall, do not schedule it". Reading it
    here means every scheduler gets the same answer instead of each re-deriving one:
    coddog and refine_wl already consulted the DB, while worklist.py and harvest_wl.py
    did not, so an agent that built a worklist directly was still offered floors."""
    out = set()
    for rec in read_records(NEARMISS):
        try:
            if rec.get("floor"):
                out.add(key_of(rec))
        except Exception:
            pass                        # a malformed DB row must not break scheduling
    return out


def parked_set():
    """Everything no scheduler should offer as a target: locally parked + verified floors."""
    return nonmatching_set() | floor_set()


def load_done():
    """matched + parked: the do-not-select / do-not-re-bank set."""
    return matched_set() | nonmatching_set()


def schedule_skip_set():
    """Keys an LLM-spending scheduler must not hand out: matched, locally parked, or a
    verified floor. Deliberately NOT load_done(): the free local sweeps (clone,
    paramclone, sweep, swarm) cost no tokens, and floor marks have been wrong often
    enough that excluding them from a zero-cost pass is a pure loss. Use this only where
    a target turns into model spend."""
    return matched_set() | parked_set()


# ---------------------------------------------------------------------- lock
class locked:
    """Cross-process mutex via atomic mkdir. Hold it only for the brief
    read-check-append, never while compiling or permuting. Default lock guards
    the progress/ ledgers; pass another dir to guard a different resource
    (nearmiss_db uses nearmiss/.lock for its DB)."""

    def __init__(self, lockdir=LOCKDIR, timeout=120.0):
        self.lockdir = pathlib.Path(lockdir)
        self.timeout = timeout

    def __enter__(self):
        self.lockdir.parent.mkdir(parents=True, exist_ok=True)
        deadline = time.monotonic() + self.timeout
        while True:
            try:
                os.mkdir(self.lockdir)
                return self
            except FileExistsError:
                if time.monotonic() >= deadline:
                    raise TimeoutError(f"could not acquire {self.lockdir}")
                time.sleep(0.1)

    def __exit__(self, *a):
        try:
            os.rmdir(self.lockdir)
        except OSError:
            pass


# -------------------------------------------------------------------- writers
def _format(record):
    """Normalized on-disk form: module as public label, addr as 0x%08x, size int."""
    rec = dict(record)
    mod, addr = key_of(record)
    rec["module"] = mod
    rec["addr"] = f"0x{addr:08x}"
    if isinstance(rec.get("size"), str):
        rec["size"] = int(rec["size"], 0)
    return rec


def _append_line(path, rec):
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("a", encoding="utf-8") as f:
        f.write(json.dumps(rec) + "\n")


def append_matched(record):
    """Append to matched.jsonl unless the key is already matched. The check
    happens under the lock, so concurrent bankers cannot double-append.
    Returns True if written, False on duplicate."""
    rec = _format(record)
    with locked():
        if key_of(rec) in matched_set():
            return False
        _append_line(MATCHED, rec)
    return True


def append_nonmatching(record):
    """Append to nonmatching.jsonl unless the key is already matched or parked.
    Returns True if written, False if skipped."""
    rec = _format(record)
    with locked():
        if key_of(rec) in load_done():
            return False
        _append_line(NONMATCHING, rec)
    return True


def bank(record, src_text):
    """Bank one verified match: write src/<name>.c|cpp and append the record to
    matched.jsonl, both under the ledger lock.

    Returns "banked", "dup" (key already matched -- e.g. a concurrent banker
    won the race), or "refused". Refusals go to stderr and cover:
      * <name> already in matched.jsonl under a DIFFERENT (module, addr);
      * an existing src/<name>.* file we cannot prove belongs to this key
        (the only sanctioned overwrite is a NONMATCHING hatch parked for the
        SAME key, which a verified match upgrades).
    """
    rec = _format(record)
    k = key_of(rec)
    name = rec["name"]
    ext = "cpp" if src_text.startswith("//cpp") else "c"
    body = src_text if src_text.endswith("\n") else src_text + "\n"

    with locked():
        keys, owner = set(), None
        for r in read_records(MATCHED):
            try:
                k2 = key_of(r)
            except Exception:
                continue
            keys.add(k2)
            if owner is None and r.get("name") == name and k2 != k:
                owner = k2
        if k in keys:
            return "dup"
        if owner is not None:
            print(f"ledger: REFUSED to bank {name} at {k}: name already matched "
                  f"at {owner}", file=sys.stderr)
            return "refused"

        existing = SP.paths_for(name)
        if existing:
            # Adoption: the file already on disk IS this verified source (typical
            # after merging a contributor's PR - src arrives via git, the local
            # gitignored ledger has no entry). Append the entry, leave the file.
            same = [p for p in existing
                    if p.suffix == "." + ext
                    and p.read_text(encoding="utf-8") == body]
            if same:
                _append_line(MATCHED, rec)
                _drop_nonmatching(k)
                return "banked"
            parked_owner = None
            for r in read_records(NONMATCHING):
                if r.get("name") == name:
                    try:
                        parked_owner = key_of(r)
                    except Exception:
                        pass
                    break
            # A committed "// NONMATCHING" hatch (e.g. PR #84's draft bank) is
            # also sanctioned: the banner proves it is a hatch even though the
            # local gitignored nonmatching.jsonl never saw it.
            all_hatches = all(
                asm_policy.has_draft_banner(
                    p.read_text(encoding="utf-8", errors="replace"))
                for p in existing)
            if parked_owner != k and not all_hatches:
                print(f"ledger: REFUSED to bank {name} at {k}: "
                      f"{existing[0].name} exists and is not a hatch parked for "
                      f"this (module, addr)", file=sys.stderr)
                return "refused"
            stale = [p for p in existing if p.suffix != "." + ext]
            if stale:
                if all_hatches:
                    # verified match upgrades the hatch; drop the odd-extension
                    # twin so it cannot shadow the new file in progress counts
                    for p in stale:
                        p.unlink()
                else:
                    print(f"ledger: REFUSED to bank {name} at {k}: existing "
                          f"{stale[0].name} has a different extension than the "
                          f"new .{ext}; resolve by hand", file=sys.stderr)
                    return "refused"

        dest = SP.new_path_for(name, ext)
        dest.parent.mkdir(parents=True, exist_ok=True)
        dest.write_text(body, encoding="utf-8")
        SP.invalidate()
        _append_line(MATCHED, rec)
        _drop_nonmatching(k)
    return "banked"


def _drop_nonmatching(k):
    """A verified match overrides a stale park: remove any NONMATCHING rows for
    this key. Caller holds the ledger lock. Atomic rewrite."""
    if not NONMATCHING.exists():
        return
    kept, dropped = [], 0
    for line in NONMATCHING.read_text(encoding="utf-8").splitlines():
        if not line.strip():
            continue
        try:
            if key_of(json.loads(line)) == k:
                dropped += 1
                continue
        except Exception:
            pass
        kept.append(line)
    if dropped:
        tmp = NONMATCHING.with_name(NONMATCHING.name + ".tmp")
        tmp.write_text("".join(l + "\n" for l in kept), encoding="utf-8")
        os.replace(tmp, NONMATCHING)
