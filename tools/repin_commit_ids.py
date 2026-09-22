"""Rewrite old commit ids embedded in tracked text files after a history rewrite.

WHY THIS EXISTS. At 07:50 EDT on 2026-09-13 the whole repository history was rewritten
(an identity scrub over four text files: the trees are otherwise identical, but every
commit got a new id). Nothing in the tree noticed. 121 tracked files spell an OLD commit
id somewhere -- prose provenance comments ("matched at be9d58ed"), fixture data, and one
config field (config/port_linkage.json's "commit") -- and one gate is not just stale
prose: tools/test_check_header_offsets.py's RealHistoryTests run `git diff <sha>^...<sha>`
against real history, so a pinned old id that no longer resolves fails CI outright
("<sha> is not in this clone").

This tool walks the tracked text tree and replaces every token that names an old commit
id with its mapped new id, using a commit-map (one "<old> <new>" pair per line, both
40-hex) produced by the rewrite (git-filter-repo writes one to
filter-repo/commit-map by default).

WHAT COUNTS AS AN OLD COMMIT ID. Either
  (a) a full 40-hex token that is exactly a key in the map, or
  (b) a 7..12-hex token that is a prefix of EXACTLY ONE key in the map (an ambiguous
      prefix -- shared by two or more old ids -- is left alone, since there is no way
      to know which one was meant).
A candidate token is rejected outright, even if it matches (a) or (b), when either is
true:
  - it starts with "02" (or the text right before it is "0x02"): this codebase's
    prose is full of DS RAM addresses in the 0x02000000..0x023fffff range, spelled as
    bare 8-hex tokens ("020cb030") or "0x"-prefixed, and those collide in shape with a
    short commit prefix far more often than chance would suggest;
  - the 24 characters before the token contain (case-insensitively) "0x", "addr",
    "address", "size" or "@": the same shape collision, generalised to every place
    this tree spells an address, a size, or an @-mention rather than a commit. This
    also covers nearmiss/db.jsonl's own "addr"/"address"/"size" JSON fields, since the
    field name always sits inside that 24-character window before its value.
Binary files are skipped, by extension (a conservative list of known binary suffixes)
and, as a backstop for anything the list misses, by a NUL-byte check on the file's own
bytes -- text files in this tree never contain one.

USAGE
    python tools/repin_commit_ids.py --map PATH/TO/commit-map
    python tools/repin_commit_ids.py --map PATH/TO/commit-map --check
    python tools/repin_commit_ids.py --map PATH/TO/commit-map --repo C:/path/to/repo

Default mode rewrites every eligible file in place and prints one line per touched
file with its replacement count, then a total. --check performs the same scan without
writing anything, prints what it would have done, and exits 1 if any file still needs
a rewrite (0 old ids left -> exit 0). This is the same predicate a pre-merge gate would
use: `python tools/repin_commit_ids.py --map <map> --check` should read clean after the
sweep lands.
"""
from __future__ import annotations

import argparse
import collections
import pathlib
import re
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent

# A hex run bounded on both sides by a non-hex character (or string edge), 7..40 chars.
HEX_TOKEN_RE = re.compile(r"(?<![0-9a-fA-F])([0-9a-f]{7,40})(?![0-9a-fA-F])")
# Any of these within 24 characters before a candidate token means "not a commit id":
# an address, a size, an @-mention, or a literal 0x prefix already covered by ADDR_RE
# but kept here too since a hex literal can appear without "0x" right in front of it
# (e.g. a decimal-looking "addr" label a few characters earlier).
CONTEXT_REJECT_RE = re.compile(r"0x|addr|address|size|@", re.I)
CONTEXT_WINDOW = 24

BINARY_EXTS = {
    ".bin", ".png", ".jpg", ".jpeg", ".gif", ".bmp", ".ico", ".webp",
    ".wav", ".mp3", ".ogg", ".flac", ".nds", ".pdf",
    ".zip", ".7z", ".gz", ".tar", ".rar",
    ".exe", ".dll", ".so", ".dylib", ".o", ".obj", ".a", ".lib", ".pyc", ".class",
    ".ttf", ".otf", ".woff", ".woff2",
    ".dat", ".sav", ".nsf",
}

MIN_PREFIX = 7
MAX_PREFIX = 12


def load_commit_map(path: pathlib.Path) -> dict[str, str]:
    """Parse a "<old> <new>" per-line commit-map into {old: new}, both 40-hex.

    Tolerates a header line ("old new", filter-repo's own format) and blank lines.
    """
    old_to_new: dict[str, str] = {}
    with open(path, encoding="utf-8") as f:
        for line in f:
            parts = line.split()
            if len(parts) != 2:
                continue
            old, new = parts
            if old == "old" and new == "new":
                continue
            if len(old) != 40 or len(new) != 40:
                continue
            if not (_is_hex(old) and _is_hex(new)):
                continue
            old_to_new[old] = new
    return old_to_new


def _is_hex(s: str) -> bool:
    return all(c in "0123456789abcdef" for c in s)


def build_prefix_index(old_to_new: dict[str, str]) -> dict[int, dict[str, list[str]]]:
    """{prefix_length: {prefix: [old ids sharing it]}} for lengths MIN_PREFIX..MAX_PREFIX."""
    by_len: dict[int, dict[str, list[str]]] = {}
    for n in range(MIN_PREFIX, MAX_PREFIX + 1):
        counts: dict[str, list[str]] = collections.defaultdict(list)
        for old in old_to_new:
            counts[old[:n]].append(old)
        by_len[n] = dict(counts)
    return by_len


def is_rom_address(token: str, ctx: str) -> bool:
    """True if the token itself, or the text right before it, looks like a DS address."""
    if token.startswith("02"):
        return True
    # the last two non-space characters of ctx, lower-cased, catch "...0x02..."
    tail = ctx.rstrip().lower()
    if tail.endswith("0x02"):
        return True
    return False


def resolve_token(token: str, old_to_new: dict[str, str],
                   prefix_index: dict[int, dict[str, list[str]]]) -> str | None:
    """The replacement for `token` (truncated to len(token)), or None to leave it alone."""
    n = len(token)
    if n == 40:
        return old_to_new.get(token)
    if MIN_PREFIX <= n <= MAX_PREFIX:
        cands = prefix_index.get(n, {}).get(token)
        if cands and len(cands) == 1:
            return old_to_new[cands[0]][:n]
        return None
    return None


def process_text(data: str, old_to_new: dict[str, str],
                  prefix_index: dict[int, dict[str, list[str]]]) -> tuple[str, int]:
    """Return (rewritten_text, replacement_count)."""
    count = 0
    pieces: list[str] = []
    last = 0
    for m in HEX_TOKEN_RE.finditer(data):
        token = m.group(1)
        ctx = data[max(0, m.start() - CONTEXT_WINDOW):m.start()]
        if is_rom_address(token, ctx):
            continue
        if CONTEXT_REJECT_RE.search(ctx):
            continue
        new = resolve_token(token, old_to_new, prefix_index)
        if new is None:
            continue
        pieces.append(data[last:m.start()])
        pieces.append(new)
        last = m.end()
        count += 1
    pieces.append(data[last:])
    return "".join(pieces), count


def is_binary(relpath: str, raw: bytes) -> bool:
    if pathlib.PurePosixPath(relpath).suffix.lower() in BINARY_EXTS:
        return True
    return b"\x00" in raw


def list_tracked_files(repo: pathlib.Path) -> list[str]:
    proc = subprocess.run(["git", "ls-files"], cwd=str(repo),
                           capture_output=True, text=True, check=True)
    return [p for p in proc.stdout.split("\n") if p]


def sweep(repo: pathlib.Path, old_to_new: dict[str, str], check: bool) -> tuple[int, int]:
    """Walk every tracked file; rewrite (or, in --check, just report). Returns
    (files_touched, total_replacements)."""
    prefix_index = build_prefix_index(old_to_new)
    files_touched = 0
    total = 0
    for relpath in list_tracked_files(repo):
        fpath = repo / relpath
        try:
            raw = fpath.read_bytes()
        except (FileNotFoundError, IsADirectoryError, OSError):
            continue
        if is_binary(relpath, raw):
            continue
        try:
            text = raw.decode("utf-8")
        except UnicodeDecodeError:
            continue
        new_text, count = process_text(text, old_to_new, prefix_index)
        if count == 0:
            continue
        files_touched += 1
        total += count
        verb = "would touch" if check else "touched"
        print(f"{count:5d}  {relpath}   ({verb})")
        if not check:
            fpath.write_bytes(new_text.encode("utf-8"))
    return files_touched, total


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description=__doc__,
                                  formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--map", required=True, type=pathlib.Path,
                     help="commit-map file: one '<old-40hex> <new-40hex>' pair per line")
    ap.add_argument("--repo", type=pathlib.Path, default=REPO,
                     help="repo root to sweep (default: this tool's own repo)")
    ap.add_argument("--check", action="store_true",
                     help="report without writing; exit 1 if any old id remains")
    args = ap.parse_args(argv)

    old_to_new = load_commit_map(args.map)
    if not old_to_new:
        print(f"no old->new pairs parsed from {args.map}", file=sys.stderr)
        return 2

    files_touched, total = sweep(args.repo, old_to_new, args.check)

    if args.check:
        if total:
            print(f"--check: {total} old commit id(s) remain across {files_touched} file(s)")
            return 1
        print("--check: 0 old commit ids remain")
        return 0

    print(f"repinned {total} old commit id(s) across {files_touched} file(s)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
