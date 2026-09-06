#!/usr/bin/env python3
"""Ratchet on the BYTES this repo actually builds from source, not the ROM it produces.

WHAT IT GATES
-------------
Every range in a `config/**/delinks.txt` entry marked `complete` is a range **mwccarm**
compiles from `src/`. Every range that is not -- because the `complete` line went away,
because the entry went away, because the range narrowed, or because the entry names a
path with no file at it -- is a range **dsd** hands back to the retail cartridge. This
tool sums those bytes at a base ref and at the working tree and fails when the total
goes DOWN, naming the module, the section, the address range, the file that used to own
it, and which of those four things happened to it.

WHY EVERY OTHER GATE IS BLIND TO THIS
-------------------------------------
Because handing a range back to the cartridge produces a byte-PERFECT ROM. That is the
whole trap. `tools/rombuild.py` fills the gap with the cartridge's own bytes, so:

  * the ROM still byte-matches, exactly;
  * module fidelity still reports 106/106 exact;
  * `tools/romdata_check.py` still passes -- it compares emitted data against the
    cartridge, and a range nobody compiled emits no data to disagree;
  * `tools/check_references.py`, the relocation audits and the link checks all still
    pass, because the cartridge's bytes are internally consistent by construction.

There is no red anywhere. The single observable consequence is that fewer bytes came
out of `src/`, which is a number nobody diffs. So it is diffed here.

WHAT `tools/layout_check.py` ALREADY COVERS, AND WHERE IT STOPS
---------------------------------------------------------------
`layout_check.py` exists for a neighbouring reason and its L1 check is the closest thing
in the tree to this one, so the overlap is worth stating precisely rather than shipping a
second copy of it. L1 fails when a delinks entry names a `src/` path with no file there.
That is ONE of the four ways a range gets handed back, and this tool deliberately does
not re-report it as a byte loss when L1 would already name it -- it reports it as the
reason a range regressed, which is the actionable form.

The other three are invisible to `layout_check.py`, by construction and not by oversight:

  * `complete` REMOVED, file still present, entry still present. L1 never looks at the
    `complete` line; `delinks_paths()` keys on the entry header alone.
  * THE ENTRY DELETED outright, file still present. `layout_check` sees exactly this as
    L5 "unenrolled", which is INFO on purpose and can never fail a build -- its own
    docstring explains why, and that reasoning is correct for L5's question and simply
    does not answer this one.
  * THE RANGE NARROWED -- `end:` pulled back, or an entry re-partitioned so that the
    union of the new ranges no longer covers the old one. `layout_check` parses no
    addresses at all.

And underneath all three: `layout_check` counts ENTRIES and inspects only the working
tree. It has no base ref and no notion of a byte. A TU-promotion PR that consolidates
38 per-function entries into 3 merged ones is a large, legitimate churn of entries and
paths and a net-zero change in bytes; only the byte set can tell that apart from a loss.

THE UNIT IS THE COVERED BYTE SET, WHICH IS WHY THE ARITHMETIC LOOKS INDIRECT
----------------------------------------------------------------------------
Comparing `(path, start, end)` tuples is wrong and was tried first. TU promotion
re-partitions the address space: the same bytes move from many per-function files into
one merged file, and tuple comparison reports every consolidated range as a removal AND
an addition. So each side is merged into disjoint intervals per (module, section) and
the two byte SETS are subtracted. A re-partition that covers the same bytes is silent,
which is the correct answer.

AND WHY THE BASE IS A MERGE BASE, NOT `origin/main`
---------------------------------------------------
Comparing a branch against CURRENT main produces false alarms proportional to how far
main moved since the fork -- measured on 2026-08-30, three PRs appeared to lose 4,028
bytes that way and lost exactly zero against their own merge base. The only honest
question is what THIS branch removed relative to the tree it forked from, so
`--merge-base` is the mode CI uses for pull requests. `--base` takes a literal ref for
the push-to-main job, which asks the different and equally necessary question: did main
lose bytes at the moment this merged? A PR can be green against a stale base and desync
main on merge; that exact failure happened to the converted ratchet on 2026-08-30.

SECTIONS, AND THE HONEST STORY ABOUT ROM DATA
----------------------------------------------
This counts EVERY section a `complete` entry names, not just `.text`. That includes
`.init`, and now also the `.data`, `.ctor` and `.bss` ranges owned by promoted intact
translation units. Sections are counted and reported separately, so a non-text claim
cannot disappear behind growth in code coverage.

MOST ROM DATA IS STILL NOT SOURCE-BUILT. Ordinary per-function objects have their
compiler-emitted vtables, typeinfo and other content measured and then isolated away;
DSD gap objects supply those addresses from the cartridge. Promoted intact TUs are the
exception: their explicitly licensed non-text ranges carry `complete` and therefore do
count here. `tools/romdata_check.py` separately compares compiler-emitted data symbols
even when they are not linked from source. A green result here means no previously
source-owned range was handed back; it does not mean the build contains no retail-gap
bytes. Nothing in this file substitutes for emitted-data verification, and nothing in
that measurement substitutes for source ownership.

HOW TO OVERRIDE WHEN A DECREASE IS LEGITIMATE
----------------------------------------------
It sometimes is. A fake match retracted, a file demoted to a ROM-byte placeholder
because its "match" turned out to be a relocation artefact, an entry removed because the
range belonged to another module -- all of those are the tree getting MORE honest while
this number goes down, and a gate with no way to say so would be pressure to leave a
known-bad match enrolled. So the escape hatch is a real mechanism, and like
`config/layout-known-issues.txt` and `config/converted-backslide-exceptions.jsonl` it
lives in the tree, in your diff, with a `git blame` behind it:

    config/source-coverage-exceptions.jsonl     one JSON object per line
    {"module": "ov010", "section": ".text", "start": "0x021e4f00",
     "end": "0x021e4f80", "reason": "func_021e4f00 was a fakematch; demoted in #1234"}

A waived range's bytes are subtracted from the loss. Waivers are consumed once -- after
the decrease lands, main's own total is the new floor and the row is inert -- so stale
rows are harmless and can be pruned in a later sweep. A row with no `reason` is rejected
loudly rather than honoured, because an unexplained waiver is the failure this gate
exists to prevent, wearing a hat.

Usage:
    python tools/source_coverage.py                       # report the working tree
    python tools/source_coverage.py --check --base REF    # exit 1 if bytes DECREASED
    python tools/source_coverage.py --check --merge-base origin/main
    python tools/source_coverage.py --json                # machine-readable
    python tools/source_coverage.py --ref REF             # measure a ref, not the tree

Exit codes: 0 clean, 1 a decrease that no waiver covers, 2 usage/configuration error
(unresolvable ref, malformed waiver). It compiles nothing, links nothing and reads no
ROM: `git cat-file` plus text parsing, about a second over 106 modules. That is a hard
requirement, not a happy accident -- `build/` is shared between worktrees and a gate
that needed a ROM build could not run concurrently with the work it is gating.
"""
import argparse
import collections
import json
import pathlib
import re
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
CONFIG_DIR = "config"
EXCEPTIONS = REPO / "config" / "source-coverage-exceptions.jsonl"

# An entry header is a bare path followed by a colon at zero indent; its body is the
# indented lines under it. `complete` is the whole ballgame -- see tools/enroll.py, which
# documents the two states: with it mwccarm compiles the source, without it dsd supplies
# the object from ROM bytes.
_ENTRY_RE = re.compile(r"^(\S+):\s*$")
_SECTION_RE = re.compile(
    r"^(\.\w+)\s+start:(0x[0-9a-fA-F]+)\s+end:(0x[0-9a-fA-F]+)")
_MODULE_RE = re.compile(r"overlays/(ov\d+)/delinks\.txt$")


class Entry:
    """One delinks entry: a source path, whether it is `complete`, and its ranges."""

    __slots__ = ("path", "module", "delinks", "complete", "sections")

    def __init__(self, path, module, delinks):
        self.path = path
        self.module = module
        self.delinks = delinks
        self.complete = False
        self.sections = []          # [(section, start, end)]


def module_of(delinks_path):
    """`config/arm9/overlays/ov010/delinks.txt` -> `ov010`; the arm9 root -> `arm9`."""
    p = delinks_path.replace("\\", "/")
    m = _MODULE_RE.search(p)
    if m:
        return m.group(1)
    if p.endswith("config/arm9/delinks.txt"):
        return "arm9"
    # Any other layout under config/ (a future module root): name it by its directory, so
    # an unrecognised path is still attributed rather than silently merged with arm9.
    #
    # Not config_tu/, and do not "fix" the scanners to reach it. Both entries_from_tree()
    # and entries_from_ref() are scoped to CONFIG_DIR on purpose: tu_config.py generates
    # config_tu/ as a PARALLEL dsd root for the TU-shaped treemap experiment, and its own
    # contract is that "config/**/delinks.txt remains the sole authority over what mwccarm
    # compiles." Measured on main: 8 delinks files, 29 entries, ZERO marked `complete` --
    # so widening the scan would not find a single source-built byte, it would only import
    # 29 unenrolled entries and the false alarms that come with them.
    return pathlib.PurePosixPath(p).parent.name or p


def parse_delinks(text, delinks_path):
    """Every entry in one delinks.txt, in file order.

    The module header at the top of the file (the `.text start:... kind:code` block that
    precedes the first entry) has no entry header above it, so `cur is None` drops it --
    that block describes the MODULE's sections, not a source file's, and counting it
    would report the entire module as source-built.
    """
    module = module_of(delinks_path)
    out, cur = [], None
    for line in text.splitlines():
        stripped = line.strip()
        if not stripped:
            continue
        m = _ENTRY_RE.match(line)
        if m:
            cur = Entry(m.group(1), module, delinks_path)
            out.append(cur)
            continue
        if cur is None:
            continue
        if stripped == "complete":
            cur.complete = True
            continue
        sec = _SECTION_RE.match(stripped)
        if sec:
            cur.sections.append(
                (sec.group(1), int(sec.group(2), 16), int(sec.group(3), 16)))
    return out


def _git(repo, *args, check=True):
    r = subprocess.run(("git", "-C", str(repo)) + args, capture_output=True,
                       encoding="utf-8", errors="replace")
    if check and r.returncode != 0:
        raise RuntimeError(f"git {' '.join(args)}: {r.stderr.strip()}")
    return r.stdout


def resolve(repo, ref):
    """A ref -> its full sha, or None when it does not resolve.

    Returned rather than raised because CI hands us `github.event.before`, which is forty
    zeros for a branch's first push; the caller has to be able to skip rather than fail.
    """
    try:
        out = _git(repo, "rev-parse", "--verify", f"{ref}^{{commit}}").strip()
    except RuntimeError:
        return None
    return out or None


def merge_base(repo, ref, head="HEAD"):
    try:
        return _git(repo, "merge-base", ref, head).strip() or None
    except RuntimeError:
        return None


def entries_from_tree(repo=None):
    """Every delinks entry in the working tree, plus the set of files on disk.

    The working tree rather than HEAD on purpose: a developer running this before
    committing should see their own edit. In CI the two are identical.
    """
    root = pathlib.Path(repo or REPO)
    entries = []
    for dl in sorted((root / CONFIG_DIR).rglob("delinks.txt")):
        rel = dl.relative_to(root).as_posix()
        entries += parse_delinks(dl.read_text(encoding="utf-8", errors="replace"), rel)
    exists = set()
    for e in entries:
        if (root / e.path).is_file():
            exists.add(e.path)
    return entries, exists


def entries_from_ref(ref, repo=None):
    """Every delinks entry at a git ref, plus the set of paths that ref actually has.

    One `cat-file --batch` for every delinks blob rather than 106 `git show` calls: the
    per-process cost dominates otherwise, and this has to be cheap enough to run on
    every push.
    """
    root = pathlib.Path(repo or REPO)
    listing = _git(root, "ls-tree", "-r", ref, "--", CONFIG_DIR)
    blobs = []
    for line in listing.splitlines():
        if "delinks.txt" not in line:
            continue
        meta, _, path = line.partition("\t")
        parts = meta.split()
        if len(parts) >= 3:
            blobs.append((parts[2], path.strip().replace("\\", "/")))
    entries = []
    if blobs:
        proc = subprocess.run(["git", "-C", str(root), "cat-file", "--batch"],
                              input="\n".join(h for h, _ in blobs) + "\n",
                              capture_output=True, encoding="utf-8", errors="replace")
        out, pos = proc.stdout, 0
        for _, path in blobs:
            nl = out.index("\n", pos)
            size = int(out[pos:nl].split()[2])
            body = out[nl + 1:nl + 1 + size]
            pos = nl + 1 + size + 1
            entries += parse_delinks(body, path)
    tracked = set(_git(root, "ls-tree", "-r", "--name-only", ref).splitlines())
    return entries, {p.strip() for p in tracked}


def built_ranges(entries, exists):
    """[(module, section, path, start, end)] for ranges genuinely built from source.

    An entry is source-built only if it is BOTH marked `complete` AND has a file at the
    path it names. A `complete` entry whose file is missing is the layout_check L1 case,
    and dsd fills it from the cartridge exactly as if `complete` were absent -- counting
    it would make the total a lie in precisely the direction this gate cares about.
    """
    out = []
    for e in entries:
        if not e.complete or e.path not in exists:
            continue
        for sec, start, end in e.sections:
            if end > start:
                out.append((e.module, sec, e.path, start, end))
    return out


def merge_intervals(ranges):
    """-> {(module, section): [(start, end)]} disjoint and sorted.

    The covered BYTE SET is the only unit that survives a TU consolidation; see the
    module docstring.
    """
    by = collections.defaultdict(list)
    for module, sec, _path, start, end in ranges:
        by[(module, sec)].append((start, end))
    out = {}
    for key, iv in by.items():
        iv.sort()
        merged = []
        cs, ce = iv[0]
        for s, e in iv[1:]:
            if s <= ce:
                ce = max(ce, e)
            else:
                merged.append((cs, ce))
                cs, ce = s, e
        merged.append((cs, ce))
        out[key] = merged
    return out


def subtract(a, b):
    """Interval-set difference a \\ b -> [(module, section, start, end)]."""
    out = []
    for key, iv in a.items():
        cuts = sorted(b.get(key, []))
        for s, e in iv:
            cur = s
            for bs, be in cuts:
                if be <= cur or bs >= e:
                    continue
                if bs > cur:
                    out.append((key[0], key[1], cur, bs))
                cur = max(cur, be)
                if cur >= e:
                    break
            if cur < e:
                out.append((key[0], key[1], cur, e))
    return sorted(out)


def total(intervals):
    return sum(e - s for iv in intervals.values() for s, e in iv)


def by_section(intervals):
    out = collections.Counter()
    for (_module, sec), iv in intervals.items():
        out[sec] += sum(e - s for s, e in iv)
    return out


def load_exceptions(path=None):
    """[(module, section, start, end, reason)] from the waiver file.

    Raises ValueError on a malformed row or a row with no reason. Failing loudly is the
    point: a waiver file that silently drops rows it cannot parse is a gate that can be
    disabled with a typo.
    """
    p = pathlib.Path(path if path is not None else EXCEPTIONS)
    if not p.is_file():
        return []
    out = []
    for n, line in enumerate(p.read_text(encoding="utf-8", errors="replace").splitlines(), 1):
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        try:
            row = json.loads(line)
        except ValueError as exc:
            raise ValueError(f"{p}:{n}: not valid JSON: {exc}") from exc
        missing = [k for k in ("module", "section", "start", "end", "reason")
                   if not row.get(k)]
        if missing:
            raise ValueError(
                f"{p}:{n}: waiver is missing {', '.join(missing)}. Every waived range "
                f"needs a reason -- an unexplained waiver is the failure this gate "
                f"exists to prevent.")
        try:
            start, end = int(str(row["start"]), 16), int(str(row["end"]), 16)
        except ValueError as exc:
            raise ValueError(f"{p}:{n}: start/end must be hex addresses: {exc}") from exc
        out.append((row["module"], row["section"], start, end, row["reason"]))
    return out


def apply_waivers(lost, waivers):
    """(still_lost, [(range, reason)]) after subtracting every waived range."""
    if not waivers:
        return lost, []
    cover = collections.defaultdict(list)
    for module, sec, start, end, _reason in waivers:
        cover[(module, sec)].append((start, end))
    remaining = subtract(merge_intervals(
        [(m, s, "", a, b) for m, s, a, b in lost]), cover) if lost else []
    waived = []
    for module, sec, start, end, reason in waivers:
        for lm, ls, lstart, lend in lost:
            if lm == module and ls == sec and lstart < end and lend > start:
                waived.append(((module, sec, max(start, lstart), min(end, lend)), reason))
                break
    return remaining, waived


def blame(lost_range, base_entries, cur_entries, cur_exists):
    """Who owned a lost range in the base, and which of the four things happened to it.

    A bare byte count is not actionable; this is what turns a number into a fix. The
    reasons map one-to-one onto the ways `dsd` reclaims a range, so the reader is told
    what to do (restore the entry, restore `complete`, re-run enroll after the move)
    rather than merely that something is wrong.
    """
    module, sec, start, end = lost_range
    cur_by_path = {}
    for e in cur_entries:
        cur_by_path.setdefault(e.path, e)
    owners = []
    for e in base_entries:
        if e.module != module or not e.complete:
            continue
        for s, a, b in e.sections:
            if s == sec and a < end and b > start:
                now = cur_by_path.get(e.path)
                if now is None:
                    why = "entry no longer in delinks.txt (dsd refills from the cartridge)"
                elif not now.complete:
                    why = "`complete` removed from the entry (dsd refills from the cartridge)"
                elif e.path not in cur_exists:
                    why = "delinks names this path but no file is there (layout_check L1)"
                else:
                    why = "range narrowed or re-partitioned; these bytes are no longer covered"
                owners.append((e.path, why))
                break
    if not owners:
        return [("(no base entry found)", "unattributed -- re-run without --check to inspect")]
    return owners


def report(base_ranges, cur_ranges, base_entries, cur_entries, cur_exists,
           waivers, limit=20):
    """-> (findings dict, printable lines)."""
    ab, ar = merge_intervals(base_ranges), merge_intervals(cur_ranges)
    tb, tr = total(ab), total(ar)
    lost = subtract(ab, ar)
    gained = subtract(ar, ab)
    still_lost, waived = apply_waivers(lost, waivers)
    lost_bytes = sum(e - s for _m, _s, s, e in lost)
    unwaived_bytes = sum(e - s for _m, _s, s, e in still_lost)
    gained_bytes = sum(e - s for _m, _s, s, e in gained)

    lines = [
        f"source-built bytes   base {tb:,} B   ->   now {tr:,} B   (net {tr - tb:+,} B)",
        f"  handed back to the cartridge: {lost_bytes:,} B"
        f"   |   newly built from source: {gained_bytes:,} B",
    ]
    secs = by_section(ar)
    lines.append("  by section: " + ", ".join(f"{k} {v:,} B" for k, v in sorted(secs.items())))
    if waived:
        lines.append(f"  waived by {EXCEPTIONS.name}: "
                     f"{sum(e - s for (_m, _c, s, e), _r in waived):,} B "
                     f"in {len(waived)} range(s)")
        for (module, sec, s, e), reason in waived[:limit]:
            lines.append(f"    ok  {e - s:>7,} B  {module:<8} {sec:<7} "
                         f"{s:#010x}-{e:#010x}  waived: {reason}")
    if still_lost:
        per_module = collections.Counter()
        for module, _sec, s, e in still_lost:
            per_module[module] += e - s
        lines.append("")
        lines.append(f"REGRESSION: {unwaived_bytes:,} B stopped being built from source")
        lines.append("  by module: " + ", ".join(f"{k}={v:,} B" for k, v in per_module.most_common()))
        for module, sec, s, e in sorted(still_lost, key=lambda r: -(r[3] - r[2]))[:limit]:
            lines.append(f"    -{e - s:>7,} B  {module:<8} {sec:<7} {s:#010x}-{e:#010x}")
            for path, why in blame((module, sec, s, e), base_entries,
                                   cur_entries, cur_exists)[:4]:
                lines.append(f"                    was: {path}")
                lines.append(f"                     why: {why}")
        if len(still_lost) > limit:
            lines.append(f"    ... and {len(still_lost) - limit} more range(s)")

    findings = {
        "baseBytes": tb,
        "currentBytes": tr,
        "netBytes": tr - tb,
        "lostBytes": lost_bytes,
        "unwaivedLostBytes": unwaived_bytes,
        "gainedBytes": gained_bytes,
        "bySection": dict(by_section(ar)),
        "lost": [{"module": m, "section": c, "start": hex(s), "end": hex(e),
                  "bytes": e - s,
                  "owners": [{"path": p, "why": w} for p, w in
                             blame((m, c, s, e), base_entries, cur_entries, cur_exists)]}
                 for m, c, s, e in still_lost],
        "waived": [{"module": m, "section": c, "start": hex(s), "end": hex(e),
                    "bytes": e - s, "reason": r} for (m, c, s, e), r in waived],
        "gained": [{"module": m, "section": c, "start": hex(s), "end": hex(e),
                    "bytes": e - s} for m, c, s, e in gained],
    }
    return findings, lines


def main(argv=None):
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--check", action="store_true",
                    help="exit 1 when source-built bytes DECREASE against the base")
    ap.add_argument("--base", metavar="REF",
                    help="compare against this ref literally (the push-to-main job's "
                         "question: did main lose bytes at the moment this merged?)")
    ap.add_argument("--merge-base", metavar="REF", dest="mergebase",
                    help="compare against merge-base(REF, HEAD) -- the only honest base "
                         "for a branch, see the module docstring")
    ap.add_argument("--ref", metavar="REF",
                    help="measure this ref instead of the working tree")
    ap.add_argument("--json", action="store_true", help="emit findings as JSON")
    ap.add_argument("--exceptions", default=None, metavar="PATH",
                    help=f"waiver file (default {EXCEPTIONS.relative_to(REPO).as_posix()})")
    ap.add_argument("--repo", default=None, metavar="PATH",
                    help="repository root (default: this file's repo)")
    ap.add_argument("--limit", type=int, default=20, metavar="N",
                    help="how many regressed ranges to name (default 20)")
    ap.add_argument("--allow-empty-base", action="store_true",
                    help="treat an unresolvable base as 'nothing to compare' and exit 0. "
                         "For a branch's first push, where github.event.before is zeros.")
    args = ap.parse_args(argv)

    try:
        sys.stdout.reconfigure(encoding="utf-8")
    except Exception:
        pass

    repo = pathlib.Path(args.repo or REPO)
    if args.base and args.mergebase:
        print("--base and --merge-base are mutually exclusive: one asks what main lost, "
              "the other what this branch removed.", file=sys.stderr)
        return 2

    try:
        waivers = load_exceptions(args.exceptions)
    except ValueError as exc:
        print(f"malformed waiver file: {exc}", file=sys.stderr)
        return 2

    try:
        if args.ref:
            cur_entries, cur_exists = entries_from_ref(args.ref, repo)
        else:
            cur_entries, cur_exists = entries_from_tree(repo)
    except (RuntimeError, OSError) as exc:
        print(f"cannot read the current tree: {exc}", file=sys.stderr)
        return 2
    cur_ranges = built_ranges(cur_entries, cur_exists)

    base_ref = None
    if args.base:
        base_ref = resolve(repo, args.base)
        if base_ref is None:
            msg = f"base ref {args.base!r} does not resolve"
            if args.allow_empty_base:
                print(f"{msg}; nothing to compare against -- skipping.")
                return 0
            print(msg, file=sys.stderr)
            return 2
    elif args.mergebase:
        base_ref = merge_base(repo, args.mergebase, args.ref or "HEAD")
        if base_ref is None:
            msg = (f"no merge base between {args.mergebase!r} and "
                   f"{args.ref or 'HEAD'!r} -- a shallow checkout is the usual cause; "
                   f"CI needs fetch-depth: 0")
            if args.allow_empty_base:
                print(f"{msg}; skipping.")
                return 0
            print(msg, file=sys.stderr)
            return 2

    if base_ref is None:
        # No base: a plain measurement of the tree in front of you. No exit-code meaning,
        # so it is safe to run mid-edit.
        intervals = merge_intervals(cur_ranges)
        secs = by_section(intervals)
        body = {"currentBytes": total(intervals), "bySection": dict(secs),
                "entries": len(cur_ranges),
                "modules": len({m for m, _ in intervals})}
        if args.json:
            print(json.dumps(body, indent=1, sort_keys=True))
        else:
            print(f"source-built bytes   {total(intervals):,} B "
                  f"across {body['modules']} module(s), {len(cur_ranges):,} range(s)")
            for sec, n in sorted(secs.items()):
                print(f"  {sec:<8} {n:>12,} B")
            print("no --base / --merge-base given: measurement only, no gate.")
        if args.check:
            print("--check needs --base or --merge-base to compare against.",
                  file=sys.stderr)
            return 2
        return 0

    try:
        base_entries, base_exists = entries_from_ref(base_ref, repo)
    except (RuntimeError, OSError, ValueError) as exc:
        print(f"cannot read base {base_ref[:9]}: {exc}", file=sys.stderr)
        return 2
    base_ranges = built_ranges(base_entries, base_exists)

    findings, lines = report(base_ranges, cur_ranges, base_entries, cur_entries,
                             cur_exists, waivers, limit=args.limit)
    findings["base"] = base_ref
    if args.json:
        print(json.dumps(findings, indent=1, sort_keys=True))
    else:
        print(f"base {base_ref[:9]}")
        print("\n".join(lines))

    if findings["unwaivedLostBytes"]:
        if not args.json:
            print(
                "\nEach range above is now filled with RETAIL CARTRIDGE BYTES. The ROM\n"
                "still matches and every other gate is still green -- that is why this\n"
                "one exists. Fix the delinks entry (restore it, restore `complete`, or\n"
                "re-run `python tools/enroll.py` after a move), or, if the decrease is\n"
                "genuinely correct -- a retracted fakematch, a demotion to a ROM-byte\n"
                "placeholder -- record it with a reason in\n"
                f"  {EXCEPTIONS.relative_to(REPO).as_posix()}\n"
                '  {"module": "ovNNN", "section": ".text", "start": "0x...",\n'
                '   "end": "0x...", "reason": "why this is correct"}')
        return 1 if args.check else 0

    if not args.json:
        print("source-coverage: clean (no bytes handed back to the cartridge)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
