"""Content-addressed object cache for tools/rombuild.py.

A stock build compiles every enrolled source file on every run -- 9,116 of them at
present -- and a one-function file costs ~100 ms that is almost entirely mwccarm
process startup under Wine. That is ~15 CPU-minutes per build spent recompiling
source nobody touched. The PR validator pays it twice per job (once for the base
tree, once for the merged tree) in order to check a handful of edited files.

This is ccache's "direct mode", scoped to this build:

    source key = sha256(schema, source path, compiler version, compiler bytes,
                        final flags, source bytes)
    manifest   = the header list mwccarm reported for that source key last time
    object key = sha256(source key, each dependency's repo path + content hash)

A file with no manifest compiles with ``-MD``; the ``.d`` mwccarm writes names the
headers it actually read, and that list is stored as the manifest. The next build
computes the object key straight from it and never starts the compiler. A changed
header therefore rebuilds exactly the files that include it, not all 9,116.

Correctness notes, because a false hit would hand a PR a green ROM-build verdict
for source that was never built:

  * Keys are content, never mtime. The validator's clone resets between unrelated
    commits with ``build/`` surviving, so file timestamps carry no ordering meaning
    there and an mtime-driven cache would reuse objects from a different tree.
  * ``-MD`` does not perturb codegen and neither does the scratch working directory
    the ``.d`` is written into. Both were checked byte-identical against a plain
    ``cwd=REPO`` compile, for a C and a C++ file, before this cache was written.
  * A dependency token that does not resolve to a real file makes that compile
    uncacheable rather than cached-with-an-unknown-input. Slow is recoverable.
  * Headers outside the repository belong to the mwccarm install, whose bytes are
    hashed into the source key.
  * A manifest records the headers that WERE read, never the lookups that failed,
    so in general a newly added header that shadows an existing one is invisible to
    the key -- direct mode's known hole. It does not apply here: mwccarm resolves a
    quoted include from `-i include` even when a same-named header sits beside the
    source (checked on the build box with the build's exact flags), and there is
    only one include root, so a PR has nowhere to introduce a shadowing header.
    Re-check that if `-cwd` or a second `-i` is ever added to CFLAGS.
  * A path containing a space makes its compile uncacheable, because mwccarm's .d
    escaping cannot be told apart from a path separator. That costs speed on such a
    checkout and shows up honestly as "0 reused" in the build's tally.

Set ``ROMBUILD_CACHE`` to relocate the store, or pass ``--no-cache`` to rombuild.py
to bypass it entirely (which restores the previous always-compile behaviour).
"""
import hashlib
import json
import os
import pathlib
import re
import shutil
import tempfile
import threading

# Bump when the meaning of a key input changes; every stored entry then misses.
# Bumped to 2 when objisolate landed. Entries written before it were stored
# post-isolation by a version whose symbol sweep missed STB_LOPROC, and because
# isolate() is idempotent it declines to re-fix an object that already looks
# reduced -- so a stale entry would be served broken forever. The cache now stores
# the compile UN-isolated and applies isolation after fetch, so a later change to
# objisolate needs no bump. Note it stores the RETARGETED compile -- retarget runs
# before put -- so a change to retarget_text_section still does.
SCHEMA = 2

_DRIVE_RE = re.compile(r"^[A-Za-z]:/")


def _resolve_dep(token, cwd, repo):
    """An existing file for one token of a .d dependency list, or None.

    mwccarm emits Windows paths. Under Wine that is the ``Z:`` drive standing in for
    the filesystem root; run natively it is a real drive letter. Try both readings
    and let the filesystem decide, rather than guessing which host we are on.
    """
    text = token.replace("\\", "/")
    if _DRIVE_RE.match(text):
        candidates = [text, text[2:]]
    else:
        candidates = [os.path.join(cwd, text)]
    for candidate in candidates:
        try:
            path = pathlib.Path(os.path.abspath(candidate))
            if path.is_file():
                return path  # absolute but NOT symlink-resolved; the caller needs both
        except OSError:
            continue
    return None


def parse_depfile(text, cwd, repo):
    """Repo-relative posix paths for the headers a .d file names, sorted and unique.

    Returns None when any token fails to resolve: an input we cannot hash has to make
    the compile uncacheable, not quietly drop out of the key. Paths containing spaces
    fail this way too -- mwccarm's escaping is ambiguous when the separator is also a
    backslash -- which costs those files their cache entry and nothing else.
    """
    # Strip CR first: mwccarm is a Windows tool and may end lines with CRLF, which
    # would leave the "\<newline>" continuation unmatched and the join wrong.
    body = text.replace("\r", "").replace("\\\n", " ")
    body = body.replace("\\", "/")
    # Strip the target. Its colon is the first one NOT followed by a separator, since
    # a drive letter's colon always is ("Z:/tmp/x.o: /repo/src/a.c" has two).
    split = re.search(r":(?!/)", body)
    if split is None:
        return None
    deps = set()
    for token in body[split.end():].split():
        path = _resolve_dep(token, cwd, repo)
        if path is None:
            return None
        real = path.resolve()
        inside_lexically = _is_within(path, repo)
        inside_really = _is_within(real, repo)
        if inside_lexically and not inside_really:
            # A symlink under include/ pointing out of the tree. Dropping it would
            # leave edits to its target invisible to the key forever, so refuse the
            # entry instead. (rombuild.enrolled already rejects symlinked sources;
            # this is the same rule applied to headers.)
            return None
        if not inside_really:
            continue  # compiler-install header; the version string already pins it
        deps.add(real.relative_to(repo).as_posix())
    return sorted(deps)


def _is_within(path, repo):
    try:
        path.relative_to(repo)
    except ValueError:
        return False
    return True


class ObjectCache:
    """Store of compiled objects keyed by their exact inputs.

    Every method is safe to call from the build's compile threads: reads need no
    lock, and writes land through a temp file plus os.replace so a torn entry can
    never be observed.
    """

    def __init__(self, root, repo, enabled=True):
        self.root = pathlib.Path(root)
        self.repo = pathlib.Path(repo).resolve()
        self.enabled = bool(enabled)
        self._hashes = {}  # repo-relative posix path -> sha256 of its bytes
        self._tools = {}   # compiler exe path -> sha256 of its bytes
        self._lock = threading.Lock()
        if self.enabled:
            try:
                for sub in ("objects", "manifests", "tmp"):
                    (self.root / sub).mkdir(parents=True, exist_ok=True)
            except OSError:
                # An unwritable cache directory is a performance problem, not a build
                # failure -- fall back to compiling everything.
                self.enabled = False

    # --- paths ------------------------------------------------------------------
    @property
    def scratch(self):
        return self.root / "tmp"

    def _shard(self, kind, key, suffix):
        return self.root / kind / key[:2] / (key + suffix)

    def _write_atomic(self, dest, write):
        dest.parent.mkdir(parents=True, exist_ok=True)
        fd, tmp = tempfile.mkstemp(dir=str(self.scratch))
        os.close(fd)
        try:
            write(pathlib.Path(tmp))
            os.replace(tmp, dest)
        except OSError:
            try:
                os.unlink(tmp)
            except OSError:
                pass

    # --- keys -------------------------------------------------------------------
    def tool_hash(self, exe):
        """sha256 of a compiler binary, memoised for this build.

        The version is a directory name, and tools/mwccarm is licensed material that
        no repository tracks -- so nothing else would notice the box being
        re-provisioned, or an install re-patched, under the same version string. The
        cache is deliberately built to outlive any single build, which is exactly the
        window in which stale objects would be reused against a compiler that no
        longer produces those bytes. A couple of 2MB hashes per build closes that.
        """
        exe = str(exe)
        got = self._tools.get(exe)
        if got is None:
            try:
                got = hashlib.sha256(pathlib.Path(exe).read_bytes()).hexdigest()
            except OSError:
                got = "missing"
            with self._lock:
                self._tools[exe] = got
        return got

    def source_key(self, rel, version, flags, exe):
        """Everything about this compile that is knowable without preprocessing.

        The path is part of the key even though the bytes are hashed too. Two files
        with identical contents at different paths need not produce identical objects
        -- a compiler is free to record the translation unit's name -- and settling
        that question per mwccarm version is not worth the entry it would save.
        """
        digest = hashlib.sha256()
        digest.update(f"{SCHEMA}\0{rel}\0{version}\0{self.tool_hash(exe)}\0{flags}\0".encode())
        try:
            digest.update((self.repo / rel).read_bytes())
        except OSError:
            return None
        return digest.hexdigest()

    def _dep_hash(self, rel):
        """sha256 of one dependency's bytes, memoised for this build.

        A header is included by hundreds of sources, so without the memo the object
        keys would re-read the whole include tree once per enrolled file. Two threads
        can race to compute the same entry; the work is pure, so the loser's result
        is identical and the duplicated effort is bounded by the thread count.
        """
        got = self._hashes.get(rel)
        if got is None:
            try:
                got = hashlib.sha256((self.repo / rel).read_bytes()).hexdigest()
            except OSError:
                # A vanished header changes the key, so this misses and recompiles --
                # which is what a missing input should do.
                got = "missing"
            with self._lock:
                self._hashes[rel] = got
        return got

    def object_key(self, source_key, deps):
        digest = hashlib.sha256()
        digest.update(source_key.encode())
        for dep in deps:
            digest.update(f"\0{dep}\0{self._dep_hash(dep)}".encode())
        return digest.hexdigest()

    # --- entries ----------------------------------------------------------------
    def manifest(self, source_key):
        """The dependency list recorded for this source key, or None if unseen."""
        try:
            raw = self._shard("manifests", source_key, ".json").read_bytes()
            deps = json.loads(raw)["deps"]
        except (OSError, ValueError, KeyError, TypeError):
            return None
        return deps if isinstance(deps, list) else None

    def fetch(self, object_key, dest):
        """Place a stored object at dest. True when the cache supplied it."""
        src = self._shard("objects", object_key, ".o")
        try:
            shutil.copyfile(src, dest)
        except OSError:
            return False
        try:
            os.utime(src)  # keep live generations young so pruning evicts stale ones
        except OSError:
            pass
        return True

    def put(self, source_key, deps, obj):
        """Record a fresh compile: its dependency list and the object it produced."""
        payload = json.dumps({"deps": deps}, indent=None).encode()
        self._write_atomic(self._shard("manifests", source_key, ".json"),
                           lambda tmp: tmp.write_bytes(payload))
        self._write_atomic(self._shard("objects", self.object_key(source_key, deps), ".o"),
                           lambda tmp: shutil.copyfile(obj, tmp))

    def deps_from(self, scratch_dir):
        """Parse the single .d mwccarm left in a compile's scratch directory."""
        try:
            found = list(pathlib.Path(scratch_dir).glob("*.d"))
        except OSError:
            return None
        if len(found) != 1:
            return None
        try:
            text = found[0].read_text(encoding="utf-8", errors="replace")
        except OSError:
            return None
        return parse_depfile(text, scratch_dir, self.repo)

    # --- housekeeping -----------------------------------------------------------
    def prune(self, max_bytes):
        """Drop the least recently used objects until the store fits. Bytes freed.

        Objects average under a kilobyte and one full build is ~8 MB of them, so this
        only matters over many months of validator jobs -- but an unbounded cache on a
        shared box is the kind of thing that is discovered when the disk fills.
        """
        if not self.enabled or max_bytes <= 0:
            return 0
        entries, total = [], 0
        for path in (self.root / "objects").rglob("*.o"):
            try:
                stat = path.stat()
            except OSError:
                continue
            entries.append((stat.st_mtime, stat.st_size, path))
            total += stat.st_size
        if total <= max_bytes:
            return 0
        entries.sort()
        want = total - int(max_bytes * 0.8)
        freed = 0
        for _mtime, size, path in entries:
            if freed >= want:
                break
            try:
                path.unlink()
                freed += size
            except OSError:
                continue
        return freed

    def summary(self, outcomes):
        """Report rows for a build's compile-phase tally (see rombuild.compile_one).

        The tally is the build's, not the cache's, so that the number the report
        publishes is the same one the build actually acted on. A validator verdict
        says how much of the ROM it rebuilt, rather than leaving that to be assumed.
        """
        return {"enabled": self.enabled,
                "reused": outcomes.get("hit", 0),
                "compiled": outcomes.get("miss", 0) + outcomes.get("uncacheable", 0),
                "uncacheable": outcomes.get("uncacheable", 0),
                "root": str(self.root)}
