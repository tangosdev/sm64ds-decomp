"""Move root-level `src/` sources into the bucket directories `srcpath` assigns them.

WHY THIS EXISTS
---------------
`src/` held 3,454 files in one directory, and GitHub stops listing a directory at 1,000
entries. `srcpath.placement_for` already routes NEW files into `src/unnamed/<module>/`,
`named/<module>/` and (for arm9) an address shard; this tool relocates the files that
were already flat, so the placement rule and the tree finally agree.

A move is only safe if EVERY record that spells the old path is rewritten in the same
commit. `enroll` writes each source's path into `config/**/delinks.txt`, and `dsd` supplies
retail ROM bytes for any range it has no object for -- so a move that forgets a delinks
row still builds a byte-identical ROM, with one fewer function actually compiled from
source. Nothing goes red. That silent failure is why this is a tool and not a `git mv`.

WHAT IT DOES
------------
  1. plan     which root files move where (``srcpath`` decides the destination, this tool
              only decides WHICH files are in scope). Never moves a file that `port/` names
              by literal path -- the port builds its own host executable off those paths
              and its changes do not ride on main.
  2. move     rename on disk, then stage old+new together so git records a rename.
  3. rekey    rewrite ``src/<old>`` -> ``src/<dir>/<old>`` in every tracked text file
              outside the moved ones (delinks, baselines, manifests, attribution, notes,
              comments in `src/` and `include/`). A JSON file written in canonical form
              (sorted keys, sorted string lists) is re-sorted, so a later regeneration of
              it does not show the move again as churn.
  4. verify   `layout_check` (L1 catches a delinks row that names a path with no file).

Usage:
    python tools/src_move.py --select unnamed:overlays              # dry run: print the plan
    python tools/src_move.py --select unnamed:arm9 --apply
    python tools/src_move.py --select named --limit 200 --apply --receipt moved.json

``--select`` is one or more of: ``unnamed:overlays`` (every non-arm9 address-named file),
``unnamed:arm9``, ``unnamed:<module>`` (one module, e.g. ``unnamed:ov007``), ``named``
(class methods and free functions), ``all``.
"""
import argparse
import collections
import json
import os
import pathlib
import re
import subprocess
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import srcpath as SP  # noqa: E402

REPO = pathlib.Path(__file__).resolve().parent.parent

# A path counts as "the same name" only when the next character cannot extend it.
_NAME_CHARS = "A-Za-z0-9_"
_PORT_REF_RE = re.compile(r"(?<![%s])src/([%s+.\-]+\.(?:c|cpp))(?![%s])"
                          % (_NAME_CHARS, _NAME_CHARS, _NAME_CHARS))
# Text we never rewrite: the sources we just moved are handled as ordinary text too, so
# the only exclusions are binary blobs and generated build output.
_SKIP_DIRS = ("build/", "extracted/")


class Move:
    __slots__ = ("old", "new", "kind", "module")

    def __init__(self, old, new, kind, module):
        self.old, self.new, self.kind, self.module = old, new, kind, module

    def as_dict(self):
        return {"old": self.old, "new": self.new, "kind": self.kind, "module": self.module}


def _git(*args, input_bytes=None):
    return subprocess.run(["git", "-C", str(SP.REPO), *args], input=input_bytes,
                          check=True, stdout=subprocess.PIPE).stdout


def tracked_files(repo=None):
    out = subprocess.run(["git", "-C", str(repo or SP.REPO), "ls-files", "-z"],
                         check=True, stdout=subprocess.PIPE).stdout
    return [f for f in out.decode("utf-8").split("\0") if f]


def root_sources():
    """Repo-relative paths of every .c/.cpp sitting directly in `src/`."""
    return sorted(p.relative_to(SP.REPO).as_posix() for p in SP.SRC.iterdir()
                  if p.is_file() and p.suffix in SP.SOURCE_SUFFIXES)


def port_pinned(files=None):
    """Root basenames that `port/` names by literal `src/<name>` path."""
    pinned = set()
    for f in files if files is not None else tracked_files():
        if not f.startswith("port/"):
            continue
        try:
            data = (SP.REPO / f).read_bytes()
        except OSError:
            continue
        if b"\0" in data[:8192]:
            continue
        for m in _PORT_REF_RE.finditer(data.decode("utf-8", "replace")):
            pinned.add(m.group(1))
    return pinned


def _class_homes(rels):
    """class -> the one directory all of its root methods should land in.

    A class already living in exactly ONE subdirectory keeps following it (that is what
    `placement_for` does for new files). Otherwise the whole class goes to
    `named/<module>/` of its most common module, so it never straddles two
    directories -- which `layout_check` L4 would call a split class."""
    cohort = SP._cohort_index()
    members = collections.defaultdict(list)
    for rel in rels:
        stem = pathlib.PurePosixPath(rel).stem
        cls = SP.class_of(stem)
        if cls:
            members[cls].append(stem)
    homes, skipped = {}, {}
    for cls, stems in members.items():
        nested = {d for d in cohort.get(cls, ()) if d != SP.SRC}
        if len(nested) == 1:
            homes[cls] = next(iter(nested))
            continue
        if len(nested) > 1:
            skipped[cls] = "class already split across %d directories" % len(nested)
            continue
        tally = collections.Counter(m for m in map(SP.named_module_of, stems) if m)
        if not tally:
            skipped[cls] = "no member has a symbols.txt row"
            continue
        # Most files wins; ties go to the lexically first module so the answer is stable.
        mod = sorted(tally.items(), key=lambda kv: (-kv[1], kv[0]))[0][0]
        homes[cls] = SP.SRC / SP.NAMED_DIR / mod
    return homes, skipped


def _owner_module(rel):
    """Module of a root file whose name is not a symbol (a reconstructed multi-symbol TU)."""
    for sym in SP.symbols_for(SP.REPO / rel):
        mod = SP.named_module_of(sym) or SP.module_of(sym)
        if mod:
            return mod
    return None


def plan(selects, limit=None, honour_port=True, files=None):
    """``(moves, skipped)``: Move objects in a stable order, and {rel: reason}."""
    rels = root_sources()
    pinned = port_pinned(files) if honour_port else set()
    homes, class_skip = _class_homes(rels)
    moves, skipped = [], {}
    for rel in rels:
        name = pathlib.PurePosixPath(rel).name
        stem = pathlib.PurePosixPath(rel).stem
        if name in pinned:
            skipped[rel] = "named by a literal path in port/"
            continue
        mod = SP.module_of(stem)
        if mod is not None:
            kind, module = "unnamed", mod
            dest_dir = SP.unnamed_dir_for(stem)
        else:
            cls = SP.class_of(stem)
            if cls:
                if cls not in homes:
                    skipped[rel] = class_skip.get(cls, "class has no home")
                    continue
                dest_dir = homes[cls]
                module = SP.named_module_of(stem) or _owner_module(rel) or ""
            else:
                module = SP.named_module_of(stem) or _owner_module(rel)
                if not module:
                    skipped[rel] = "no symbols.txt row names its module"
                    continue
                dest_dir = SP.SRC / SP.NAMED_DIR / module
            kind = "named"
        if not _selected(selects, kind, module):
            continue
        new = (dest_dir / name).relative_to(SP.REPO).as_posix()
        if new == rel:
            continue
        moves.append(Move(rel, new, kind, module))
    if limit is not None:
        moves = moves[:limit]
    _check_collisions(moves)
    return moves, skipped


def _selected(selects, kind, module):
    for s in selects:
        if s == "all":
            return True
        if s == "named" and kind == "named":
            return True
        if s == "unnamed:overlays" and kind == "unnamed" and module != "arm9":
            return True
        if s.startswith("unnamed:") and kind == "unnamed" and s == "unnamed:" + module:
            return True
    return False


def _check_collisions(moves):
    seen = {}
    for m in moves:
        if (SP.REPO / m.new).exists():
            raise SystemExit("refusing: destination already exists: " + m.new)
        if m.new in seen:
            raise SystemExit("refusing: two files map to " + m.new)
        seen[m.new] = m.old


# --- rekey -------------------------------------------------------------------------

def build_pattern(moves):
    """One regex matching ``src/<old basename>`` for every moved file.

    Longest names first, so ``foo.cpp`` is tried before ``foo.c``; the trailing lookahead
    stops a moved ``foo.c`` from rewriting an unmoved ``foo.cpp``."""
    names = sorted({pathlib.PurePosixPath(m.old).name for m in moves}, key=lambda n: (-len(n), n))
    return re.compile(r"(?<![%s])src/(%s)(?![%s])"
                      % (_NAME_CHARS, "|".join(re.escape(n) for n in names), _NAME_CHARS))


def _dest_by_name(moves):
    return {pathlib.PurePosixPath(m.old).name: pathlib.PurePosixPath(m.new).parent.relative_to("src").as_posix()
            for m in moves}


def rekey_text(text, pattern, dest):
    """``(new_text, n_replacements)``."""
    n = 0

    def sub(m):
        nonlocal n
        n += 1
        return "src/%s/%s" % (dest[m.group(1)], m.group(1))

    return pattern.sub(sub, text), n


def _detect_json_format(obj, text):
    """The ``json.dumps`` kwargs that reproduce ``text`` exactly, or None."""
    body = text[:-1] if text.endswith("\n") else text
    for indent in (2, 1, 4, "\t", None):
        for ensure_ascii in (False, True):
            kw = {"indent": indent, "ensure_ascii": ensure_ascii}
            if indent is None:
                kw["separators"] = (",", ":")
            try:
                if json.dumps(obj, **kw) == body:
                    return kw
            except (TypeError, ValueError):
                return None
    return None


def _resort(orig, new):
    """Rebuild ``new`` so every ordering ``orig`` had still holds.

    ``orig`` and ``new`` have the same shape (only strings changed): a dict whose keys
    were sorted is re-sorted, a list of strings that was sorted is re-sorted, anything
    else keeps its positional order. Recurses by position, so a renamed key keeps its
    value."""
    if isinstance(orig, dict) and isinstance(new, dict):
        okeys, nkeys = list(orig), list(new)
        pairs = [(nk, _resort(orig[ok], new[nk])) for ok, nk in zip(okeys, nkeys)]
        if okeys == sorted(okeys):
            pairs.sort(key=lambda kv: kv[0])
        return dict(pairs)
    if isinstance(orig, list) and isinstance(new, list) and len(orig) == len(new):
        if orig and all(isinstance(x, str) for x in orig) and orig == sorted(orig):
            return sorted(new)
        return [_resort(o, n) for o, n in zip(orig, new)]
    return new


def rekey_json(orig_text, pattern, dest):
    """Rekey a JSON document, preserving canonical form. ``(text, n, canonical)``."""
    new_text, n = rekey_text(orig_text, pattern, dest)
    if n == 0:
        return orig_text, 0, True
    try:
        orig = json.loads(orig_text)
        new = json.loads(new_text)
    except ValueError:
        return new_text, n, False
    kw = _detect_json_format(orig, orig_text)
    if kw is None:
        return new_text, n, False
    out = json.dumps(_resort(orig, new), **kw)
    if orig_text.endswith("\n"):
        out += "\n"
    return out, n, True


def _line_sorted(lines):
    body = [l for l in lines if l.strip() and not l.lstrip().startswith("#")]
    return len(body) > 1 and body == sorted(body)


def rekey_lines(orig_text, pattern, dest):
    """Rekey a line-oriented file, re-sorting it only if it was fully sorted before."""
    new_text, n = rekey_text(orig_text, pattern, dest)
    if n == 0:
        return orig_text, 0, True
    eol = "\r\n" if "\r\n" in orig_text else "\n"
    olines = orig_text.split(eol)
    if _line_sorted(olines):
        tail = olines[-1] == ""
        nlines = new_text.split(eol)
        if tail:
            nlines = nlines[:-1]
        head = [l for l in nlines if not l.strip() or l.lstrip().startswith("#")]
        body = sorted(l for l in nlines if l.strip() and not l.lstrip().startswith("#"))
        new_text = eol.join(head + body + ([""] if tail else []))
    return new_text, n, True


# Files where a line order carries meaning other than sorted-ness are never re-sorted.
_NO_RESORT_NAMES = ("delinks.txt", "symbols.txt")


def rekey_file(rel, pattern, dest):
    """Rewrite one tracked file in place. Returns ``(replacements, canonical, resorted)``."""
    path = SP.REPO / rel
    try:
        raw = path.read_bytes()
    except OSError:
        return 0, True, False
    if b"\0" in raw[:8192]:
        return 0, True, False
    try:
        text = raw.decode("utf-8")
    except UnicodeDecodeError:
        return 0, True, False
    if "src/" not in text:
        return 0, True, False
    suffix = path.suffix.lower()
    if suffix == ".json":
        new, n, canonical = rekey_json(text, pattern, dest)
    elif suffix in (".tsv", ".txt") and path.name not in _NO_RESORT_NAMES:
        new, n, canonical = rekey_lines(text, pattern, dest)
    else:
        new, n = rekey_text(text, pattern, dest)
        canonical = True
    if n and new != text:
        path.write_bytes(new.encode("utf-8"))
    return n, canonical, bool(n) and new != text


def apply(moves, files=None):
    """Move the files, stage the renames, rekey every reference. Returns a summary."""
    old_rels = [m.old for m in moves]
    for m in moves:
        dst = SP.REPO / m.new
        dst.parent.mkdir(parents=True, exist_ok=True)
        os.rename(SP.REPO / m.old, dst)
    paths = "\0".join(old_rels + [m.new for m in moves]) + "\0"
    _git("add", "-A", "--pathspec-from-file=-", "--pathspec-file-nul",
         input_bytes=paths.encode("utf-8"))

    pattern, dest = build_pattern(moves), _dest_by_name(moves)
    total, touched, non_canonical = 0, 0, []
    for rel in files if files is not None else tracked_files():
        if rel.startswith(_SKIP_DIRS):
            continue
        n, canonical, changed = rekey_file(rel, pattern, dest)
        total += n
        touched += 1 if changed else 0
        if n and not canonical:
            non_canonical.append(rel)
    SP.invalidate()
    return {"moved": len(moves), "replacements": total, "files_rewritten": touched,
            "text_replaced_only": non_canonical}


def decl_agreement_is_red():
    """Read-only: does the decl-agreement gate fail on the tree as it now stands?

    A move can flip which side of a plurality TIE the gate calls wrong (ties fall to path
    order) without a single declaration changing, so the rekeyed baseline can read as
    225 "new" disagreements of symbols it already carries. This tool does not re-bank on
    its own -- `check_decl_agreement.py --update` rewrites the whole baseline, including
    rows unrelated to the move, and that is a diff a human should read. It only says so."""
    gate = [sys.executable, str(REPO / "tools" / "check_decl_agreement.py")]
    return subprocess.run(gate, cwd=SP.REPO, stdout=subprocess.DEVNULL,
                          stderr=subprocess.DEVNULL).returncode != 0


def leftover_backslash_refs(moves, files=None):
    """Files still spelling a moved path with backslashes -- the one form rekey skips."""
    pat = re.compile(r"src\\\\?(" + "|".join(re.escape(pathlib.PurePosixPath(m.old).name)
                                            for m in moves[:2000]) + r")")
    hits = []
    for rel in files if files is not None else tracked_files():
        if rel.startswith(_SKIP_DIRS):
            continue
        try:
            text = (SP.REPO / rel).read_text(encoding="utf-8")
        except (OSError, UnicodeDecodeError):
            continue
        if pat.search(text):
            hits.append(rel)
    return hits


def summarise(moves, skipped):
    by = collections.Counter((m.kind, m.module, pathlib.PurePosixPath(m.new).parent.as_posix())
                             for m in moves)
    print("%d file(s) to move; %d skipped" % (len(moves), len(skipped)))
    for (kind, module, d), n in sorted(by.items()):
        print("  %-8s %-6s -> %-34s %5d" % (kind, module, d + "/", n))
    reasons = collections.Counter(skipped.values())
    for why, n in reasons.most_common():
        print("  skipped %5d: %s" % (n, why))


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--select", action="append", required=True,
                    help="unnamed:overlays | unnamed:arm9 | unnamed:<module> | named | all")
    ap.add_argument("--limit", type=int, help="move at most this many files")
    ap.add_argument("--apply", action="store_true", help="do it (default: print the plan)")
    ap.add_argument("--receipt", help="write the move list to this JSON file")
    ap.add_argument("--ignore-port", action="store_true",
                    help="also move files port/ names by literal path (port/ is rekeyed too)")
    a = ap.parse_args()

    moves, skipped = plan(a.select, a.limit, honour_port=not a.ignore_port)
    summarise(moves, skipped)
    if a.receipt:
        pathlib.Path(a.receipt).write_text(
            json.dumps([m.as_dict() for m in moves], indent=1) + "\n", encoding="utf-8")
    if not a.apply:
        print("dry run: pass --apply to move")
        return 0
    if not moves:
        return 0
    summary = apply(moves)
    print(json.dumps(summary, indent=1))
    stale = leftover_backslash_refs(moves)
    if stale:
        print("WARNING: backslash-spelled references remain in: " + ", ".join(stale[:10]))
    if decl_agreement_is_red():
        print("NOTE: check_decl_agreement.py is red after the move. If its output is "
              "'plurality is ...' disagreements for symbols the baseline already carries, "
              "a tie flipped with the new path order: run "
              "`python tools/check_decl_agreement.py --update` and read the diff "
              "(the count should not grow by more than a handful).")
    import layout_check
    return 1 if layout_check.print_report(layout_check.check(), quiet=True) else 0


if __name__ == "__main__":
    sys.exit(main())
