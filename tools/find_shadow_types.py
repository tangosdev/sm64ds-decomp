#!/usr/bin/env python3
"""Find locally coined types that stand in for something the tree already has.

WHY THIS EXISTS
---------------
Two patterns accumulate in a file that was matched function by function, and
both read as if the author discovered a new type when they were really working
around a name they did not have yet.

**POD twins.** A struct whose entire body is `{int x, y, z;}` is a `Vector3`;
`{s16 x, y, z;}` is a `Vector3_16` or a `Vector3_16f`. `include/types.h`
records the one real reason to coin one -- declaring a constructor on `Vector3`
flips a whole-struct copy from a block move to memberwise, so "those sites use
a local POD twin instead" -- but that reason has to be measured per site, and
the pattern spreads far past it. Player.cpp had 32 of them under 17 names, and
7 shadowed a real type **by its own name**: inside those bodies `Vector3` did
not mean `types.h`'s `Vector3`, and nothing said so. One function had to spell
`::Vector3` to reach past its own shadow.

**Vtable-slot scaffolds.** A class whose virtuals are all placeholders
(`v00`, `m04`, `Virtual10`) exists to pad to a slot index so a call can be
cast through it. The slot usually already has a name in `include/`: five such
scaffolds in Player.cpp were all reaching `dActor_c::OnYoshiTryEat` (slot 18),
under three different invented names plus a raw function-pointer table.

This tool only locates candidates. **Whether a candidate can change is a
codegen question, and the answer is often no** -- a non-POD element can add a
compiler-numbered destructor symbol, a POD aggregate initializer can be the
ROM's lowering, and a twin can be what keeps a value in registers. Measure
each one with `tools/tu_object_diff.py` before touching it, and record the
measurement next to any you keep.

Comments are stripped before scanning. A commented-out `TUBUILD CONFLICT`
block in d_a_demo.cpp made a plain grep report a scaffold in a file that has
none, which is how this ended up being a tool instead of a regex.

USAGE
-----
    python tools/find_shadow_types.py src/game/player/Player.cpp
    python tools/find_shadow_types.py src/game/player/Player.cpp --twins --format json
    git ls-files 'src/**/*.cpp' | python tools/find_shadow_types.py --files-from -

Always exits 0: this is a survey, not a gate.
"""
import argparse
import json
import pathlib
import re
import sys

# A body that is only scalar members. Kept narrow on purpose: a struct with a
# method, a bitfield or a nested type is not what this is looking for.
SCALAR = r"(?:int|short|long|char|float|s8|s16|s32|u8|u16|u32|unsigned\s+\w+|signed\s+\w+)"
BLOCK = re.compile(r"(?:volatile\s+)?(?:typedef\s+)?struct\s+(\w*)\s*\{([^{}]*)\}\s*(\w*)\s*;")

# Placeholder virtual names: v00, m04, V18, Virtual10, vfunc3, unk12, func8
PLACEHOLDER = re.compile(r"^(?:[vmVM]|vf|vfunc|unk|unknown|func|Virtual|virtual)_?\d+$")
VIRTUAL = re.compile(r"\bvirtual\b[^;{]*?\b(\w+)\s*\(")

# Real types a triple stands in for, by member type.
TWIN_OF = {
    "int": "Vector3", "s32": "Vector3", "long": "Vector3",
    "short": "Vector3_16 or Vector3_16f", "s16": "Vector3_16 or Vector3_16f",
    "u16": "Vector3_16 or Vector3_16f",
    "float": "Vec3 (float) -- check what the callee wants",
}


def strip_comments(text):
    """Remove // and /* */ comments, preserving line count and string literals."""
    out, i, n = [], 0, len(text)
    while i < n:
        two = text[i:i + 2]
        if two == "//":
            end = text.find("\n", i)
            if end < 0:
                break
            out.append(text[i:end].count("\n") * "\n")
            i = end
        elif two == "/*":
            end = text.find("*/", i + 2)
            end = n if end < 0 else end + 2
            out.append("\n" * text.count("\n", i, end))
            i = end
        elif text[i] in "\"'":
            quote, j = text[i], i + 1
            while j < n and text[j] != quote:
                j += 2 if text[j] == "\\" else 1
            out.append(text[i:min(j + 1, n)])
            i = min(j + 1, n)
        else:
            out.append(text[i])
            i += 1
    return "".join(out)


def members(body):
    """Return [(type, name)] when the body is only scalar declarations, else None."""
    found = []
    for statement in [s.strip() for s in body.split(";") if s.strip()]:
        m = re.fullmatch(r"(" + SCALAR + r")\s+(.+)", statement, re.S)
        if not m:
            return None
        kind = re.sub(r"\s+", " ", m.group(1))
        for name in m.group(2).split(","):
            name = name.strip()
            if not re.fullmatch(r"\w+", name):
                return None
            found.append((kind, name))
    return found


def find_twins(text, path="?"):
    """Structs whose whole body is an x/y/z triple of one scalar type."""
    clean = strip_comments(text)
    out = []
    for m in BLOCK.finditer(clean):
        got = members(m.group(2))
        if not got or len(got) != 3:
            continue
        if [n for _, n in got] != ["x", "y", "z"]:
            continue
        kinds = {k for k, _ in got}
        if len(kinds) != 1:
            continue
        kind = kinds.pop()
        name = m.group(1) or m.group(3)
        real = TWIN_OF.get(kind)
        if not real:
            continue
        out.append(dict(file=path, line=clean[:m.start()].count("\n") + 1,
                        kind="pod-twin", name=name or "(anonymous)", member_type=kind,
                        stands_for=real, volatile="volatile" in m.group(0),
                        shadows_real_name=name in ("Vector3", "Vector3_16", "Vector3_16f")))
    return out


def find_scaffolds(text, path="?", threshold=2):
    """Classes whose virtuals are mostly placeholder names padding to a slot."""
    clean = strip_comments(text)
    out = []
    for m in re.finditer(r"\b(?:struct|class)\s+(\w+)[^;{]*\{", clean):
        depth, i, n = 0, m.end() - 1, len(clean)
        while i < n:
            if clean[i] == "{":
                depth += 1
            elif clean[i] == "}":
                depth -= 1
                if depth == 0:
                    break
            i += 1
        body = clean[m.end():i]
        names = VIRTUAL.findall(body)
        if not names:
            continue
        placeholders = [x for x in names if PLACEHOLDER.match(x)]
        if len(placeholders) < threshold:
            continue
        out.append(dict(file=path, line=clean[:m.start()].count("\n") + 1,
                        kind="vtable-scaffold", name=m.group(1),
                        virtuals=len(names), placeholders=len(placeholders),
                        named=[x for x in names if not PLACEHOLDER.match(x)],
                        reaches_slot=len(names) - 1))
    return out


def scan(paths, want_twins=True, want_scaffolds=True, threshold=2):
    findings = []
    for name in paths:
        path = pathlib.Path(name)
        try:
            text = path.read_text(encoding="utf-8", errors="replace")
        except OSError as exc:
            print("%s: cannot read (%s)" % (name, exc), file=sys.stderr)
            continue
        rel = path.as_posix()
        if want_twins:
            findings += find_twins(text, rel)
        if want_scaffolds:
            findings += find_scaffolds(text, rel, threshold)
    findings.sort(key=lambda f: (f["file"], f["line"]))
    return findings


def report(findings):
    twins = [f for f in findings if f["kind"] == "pod-twin"]
    scaffolds = [f for f in findings if f["kind"] == "vtable-scaffold"]
    shadowing = [f for f in twins if f["shadows_real_name"]]

    if twins:
        print("POD twins (%d):" % len(twins))
        for f in twins:
            flags = []
            if f["shadows_real_name"]:
                flags.append("SHADOWS THE REAL NAME")
            if f["volatile"]:
                flags.append("volatile -- probably a codegen pin, leave alone")
            print("  %s:%d  %-14s {%s x,y,z} -> %s%s"
                  % (f["file"], f["line"], f["name"], f["member_type"], f["stands_for"],
                     "  [" + "; ".join(flags) + "]" if flags else ""))
    if scaffolds:
        print("Vtable-slot scaffolds (%d):" % len(scaffolds))
        for f in scaffolds:
            extra = ("  named: " + ", ".join(f["named"])) if f["named"] else ""
            print("  %s:%d  %-14s %d virtuals, %d placeholder -- reaches slot %d%s"
                  % (f["file"], f["line"], f["name"], f["virtuals"], f["placeholders"],
                     f["reaches_slot"], extra))

    files = len({f["file"] for f in findings})
    print("find-shadow-types: %d candidate(s) in %d file(s) -- %d POD twin(s) (%d shadowing a "
          "real name), %d scaffold(s)"
          % (len(findings), files, len(twins), len(shadowing), len(scaffolds)))
    if findings:
        print("  these are CANDIDATES: measure each with tools/tu_object_diff.py first")


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("files", nargs="*")
    ap.add_argument("--files-from", metavar="PATH",
                    help="read paths from a file, or '-' for stdin. A tree-wide sweep "
                         "does not fit in one command line (2,400+ sources), and xargs "
                         "splits it into several runs, which breaks --format json")
    ap.add_argument("--twins", action="store_true", help="only report POD twins")
    ap.add_argument("--scaffolds", action="store_true", help="only report vtable scaffolds")
    ap.add_argument("--threshold", type=int, default=2,
                    help="placeholder virtuals needed to call a class a scaffold (default 2)")
    ap.add_argument("--format", choices=("text", "json"), default="text")
    args = ap.parse_args(argv)

    paths = list(args.files)
    if args.files_from:
        stream = sys.stdin if args.files_from == "-" else open(args.files_from, encoding="utf-8")
        try:
            paths += [line.strip() for line in stream if line.strip()]
        finally:
            if stream is not sys.stdin:
                stream.close()
    if not paths:
        ap.error("no input files: pass paths, or --files-from PATH")

    both = not (args.twins or args.scaffolds)
    findings = scan(paths, want_twins=both or args.twins,
                    want_scaffolds=both or args.scaffolds, threshold=args.threshold)
    if args.format == "json":
        print(json.dumps(findings, indent=1))
    else:
        report(findings)
    return 0


if __name__ == "__main__":
    sys.exit(main())
