"""The byte-gate-failure half of the matched test: sources that are named after a
matched symbol and that no compiler in the sweep will build.

WHY THIS IS A FILE AND NOT AN `if`
-----------------------------------
`matched` used to mean "a src/ file exists, unbannered, not a dcd transcription".
audit/enrollment_report.md put every one of the 251 matched-but-unverified functions
through tools/linkcheck.py and found 22 that the byte gate reports NO-SYM: 18 that no
compiler in reverify_corpus.ALL_VERSIONS will build, and 4 zero-size alias records whose
sized twin at the same address is separately counted unmatched. Policy D drops exactly
those 22 and keeps everything that reproduces the cartridge. This module is the half of
that test which cannot be computed from committed data alone.

The class splits by what it takes to decide it, and the two halves are implemented
differently on purpose:

  zero-size alias   DERIVED, live, by alias_collision_addresses() below. It reads
                    config/**/symbols.txt and nothing else, so the progress and Chaos
                    generators share one definition and it self-heals in both
                    directions: fix the config and the record returns to the count;
                    match a previously unmatched alias and it is excluded the same day.

  will not build    a MANIFEST, config/bytegate-known-failures.txt, because deciding it
                    means running mwccarm. The two workflows that regenerate the
                    published count (update-chaos-data.yml, report.yml) run on a stock
                    ubuntu runner with pyelftools and capstone installed and nothing
                    else; tools/mwccarm and tools/bin are gitignored and no workflow
                    fetches them. There is no compiler in the process that computes the
                    number, so the number has to read a recorded verdict.

A bare name list would not self-heal -- someone fixes one of the rows and the function
stays uncounted until a human remembers this file exists. So each row pins the sha256 of
the source it was recorded against, and the exclusion lapses the moment the file changes.
A fix therefore restores the count with no list edit and no compiler. The cost is that a
cosmetic edit lapses it too, which would re-count a file that still does not build; that
is what tools/test_bytegate.py's staleness check is for, and it fails loudly rather than
letting the count go quietly wrong. Both failure directions land on the old status quo
plus a red test, never on a silently smaller-or-larger number.

That property has now been exercised: all 18 original rows were repaired without anyone
consulting this file, and every one lapsed on its own. The manifest is now empty.

What this does NOT do: notice a NEW file that will not build. Nothing in CI can, for the
same no-compiler reason. `--scan` below is the discovery pass, run with a compiler.

Usage:
  python tools/bytegate.py                       # show the manifest and its health
  python tools/bytegate.py --recheck             # recompile every row (needs mwccarm)
  python tools/bytegate.py --scan chaos-db.json  # find rows the manifest is missing
"""
import argparse
import hashlib
import pathlib
import sys
import re

REPO = pathlib.Path(__file__).resolve().parent.parent
MANIFEST = REPO / "config" / "bytegate-known-failures.txt"
HASH_LEN = 16
FUNC_RE = re.compile(
    r"^(\S+)\s+kind:function\([^)]*,size=0x([0-9a-fA-F]+)\).*?"
    r"addr:0x([0-9a-fA-F]+)"
)


def alias_collision_addresses(module_universe=None) -> set[tuple[str, int]]:
    """``{(module, addr)}`` where a zero-size function aliases a sized record.

    Only the zero-size side is excluded from MATCHED. Address-keying lets callers make
    that decision while visiting the full symbol record, and deriving it from committed
    config keeps the progress and Chaos generators in lockstep.
    """
    if module_universe is None:
        sys.path.insert(0, str(REPO / "tools"))
        import relocs as RL
        module_universe = RL.module_universe

    out = set()
    for symbols, label in module_universe():
        sized, zero = set(), []
        for line in symbols.read_text(errors="ignore").splitlines():
            match = FUNC_RE.match(line)
            if not match:
                continue
            size, addr = int(match.group(2), 16), int(match.group(3), 16)
            if size:
                sized.add(addr)
            else:
                zero.append(addr)
        out.update((label, addr) for addr in zero if addr in sized)
    return out


def source_hash(path: pathlib.Path) -> str | None:
    """sha256 of the file's bytes with CRLF folded to LF, truncated.

    Bytes rather than decoded text, because 42 files in src/ carry non-ASCII in codegen
    comments and hashing decoded text would make the pin depend on the reader's encoding
    -- which on Windows is cp1252 and has fabricated "corruption" in this repo before.

    LF-normalised, because the pin has to mean the same thing on both platforms and the
    raw bytes do not. This repository has no `text=auto` rule in .gitattributes, so line
    endings are left to each clone's core.autocrlf: src/game/actors/Tornado/_ZN7Tornado6State1Ev.cpp is 105
    CRLF and zero bare LF in a default Windows checkout and 105 bare LF on the ubuntu
    runner that regenerates the published count. A raw-byte pin recorded on Windows
    therefore misses on every row in CI, all 18 exclusions lapse at once, and the count
    silently returns to the pre-policy number while every local check still passes. That
    is the failure this normalisation exists to prevent, and test_bytegate covers it
    directly.

    Folding CRLF is also the right semantics on its own: a line-ending flip is not
    somebody fixing a signature clash, so it must not retire the row.
    """
    try:
        return hashlib.sha256(
            path.read_bytes().replace(b"\r\n", b"\n")).hexdigest()[:HASH_LEN]
    except OSError:
        return None


def manifest_rows(path: pathlib.Path | None = None) -> list[dict]:
    """[{src, sha, reason, line}] in file order. Malformed lines raise rather than being
    skipped: a typo that silently dropped a row would silently re-inflate the count.

    `path=None` rather than `path=MANIFEST`: a default argument is bound once at import,
    so the constant form would ignore any later reassignment of MANIFEST and quietly read
    the real config out of a test fixture's repository."""
    path = path or MANIFEST
    rows = []
    if not path.is_file():
        return rows
    for n, raw in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        line = raw.split("#", 1)[0].strip()
        if not line:
            continue
        parts = line.split()
        if len(parts) != 3:
            raise ValueError(f"{path.name}:{n}: expected '<sha16> <src path> <reason>', "
                             f"got {raw.strip()!r}")
        sha, src, reason = parts
        rows.append({"src": src, "sha": sha, "reason": reason, "line": n})
    return rows


def excluded_paths(path: pathlib.Path | None = None) -> set[str]:
    """The src paths whose exclusion is LIVE: listed, present, and still hashing to the
    value the row was recorded against. A row whose file moved or changed contributes
    nothing here, so the function goes back to counting matched (the old behaviour) and
    test_bytegate flags the row as stale."""
    out = set()
    for r in manifest_rows(path):
        if source_hash(REPO / r["src"]) == r["sha"]:
            out.add(r["src"])
    return out


def stale_rows(path: pathlib.Path | None = None) -> list[dict]:
    """Rows whose recorded evidence no longer describes the file on disk, each tagged
    'missing' (the file is gone) or 'changed' (it was edited without a re-check)."""
    out = []
    for r in manifest_rows(path):
        have = source_hash(REPO / r["src"])
        if have is None:
            out.append({**r, "problem": "missing", "have": None})
        elif have != r["sha"]:
            out.append({**r, "problem": "changed", "have": have})
    return out


def builds_anywhere(src: pathlib.Path) -> bool:
    """True if ANY compiler in the sweep produces an object for this source.

    Mirrors reloc_audit.winning_object's sweep -- same //cpp sniff, same both-flag
    fallback, same 12 versions -- minus the ROM and the byte compare, which is the whole
    point: this asks only whether the file is buildable, so it can be answered without an
    extracted cartridge. Imports are local because this is the one function in the module
    that needs the matching toolchain, and chaos_db_ci must import the module without it.
    """
    import os
    import tempfile
    sys.path.insert(0, str(REPO / "tools"))
    import match as M
    import swarm as S
    import reverify_corpus as RV

    text = src.read_text(encoding="utf-8", errors="replace")
    attempts = ([(S.CPP_FLAGS, ".cpp")] if text.startswith("//cpp")
                else [(M.DEFAULT_FLAGS, ".c"), (S.CPP_FLAGS, ".cpp")])
    for flags, suf in attempts:
        fd, tmp = tempfile.mkstemp(suffix=suf)
        os.close(fd)
        cfile = pathlib.Path(tmp)
        cfile.write_text(text, encoding="utf-8")
        try:
            for v in RV.ALL_VERSIONS:
                if M.compile_c(cfile, v, flags, ()) is not None:
                    return True
        finally:
            os.unlink(tmp)
    return False


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--recheck", action="store_true",
                    help="recompile every manifest row and report which still fail")
    ap.add_argument("--scan", default=None, metavar="CHAOS_DB",
                    help="compile every matched-but-unverified row in this chaos-db and "
                         "report will-not-build files the manifest does not list")
    args = ap.parse_args()

    rows = manifest_rows()
    stale = stale_rows()
    print(f"{MANIFEST.relative_to(REPO).as_posix()}: {len(rows)} row(s), "
          f"{len(excluded_paths())} live, {len(stale)} stale")
    for s in stale:
        print(f"  STALE ({s['problem']}) line {s['line']}: {s['src']}"
              + (f"  recorded {s['sha']}, on disk {s['have']}" if s["have"] else ""))

    if args.recheck:
        print("\nrecompiling every row across all 12 versions ...")
        retire = []
        for r in rows:
            p = REPO / r["src"]
            if not p.is_file():
                print(f"  MISSING  {r['src']}")
                continue
            if builds_anywhere(p):
                print(f"  RETIRE   {r['src']} now builds -- drop this row")
                retire.append(r)
            else:
                h = source_hash(p)
                flag = "" if h == r["sha"] else f"  (re-pin sha to {h})"
                print(f"  CONFIRM  {r['src']} still fails{flag}")
        print(f"\n{len(rows) - len(retire)} confirmed, {len(retire)} to retire")
        return 1 if retire or stale else 0

    if args.scan:
        import json
        db = json.loads(pathlib.Path(args.scan).read_text(encoding="utf-8"))
        listed = {r["src"] for r in rows}
        gap = [f for f in db["functions"]
               if f.get("matched") and not f.get("verified") and f.get("srcPath")]
        print(f"\nscanning {len(gap)} matched-but-unverified row(s) for files the "
              f"manifest does not list ...")
        missing = []
        for f in sorted({f["srcPath"] for f in gap}):
            if f in listed:
                continue
            if not builds_anywhere(REPO / f):
                print(f"  UNLISTED {f} will not build")
                missing.append(f)
        print(f"\n{len(missing)} unlisted will-not-build file(s)")
        return 1 if missing else 0
    return 1 if stale else 0


if __name__ == "__main__":
    sys.exit(main())
