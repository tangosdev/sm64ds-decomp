#!/usr/bin/env python3
"""Re-derive the measurable columns of notes/data/tu-promotion-queue.tsv.

The queue drives target selection for the whole class-reconstruction pipeline,
and until this tool existed it had no generator at all: `classqueue.py` only
READS it. Every column was hand-carried, and every one of them had been wrong on
every class that actually checked it -- `already_promoted` goes stale on each
promotion, `shard_count` under-counts by however many adjacent factories
`tu_map.py` failed to label, `compiler-only:~N` was copied from a sibling with no
depth check, and `no-legacy-source` asserted sourceless functions that have both
a `src/` file and a `delinks.txt` entry.

    python tools/queue_audit.py            # report what disagrees
    python tools/queue_audit.py --write    # rewrite the rows in place
    python tools/queue_audit.py --check    # exit 1 if anything disagrees

WHAT IS DERIVED, AND FROM WHAT
------------------------------
already_promoted   every named class has a config/tu_manifest.d entry whose
                   status is exactly `promoted`.
shard_count        distinct src/ files covering the TU's ROM run, where the run
                   is tu_map.py's unit EXTENDED over zero-gap neighbours named
                   <Class>_classInit / <Class>_Spawn. tu_map cuts on symbol
                   NAME, so those factories fall outside a run they physically
                   abut; absorbing them adds a member to about 50 rows. It is
                   still a FLOOR -- any other unlabelled neighbour is uncounted.
unmatched:N        delink entries in the run with no `complete` marker.
                   The NAME MISLEADS: it is a link condition, not a matching
                   one. Measured on ov066/Eyerok, all six `unmatched` shards
                   byte-MATCH under `match.py --strict-relocs`; they carry no
                   `complete` marker because they reference cross-overlay
                   addresses under names no module's symbols.txt gives, so dsd
                   drops them from the link. Check the bytes before assuming
                   there is decompilation work to do.
no-legacy-source:N functions in the run with NO delinks entry and NO
                   src/<symbol>.c[pp] -- the cartridge's own bytes cover them.
not-in-delinks:N   functions with a src/ file but no delinks entry. A milder
                   and separate condition; conflating the two is what made
                   `no-legacy-source` assert holes that are not there.
compiler-only:>=N  N = 2 x |union of every named class's own ancestor chain|
                   + one vtable per named class, from build/rtti.json and, where
                   a class has no RTTI record, its include/ base clause.

The compiler-only figure is a FLOOR and this tool proves it rather than assuming
it: calibrated against all 108 promoted manifests it is exact 45 times and an
under-count 37 times on the text-only route, and never an over-count. On
`intact-object` the TU owns its own _ZTV/_ZTI/_ZTS at their natural address, so
the floor is 3 lower -- 16 of 16 intact rows agree. Two things it cannot
predict: a TU whose key function falls outside the licensed range emits nothing
at all (0 rows, measured on dScMgBase_c, dScMgHanachan_c, dScMgBomroom_c), and
every Vector3 the TU odr-uses -- function locals included -- adds a row.

pragma:N is measured (shards in the run carrying a `#pragma` line) but is NOT a
blocker: `#pragma defer_codegen off` makes positional brackets bind, and a
pragma count means nothing until a delete-outright control shows the bytes move.

Inputs it needs, and how to make them:

    python tools/rtti_extract.py                       -> build/rtti.json
    python tools/rtti_vtables.py --out build/rtti_vtables.json
    python tools/tu_map.py --out build/tu_map.json
"""
import argparse
import collections
import csv
import glob
import json
import os
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
QUEUE = REPO / "notes" / "data" / "tu-promotion-queue.tsv"
RTTI = REPO / "build" / "rtti.json"
TU_MAP = REPO / "build" / "tu_map.json"

# `;` separates blockers, except inside a parenthesised reason.
BLOCKER_SPLIT = re.compile(r";(?![^()]*\))")
SYM_RE = re.compile(r"^(\S+)\s+kind:function\(([^)]*)\)\s+addr:(0x[0-9a-fA-F]+)")
PRAGMA_RE = re.compile(r"^\s*#pragma\s+\w+")
BASE_RE = re.compile(
    r"^\s*(?:class|struct)\s+([A-Za-z_]\w*)\s*:\s*"
    r"(?:public\s+|private\s+|protected\s+|virtual\s+)*([A-Za-z_][\w:]*)\s*(?:,|\{|$)")


def split_blockers(s):
    return [b.strip() for b in BLOCKER_SPLIT.split(s or "")
            if b.strip() and b.strip() != "-"]


def missing_input(path, how):
    raise SystemExit(f"{path.relative_to(REPO)} is absent; make it with:\n    {how}")


def _config_inputs():
    """The tree files build/rtti.json and build/tu_map.json are derived from."""
    return (glob.glob(str(REPO / "config" / "arm9" / "overlays" / "*" / "symbols.txt"))
            + glob.glob(str(REPO / "config" / "arm9" / "overlays" / "*" / "delinks.txt"))
            + [str(REPO / "config" / "arm9" / "symbols.txt"),
               str(REPO / "config" / "arm9" / "delinks.txt")])


def stale_input(path, how):
    """Refuse a build/ artifact older than the config it was derived from.

    Measured 2026-09-05: a build/tu_map.json 22 hours behind config/ produced a
    *different* queue -- daStarGate_c came out 1 shard / 457 lines instead of
    19 / 382 -- and both --check and --write reported success. A stale input
    here is a silently wrong answer, not a missing one.
    """
    newest = max((os.path.getmtime(f) for f in _config_inputs()
                  if os.path.exists(f)), default=0)
    if newest > os.path.getmtime(path):
        rel = path.relative_to(REPO)
        raise SystemExit(f"{rel} is older than config/; regenerate it with: {how}")


# --------------------------------------------------------------- class graph
class Graph:
    """Parent edges from the ROM's RTTI, falling back to include/ base clauses.

    RTTI is the strong source but covers 429 classes; 172 of the queue's 275
    class mentions are tree coinages with no RTTI record at all. For those the
    include/ base clause is the only recorded evidence, so the derived cell says
    which source it used and a reader can weigh it.
    """

    def __init__(self):
        if not RTTI.exists():
            missing_input(RTTI, "python tools/rtti_extract.py")
        rt = json.loads(RTTI.read_text(encoding="utf-8"))
        self.rtti_parent = collections.defaultdict(set)
        for e in rt["edges"]:
            self.rtti_parent[e["derived"]].add(e["base"])
        self.rtti_names = {v["name"] for v in rt["records"].values()}

        self.hdr_parent = collections.defaultdict(set)
        for f in glob.glob(str(REPO / "include" / "**" / "*.h"), recursive=True):
            for line in open(f, encoding="utf-8", errors="replace"):
                m = BASE_RE.match(line)
                if m and m.group(1) != m.group(2).split("::")[-1]:
                    self.hdr_parent[m.group(1)].add(m.group(2).split("::")[-1])

    def ancestors(self, cls, seen=frozenset()):
        """({cls} | ancestors, provenance) -- 'rtti', 'header' or 'unknown'."""
        if cls in seen:
            return {cls}, "rtti"
        acc, prov = {cls}, "rtti"
        parents = self.rtti_parent.get(cls)
        if not parents:
            parents = self.hdr_parent.get(cls)
            prov = "header" if parents else None
        if not parents:
            return (acc, "rtti") if cls in self.rtti_names else (acc, "unknown")
        for p in sorted(parents):
            s, pv = self.ancestors(p, seen | {cls})
            acc |= s
            if pv == "unknown":
                prov = "unknown"
            elif pv == "header" and prov != "unknown":
                prov = "header"
        return acc, prov

    def union(self, classes):
        """(chain union, provenance, unresolved classes).

        An unresolved class contributes nothing: it has no RTTI record, so no
        _ZTI/_ZTS to license, and guessing one would put the floor above the
        truth. Excluding it keeps the figure a floor.
        """
        u, prov, unresolved = set(), "rtti", []
        for c in classes:
            s, pv = self.ancestors(c)
            if pv == "unknown":
                unresolved.append(c)
                continue
            if pv != "rtti":
                prov = "header"
            u |= s
        return u, prov, unresolved


# ------------------------------------------------------------------- the tree
class Tree:
    def __init__(self):
        self.sym = collections.defaultdict(list)
        for f in glob.glob(str(REPO / "config" / "arm9" / "overlays" / "*" / "symbols.txt")) \
                + [str(REPO / "config" / "arm9" / "symbols.txt")]:
            if not os.path.exists(f):
                continue
            mod = os.path.basename(os.path.dirname(f))
            for line in open(f, encoding="utf-8", errors="replace"):
                m = SYM_RE.match(line.strip())
                if not m:
                    continue
                sz = re.search(r"size=(0x[0-9a-fA-F]+)", m.group(2))
                self.sym[mod].append(
                    (int(m.group(3), 16), int(sz.group(1), 16) if sz else 4, m.group(1)))
        for k in self.sym:
            self.sym[k].sort()

        self.delink = collections.defaultdict(list)
        for f in glob.glob(str(REPO / "config" / "arm9" / "overlays" / "*" / "delinks.txt")) \
                + [str(REPO / "config" / "arm9" / "delinks.txt")]:
            if not os.path.exists(f):
                continue
            mod = os.path.basename(os.path.dirname(f))
            cur, complete = None, False
            for line in open(f, encoding="utf-8", errors="replace"):
                s = line.strip()
                if s.endswith(":") and s.startswith("src/"):
                    cur, complete = s[:-1], False
                elif s == "complete":
                    complete = True
                elif s.startswith(".text") and cur:
                    m = re.search(r"start:(0x[0-9a-fA-F]+)\s+end:(0x[0-9a-fA-F]+)", s)
                    if m:
                        self.delink[mod].append(
                            (int(m.group(1), 16), int(m.group(2), 16), cur, complete))
        for k in self.delink:
            self.delink[k].sort()

        self.src = {q.replace(os.sep, "/")[len(str(REPO)) + 1:]
                    for q in glob.glob(str(REPO / "src" / "**" / "*.c"), recursive=True)
                    + glob.glob(str(REPO / "src" / "**" / "*.cpp"), recursive=True)}

        if not TU_MAP.exists():
            missing_input(TU_MAP, "python tools/tu_map.py --out build/tu_map.json")
        stale_input(TU_MAP, "python tools/tu_map.py --out build/tu_map.json")
        tm = json.loads(TU_MAP.read_text(encoding="utf-8"))
        self.units = collections.defaultdict(list)
        for mod, m in tm["modules"].items():
            for u in m["units"]:
                self.units[frozenset(u["classes"])].append((mod, u))

        self.promoted, self.manifest_status = set(), {}
        for p in glob.glob(str(REPO / "config" / "tu_manifest.d" / "**" / "*.json"),
                           recursive=True):
            if os.path.basename(p) == "_meta.json":
                continue
            d = json.loads(open(p, encoding="utf-8").read())
            n = len(d.get("compiler_only_output") or []) + len(d.get("externalized_output") or [])
            for c in d["id"].split("/")[-1].split("+"):
                self.manifest_status[c] = (d.get("status"), n)
                if d.get("status") == "promoted":
                    self.promoted.add(c)

    def cover(self, mod, addr):
        for a, b, path, complete in self.delink.get(mod, []):
            if a <= addr < b:
                return path, complete
        return None

    def measure(self, classes):
        """Measure one queue row's TU run, or None when tu_map cannot place it."""
        u = self.units.get(frozenset(classes))
        if not u or len(u) != 1:
            return None
        mod, unit = u[0]
        a, b = int(unit["start"], 16), int(unit["end"], 16)

        # tu_map labels on symbol NAME, so a factory spelled <Class>_classInit
        # rather than _ZN<len><Class>... is never labelled and never lands in
        # the run -- even when it abuts it with a zero-byte gap. Absorb exactly
        # those: the name ties it to a class the row already names, and the
        # zero gap is what the linker's contiguity rule needs.
        absorbed, changed = [], True
        while changed:
            changed = False
            for addr, size, name in self.sym.get(mod, []):
                stem = name.rsplit("_classInit", 1)[0].rsplit("_Spawn", 1)[0]
                if stem == name or stem not in classes or a <= addr < b:
                    continue
                if addr == b:
                    b, changed = addr + size, True
                elif addr + size == a:
                    a, changed = addr, True
                else:
                    continue
                absorbed.append(name)

        srcs, incomplete, sourceless, orphan = set(), set(), [], []
        for addr, _size, name in self.sym.get(mod, []):
            if not (a <= addr < b):
                continue
            hit = self.cover(mod, addr)
            if hit is None:
                byname = [p for p in (f"src/{name}.c", f"src/{name}.cpp") if p in self.src]
                (orphan if byname else sourceless).append(name)
                continue
            path, complete = hit
            srcs.add(path)
            if not complete:
                incomplete.add(path)

        lines = 0
        for p in srcs:
            full = REPO / p
            if full.exists():
                lines += sum(1 for _ in open(full, encoding="utf-8", errors="replace"))

        return {
            "module": mod, "start": hex(a), "end": hex(b), "absorbed": absorbed,
            "run_functions": sum(1 for ad, _s, _n in self.sym.get(mod, []) if a <= ad < b),
            "src_files": len(srcs), "total_lines": lines,
            "unmatched": len(incomplete),
            "no_legacy_source": len(sourceless), "sourceless": sourceless,
            "not_in_delinks": len(orphan), "orphans": orphan,
            "pragma_files": sum(
                1 for p in srcs
                if (REPO / p).exists()
                and any(PRAGMA_RE.match(l) for l in
                        open(REPO / p, encoding="utf-8", errors="replace"))),
            "all_promoted": all(c in self.promoted for c in classes),
        }


def compiler_only_cell(graph, classes, intact):
    u, prov, unresolved = graph.union(classes)
    known = [c for c in classes if c not in unresolved]
    if not known:
        return ("compiler-only:unknown(no chain for " + ",".join(unresolved) +
                ": no RTTI record in build/rtti.json and no include/ base clause; "
                "measure with tubuild verify)")
    floor = 2 * len(u) + len(known) - (3 if intact else 0)
    src = "build/rtti.json" if prov == "rtti" else "build/rtti.json + include/ base clause"
    return (f"compiler-only:>={floor}"
            f"(derived from own chain: {len(u)} classes incl. self via {src}; "
            f"floor 2x{len(u)}+{len(known)}"
            + ("-3 intact-object" if intact else "")
            + (f"; excludes {len(unresolved)} unresolved: {','.join(unresolved)}"
               if unresolved else "") + ")")


NOTES = [
    "# HOW TO READ THIS FILE. Rows whose first field starts with '#' are notes;",
    "# tools/classqueue.py skips them. Re-derive the data rows with:",
    "#     python tools/rtti_extract.py && python tools/tu_map.py --out build/tu_map.json",
    "#     python tools/queue_audit.py --write        (--check in CI, --help for the method)",
    "#",
    "# shard_count is a FLOOR, not a figure. It counts distinct src/ files covering the TU's",
    "# ROM run as tools/tu_map.py cuts it, extended over zero-gap <Class>_classInit factories",
    "# tu_map cannot label because it cuts on symbol NAME. Any OTHER unlabelled neighbour is",
    "# still uncounted. Trust `tubuild.py inspect` over this column.",
    "#",
    "# compiler-only:>=N is DERIVED from the row's own ancestor chain, not copied from",
    "# sibling_oracle -- that copy had no depth check and was wrong in both directions.",
    "# Calibrated on all 108 promoted manifests: exact 45 times, an UNDER-count 37 times, and",
    "# never an over-count on the text-only route; 3 lower on intact-object (16/16). It does",
    "# NOT apply when the key function falls outside the licensed range -- then the real count",
    "# is 0, measured on dScMgBase_c, dScMgHanachan_c and dScMgBomroom_c -- and every Vector3",
    "# the TU odr-uses, function locals included, adds a row the chain cannot predict.",
    "#",
    "# pragma:N IS NOT A BLOCKER. It counts shards in the run carrying a #pragma line, nothing",
    "# more. '#pragma defer_codegen off' makes positional brackets bind and took dScMgHanachan_c",
    "# from 22/61 to 49/61 (PR #2309), at the cost of rewriting the TU ROM-ascending in the same",
    "# edit. A pragma count means something only after a DELETE-OUTRIGHT control shows the bytes",
    "# move: dScMgRoulette_c's pragmas were inert and it scored 40/40 with them deleted.",
    "#",
    "# unmatched:N        delink entries in the run with no 'complete' marker.",
    "#                    The NAME MISLEADS: this is a LINK condition, not a matching one.",
    "#                    Measured on ov066/Eyerok all six byte-MATCHed; they lack the marker",
    "#                    because they reference cross-overlay addresses under names no",
    "#                    symbols.txt gives, so dsd drops them from the link.",
    "# no-legacy-source:N functions with no delinks entry AND no src/<symbol>.c[pp] at all. The",
    "#                    cartridge's own bytes cover them, and a licensed claim cannot have a",
    "#                    hole, so such a run can only be taken as one of its two contiguous",
    "#                    sides. A function that HAS a src/ file but no delinks entry is NOT",
    "#                    sourceless: that is not-in-delinks, a milder and separate condition.",
    "# already_promoted   every named class has a config/tu_manifest.d entry reading",
    "#                    status: promoted. Goes stale on every promotion; re-derive, never edit.",
    "#",
    "# sibling_oracle is a SHAPE reference only. Copying its counts is what made this file wrong.",
]


def run(write, check):
    graph, tree = Graph(), Tree()
    with QUEUE.open(newline="", encoding="utf-8") as fh:
        rd = csv.DictReader(fh, delimiter="\t")
        fields, all_rows = rd.fieldnames, list(rd)
    rows = [r for r in all_rows if not (r[fields[0]] or "").lstrip().startswith("#")]

    changed = collections.Counter()
    for row in rows:
        classes = [c.strip() for c in row["class_name"].split("+") if c.strip()]
        m = tree.measure(classes)
        blockers = split_blockers(row["blockers"])

        if m is not None:
            want = "yes" if m["all_promoted"] else "no"
            if row["already_promoted"] != want:
                row["already_promoted"] = want
                changed["already_promoted"] += 1
            if int(row["shard_count"]) != m["src_files"]:
                row["shard_count"] = str(m["src_files"])
                changed["shard_count"] += 1
            # total_lines is measured independently of shard_count. It used to be
            # nested inside the test above, so a row whose line count moved while
            # its shard count held was never corrected and never counted -- which
            # is why the two counters always reported the same figure. At the time
            # of the fix that hid 29 stale rows behind the 4 the gate reported,
            # with deltas from -51 to +95.
            if int(row["total_lines"]) != m["total_lines"]:
                row["total_lines"] = str(m["total_lines"])
                changed["total_lines"] += 1
            # Omitting the token was never a claim of zero: of 108 promoted
            # manifests only two carry no compiler_only_output, both because
            # the key function fell outside the licensed range.
            if not any(b.startswith("compiler-only") for b in blockers):
                blockers.append("compiler-only:PLACEHOLDER")

        out = []
        for b in blockers:
            if b.startswith("compiler-only"):
                cell = compiler_only_cell(graph, classes,
                                          row["promotion_route"] == "intact-object")
                if cell != b:
                    changed["compiler-only"] += 1
                out.append(cell)
                continue
            for key, field in (("no-legacy-source", "no_legacy_source"),
                               ("unmatched", "unmatched")):
                if re.match(re.escape(key) + r":(\d+)$", b) and m is not None:
                    n = m[field]
                    if b != f"{key}:{n}":
                        changed[key] += 1
                    if n:
                        out.append(f"{key}:{n}")
                    break
            else:
                out.append(b)

        if m is not None:
            have = {re.split(r"[:(]", b)[0] for b in out}
            for key, field in (("no-legacy-source", "no_legacy_source"),
                               ("unmatched", "unmatched"),
                               ("not-in-delinks", "not_in_delinks")):
                if m[field] and key not in have and not any(
                        b.startswith("classif:" + key) for b in out):
                    out.append(f"{key}:{m[field]}")
                    changed[key] += 1
        row["blockers"] = ";".join(out) if out else "-"

    stale = sum(changed.values())
    for k, v in sorted(changed.items()):
        print(f"  {k}: {v} row(s) disagree with the tree")
    if not stale:
        print("queue_audit: queue agrees with the tree")

    if write:
        with QUEUE.open("w", newline="", encoding="utf-8") as fh:
            w = csv.DictWriter(fh, fieldnames=fields, delimiter="\t", lineterminator="\n")
            w.writeheader()
            for line in NOTES:
                fh.write(line + "\n")
            for row in rows:
                w.writerow(row)
        print(f"queue_audit: wrote {QUEUE.relative_to(REPO)}")
        return 0
    if check and stale:
        print("queue_audit: FAIL -- run 'python tools/queue_audit.py --write'")
        return 1
    return 0


def main():
    p = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("--write", action="store_true", help="rewrite the queue in place")
    p.add_argument("--check", action="store_true", help="exit 1 if the queue is stale")
    args = p.parse_args()
    return run(args.write, args.check)


if __name__ == "__main__":
    sys.exit(main())
