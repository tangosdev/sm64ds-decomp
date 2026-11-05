#!/usr/bin/env python3
"""Map particle effect IDs to the call sites that spawn them.

Scans src/ for `Particle::System::NewSimple(id, ...)` (and its mangled
`_ZN8Particle6System9NewSimpleE...` spelling), resolves an identifier argument
through a same-file `NAME = 0x..` / `#define NAME 0x..` / `const ... NAME = ..`,
and joins the result with the SPA table from tools/spa_dump.py.

For every ID it reports the call count, where it is called (file:line and the
nearest preceding `// @symbol`), any source name already given to it, and the
definition's flags / inferred texture / records.  IDs in the table that no
`NewSimple` literal reaches are listed separately.

Static only: a literal in source does not prove the call is reachable, and
`System::New`, the fixed-effect wrappers and computed IDs are not counted.

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

CALL = re.compile(r"(?:NewSimple|_ZN8Particle6System9NewSimpleE\w*)\s*\(\s*([^,)]+)")
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


def scan(root):
    sites = []  # dicts
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
            if "NewSimple" not in text:
                continue
            consts = constants(text)
            sym = None
            for n, line in enumerate(text.splitlines(), 1):
                s = SYMBOL.search(line)
                if s:
                    sym = s.group(1)
                for m in CALL.finditer(line):
                    arg = m.group(1).strip()
                    name = comment = None
                    if NUM.match(arg):
                        val = int(arg, 0)
                    elif arg in consts and CONSTNAME.match(arg):
                        val, comment = consts[arg]
                        name = arg
                    else:
                        continue  # variable / declaration
                    sites.append({"id": val, "name": name, "comment": comment,
                                  "file": os.path.relpath(path, root).replace("\\", "/"),
                                  "line": n, "symbol": sym})
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
              "flags 0x%08x texture %d(inferred) %s %s" % (
                  d["flags"], d["texture"], ",".join(d["optional"]) or "-", ",".join(d["behaviors"]) or "-")))
        for s in by.get(a.id, []):
            print("  %s:%d  %s%s%s" % (s["file"], s["line"], s["symbol"] or "?",
                  "  name=" + s["name"] if s["name"] else "",
                  "  /* %s */" % s["comment"] if s["comment"] else ""))
        if a.id not in by:
            print("  no NewSimple literal in src/")
        return

    if a.unused:
        ids = [i for i in sorted(defs) if i not in by]
        print("%d of %d table IDs are never passed to NewSimple as a literal:" % (len(ids), len(defs)))
        print(" ".join("0x%x" % i for i in ids))
        return

    named = sum(1 for v in by.values() if any(s["name"] for s in v))
    print("%d distinct IDs from %d call sites; %d carry a source name; table has %d" % (
        len(by), sum(len(v) for v in by.values()), named, len(defs)))
    print("%-6s %-5s %-4s %-26s %s" % ("id", "calls", "tex?", "name", "first site"))
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
