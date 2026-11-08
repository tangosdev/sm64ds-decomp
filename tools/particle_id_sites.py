#!/usr/bin/env python3
"""Map particle effect IDs to the call sites that spawn them.

Scans src/ for every entry point that takes an effect ID: `NewSimple(id, ...)`,
`System::New(uniqueID, id, ...)`, `NewWeather`, `NewCleanParticle` and the
`NewCheckWater`/`NewCheckWaterRipple`/`NewFitWater`/`NewFitWaterRaised`/
`NewCheckYogan` callback wrappers (also under their old names,
`NewUnkCallback818` and `func_02022d44/d00/cbc/c80/c3c`), in plain and
mangled spellings.  Resolves an identifier argument through
include/ParticleIds.h or a same-file `NAME = 0x..` / `#define NAME 0x..` /
`const ... NAME = ..`,
and joins the result with the SPA table from tools/spa_dump.py.

For every ID it reports the call count, where it is called (file:line and the
enclosing definition's linker symbol), any source name already given to it, and the
definition's flags / texture / records.  IDs in the table that no
`NewSimple` literal reaches are listed separately.

Static only: a literal in source does not prove the call is reachable, and
computed IDs (variables) are not counted.  The fixed-ID wrappers (NewRipple,
NewBigSplash, RunningSlidingDustAt, ...) hard-wire their IDs inside their own
bodies, so those show up as `System::New` sites in src/engine/particle/.

Usage:
  python tools/particle_id_sites.py               # table
  python tools/particle_id_sites.py --id 0xb3     # one ID, every site
  python tools/particle_id_sites.py --unused      # table IDs never named by a literal
  python tools/particle_id_sites.py --json out.json
"""
import argparse
import json
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import spa_dump  # noqa: E402
import def_locate as DL  # noqa: E402

# (regex for the callee, index of the effect-ID argument, label).  NewSimple takes
# the ID first; System::New and the callback wrappers take (uniqueID, effectID, ...).
ENTRY = [
    (re.compile(r"(?:Particle::System::NewSimple|_ZN8Particle6System9NewSimpleE\w*|\bNewSimple)\s*\("), 0, "NewSimple"),
    (re.compile(r"(?:Particle::System::New|System::New|_ZN8Particle6System3NewE\w*)\s*\("), 1, "System::New"),
    (re.compile(r"(?:Particle::System::NewWeather|_ZN8Particle6System10NewWeatherE\w*)\s*\("), 1, "NewWeather"),
    (re.compile(r"(?:\bNewCleanParticle|\bNewUnkCallback818|_ZN8Particle6System1[67]New(?:CleanParticle|UnkCallback818)E\w*)\s*\("), 1, "NewCleanParticle"),
    (re.compile(r"\b(NewCheckWater(?:Ripple)?|NewFitWater(?:Raised)?|NewCheckYogan|"
                r"func_02022d44|func_02022d00|func_02022cbc|func_02022c80|func_02022c3c)\s*\("), 1, "callback wrapper"),
]


def split_args(text, i):
    """Arguments of the call whose '(' is at text[i-1], paren-aware; None if unterminated."""
    depth, cur, args = 1, [], []
    while i < len(text) and depth:
        c = text[i]
        if c in "([{":
            depth += 1
        elif c in ")]}":
            depth -= 1
            if depth == 0:
                break
        if c == "," and depth == 1:
            args.append("".join(cur).strip())
            cur = []
        else:
            cur.append(c)
        i += 1
    if depth:
        return None
    args.append("".join(cur).strip())
    return args
NUM = re.compile(r"^(?:0[xX][0-9a-fA-F]+|\d+)$")
SYMBOL = re.compile(r"//\s*@symbol\s+(\S+)")
SRC_EXT = (".c", ".cpp", ".h")
# Only treat kFoo / FOO_BAR as constants; `id = 0x48` assignments are variables.
CONSTNAME = re.compile(r"^(?:k[A-Z]\w*|[A-Z][A-Z0-9_]+)$")


def constants(text):
    """name -> (value, comment) for same-file integer constants."""
    out = {}
    pats = [
        re.compile(r"\b([A-Za-z_]\w*)\s*=\s*(0[xX][0-9a-fA-F]+|\d+)\s*[,;}]?[ \t]*(?:/\*\s*(.*?)\s*\*/|//\s*(.*))?$", re.M),
        re.compile(r"#define\s+([A-Za-z_]\w*)\s+(0[xX][0-9a-fA-F]+|\d+)[ \t]*(?:/\*\s*(.*?)\s*\*/|//\s*(.*))?$", re.M),
    ]
    for p in pats:
        for m in p.finditer(text):
            out[m.group(1)] = (int(m.group(2), 0), (m.group(3) or m.group(4) or "").strip())
    return out


def header_constants(root):
    """The shared effect-ID names (include/ParticleIds.h), beside root=src/."""
    path = os.path.join(root, os.pardir, "include", "ParticleIds.h")
    try:
        return constants(open(path, encoding="utf-8").read())
    except OSError:
        return {}


def scan(root):
    sites = []  # dicts
    shared = header_constants(root)
    for dp, dn, fn in os.walk(root):
        dn[:] = [d for d in dn if d not in (".git", ".claude")]
        for f in fn:
            if not f.endswith(SRC_EXT):
                continue
            path = os.path.join(dp, f)
            try:
                text = open(path, encoding="utf-8", errors="replace").read()
            except OSError:
                continue
            if "Particle" not in text and "func_02022" not in text and "NewSimple" not in text:
                continue
            consts = dict(shared)
            consts.update(constants(text))  # a file-local name wins
            lines_start = [0]
            for ln in text.splitlines(True):
                lines_start.append(lines_start[-1] + len(ln))
            symbols = [(m.start(), m.group(1)) for m in SYMBOL.finditer(text)]
            # the enclosing definition's linker symbol -- what "nearest
            # preceding // @symbol" reported, resolved structurally so it
            # survives the markers' removal.
            try:
                rel = os.path.relpath(path, root).replace("\\", "/")
                if not rel.startswith("src"):
                    rel = "src/" + rel
                bound = DL.bind(rel, text)[0]
            except Exception:
                bound = {}
            spans = []   # (def_start, def_end, symbol)
            for sym, d in bound.items():
                if not d.is_object:
                    spans.append((d.start, d.end, sym))
            spans.sort()
            import bisect
            for rx, idx, label in ENTRY:
                for m in rx.finditer(text):
                    args = split_args(text, m.end())
                    if not args or len(args) <= idx:
                        continue
                    arg = args[idx]
                    name = comment = None
                    if NUM.match(arg):
                        val = int(arg, 0)
                    elif arg in consts and CONSTNAME.match(arg):
                        val, comment = consts[arg]
                        name = arg
                    else:
                        continue  # variable / declaration / definition
                    n = bisect.bisect_right(lines_start, m.start())
                    owner = None
                    k = bisect.bisect_right([s0 for s0, _, _ in spans],
                                            m.start()) - 1
                    while k >= 0:
                        if m.start() < spans[k][1]:
                            owner = spans[k][2]
                            break
                        k -= 1
                    if owner is None and symbols:
                        k = bisect.bisect_right(
                            [p0 for p0, _ in symbols], m.start()) - 1
                        if k >= 0:
                            owner = symbols[k][1]
                    sites.append({"id": val, "name": name, "comment": comment, "via": label,
                                  "file": os.path.relpath(path, root).replace("\\", "/"),
                                  "line": n, "symbol": owner})
    return sites


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    repo = os.path.join(here, "..")
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--src", default=os.path.join(repo, "src"))
    ap.add_argument("--arm9", default=os.path.join(repo, "extracted", "arm9_dec.bin"))
    ap.add_argument("--id", type=lambda s: int(s, 0))
    ap.add_argument("--unused", action="store_true")
    ap.add_argument("--json")
    a = ap.parse_args()

    table = spa_dump.parse(open(a.arm9, "rb").read())
    defs = {d["id"]: d for d in table["definitions"]}
    by = {}
    for s in scan(a.src):
        by.setdefault(s["id"], []).append(s)

    if a.json:
        out = [{"id": i, "calls": by.get(i, []), "definition": {
            k: defs[i][k] for k in ("flags", "texture", "optional", "behaviors", "size")} if i in defs else None}
            for i in sorted(set(by) | set(defs))]
        json.dump(out, open(a.json, "w"), indent=1)
        print("wrote %s" % a.json)
        return

    if a.id is not None:
        d = defs.get(a.id)
        print("effect 0x%x (%d): %s" % (a.id, a.id, "no definition (out of table)" if not d else
              "flags 0x%08x texture %d %s %s" % (
                  d["flags"], d["texture"], ",".join(d["optional"]) or "-", ",".join(d["behaviors"]) or "-")))
        for s in by.get(a.id, []):
            print("  %s:%d  [%s] %s%s%s" % (s["file"], s["line"], s["via"], s["symbol"] or "?",
                  "  name=" + s["name"] if s["name"] else "",
                  "  /* %s */" % s["comment"] if s["comment"] else ""))
        if a.id not in by:
            print("  no literal effect ID in src/")
        return

    if a.unused:
        ids = [i for i in sorted(defs) if i not in by]
        print("%d of %d table IDs are never passed as a literal to any entry point:" % (len(ids), len(defs)))
        print(" ".join("0x%x" % i for i in ids))
        return

    named = sum(1 for v in by.values() if any(s["name"] for s in v))
    via = {}
    for v in by.values():
        for x in v:
            via[x["via"]] = via.get(x["via"], 0) + 1
    print("%d distinct IDs from %d call sites (%s); %d carry a source name; table has %d" % (
        len(by), sum(len(v) for v in by.values()),
        ", ".join("%s %d" % kv for kv in sorted(via.items())), named, len(defs)))
    print("%-6s %-5s %-4s %-26s %s" % ("id", "calls", "tex", "name", "first site"))
    for i in sorted(by):
        v = by[i]
        names = sorted({s["name"] for s in v if s["name"]})
        d = defs.get(i)
        print("0x%03x  %-5d %-4s %-26s %s:%d %s%s" % (
            i, len(v), d["texture"] if d else "-", ",".join(names) or "-",
            v[0]["file"], v[0]["line"], v[0]["symbol"] or "",
            "" if i in defs else "  (NOT IN TABLE)"))


if __name__ == "__main__":
    main()
