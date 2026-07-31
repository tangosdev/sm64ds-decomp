"""PR-scoped link verification: resolve the symbols a PR's changed src files
define, then run linkcheck's explicit (ledger-bypassing) mode on each.

WHY THIS EXISTS
---------------
linkcheck.py's corpus and --name modes only inspect functions with a
progress/matched.jsonl row, so compiler-emitted passengers with no ledger row --
this-adjusting thunks (_ZThn..._) and the weak dtor/ctor copies emitted alongside
a class definition -- are never checked. That gap let PR #86's
_ZThn80_N9AnimationD1Ev pass local verify and fail review with WRONG-DEST (its
tail branch relocated to Animation::~Animation when the ROM slot needs
ModelAnim2::~ModelAnim2). This tool closes the gap by scope: it takes the exact
files a PR touched, maps each filename to the symbol it defines (in this repo the
src filename IS the mangled symbol), resolves that symbol's (addr, size, module)
from the checked-in config/**/symbols.txt, and runs linkcheck on every slot.
linkcheck's explicit --addr/--size/--module mode needs no ledger row and returns
WRONG for a wrong-dest thunk (verified: is_benign forgives a branch diff only when
the ROM's veneer/twin resolves to the exact address the source names).

HEADER FAN-OUT
--------------
Since shared headers landed (#859, #860) a PR can change the codegen of a file it never
names: edit a field width in `include/types.h` and every consumer's bytes move, while the
diff lists only the header. Checking the literal diff would let that PR go green with
matched functions broken behind it -- so a changed header is expanded through the reverse
include graph (`affected_src.py`) and every consumer is checked too. Any caller that hands
this tool a file list must therefore pass changed headers as well as changed sources.

Usage:
  python tools/pr_linkcheck.py                      # local branch vs origin/main
  python tools/pr_linkcheck.py --base main          # vs a different base ref
  python tools/pr_linkcheck.py --pr 86              # a GitHub PR (needs gh)
  python tools/pr_linkcheck.py --files src/a.c src/b.cpp   # explicit file list
  python tools/pr_linkcheck.py --files include/types.h     # expands to its consumers
  python tools/pr_linkcheck.py --json out.json --fail       # CI mode
"""
import argparse
import glob
import json
import pathlib
import re
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import affected_src as A  # noqa: E402
import linkcheck as LC  # noqa: E402

SRC_SUFFIXES = (".c", ".cpp")
HDR_SUFFIXES = (".h", ".hpp")

# name -> kind:function(...size=0xNN) ... addr:0xNNNN  (RE tolerant of extra attrs)
_SYM_RE = re.compile(r"^(\S+)\s+kind:function\S*?size=0x([0-9a-fA-F]+)\).*?addr:0x([0-9a-fA-F]+)")


def module_of(symbols_path: str) -> str:
    """dsd config path -> the module string linkcheck/reverify expect."""
    s = symbols_path.replace("\\", "/")
    m = re.search(r"/overlays/(ov\d+)/", s)
    if m:
        return m.group(1)
    if "/itcm/" in s:
        return "itcm"
    if "/dtcm/" in s:
        return "dtcm"
    return "arm9"  # config/arm9/symbols.txt == the main module (ledger calls it arm9)


def build_symbol_index():
    """symbol name -> [(addr, size, module), ...] from every checked-in symbols.txt.

    Names are not unique: overlays reuse them and identical thunks repeat within a
    module, so every slot is kept and checked."""
    idx = {}
    for sf in glob.glob(str(REPO / "config" / "**" / "symbols.txt"), recursive=True):
        mod = module_of(sf)
        for line in open(sf, encoding="utf-8"):
            m = _SYM_RE.match(line.strip())
            if m:
                idx.setdefault(m.group(1), []).append(
                    (int(m.group(3), 16), int(m.group(2), 16), mod))
    return idx


def changed_src_files(args):
    """Every compiled file the PR/branch puts at risk.

    That is its added/modified `src/*.c|*.cpp` PLUS every source that transitively
    includes one of its added/modified `include/**/*.h|*.hpp` -- see HEADER FAN-OUT
    above. A header with no consumer yet expands to nothing, which is correct: there
    is no compiled artifact to check."""
    if args.files:
        paths = args.files
    elif args.pr:
        out = subprocess.run(
            ["gh", "pr", "view", str(args.pr), "--json", "files",
             "-q", ".files[].path"],
            cwd=REPO, capture_output=True, text=True, check=True).stdout
        paths = out.splitlines()
    else:
        out = subprocess.run(
            ["git", "diff", "--name-only", "--diff-filter=AM", f"{args.base}...HEAD"],
            cwd=REPO, capture_output=True, text=True, check=True).stdout
        paths = out.splitlines()
    keep, headers = [], []
    for p in paths:
        p = p.strip().replace("\\", "/")
        if p.startswith("src/") and p.endswith(SRC_SUFFIXES):
            keep.append(p)
        elif p.startswith("include/") and p.endswith(HDR_SUFFIXES):
            headers.append(p)
    if not keep and not headers:
        return []
    expanded = A.affected_sources(keep + headers)
    if headers:
        extra = len(expanded) - len(set(keep) & set(expanded))
        print(f"{len(headers)} changed header(s) fan out to {extra} additional "
              f"source file(s); checking {len(expanded)} in total\n")
    return expanded


def _resolve(name, idx, ledger):
    """(addr, size, module) slots for a symbol: the config symbol index first (a
    superset of the ledger -- it carries thunks and weak dtor/ctor copies), then the
    ledger for module disambiguation when a name is banked but absent from config."""
    slots = idx.get(name)
    if not slots and name in ledger:
        e = ledger[name]
        slots = [(e["addr"], e["size"], e["module"])]
    return slots


def check_file(path, idx, ledger):
    """Link-check every symbol the file compiles to -- the named function AND its
    compiler-emitted passengers (this-adjusting thunks, weak dtor/ctor copies, local
    helpers) -- not just the filename stem, and not just ledger rows.

    The file is compiled once; every emitted function symbol that resolves to a ROM
    slot (via config/ledger) is checked against that slot. This is the class the
    ledger-scoped checks miss -- e.g. PR #86's `_ZThn80_N9AnimationD1Ev` thunk, whose
    tail branch relocated to the wrong dtor. Emitted symbols with no ROM slot (inline
    or local emissions) are simply not ROM functions and are ignored."""
    sym = pathlib.Path(path).stem
    slots = _resolve(sym, idx, ledger)
    if not slots:
        return {"file": path, "symbol": sym, "results": [], "note": "unresolved"}

    # Compile once (winning version/flags for the named symbol) so the named function
    # and its passengers are read from the very same object the ROM was matched with.
    import reloc_audit as RA
    obj = wsym = None
    for addr, size, mod in slots:
        obj, wsym, _ = RA.winning_object(sym, addr, size, mod)
        if obj is not None:
            break

    results = []
    for addr, size, mod in slots:
        r = LC.linkcheck(sym, addr, size, mod, _NAME_INDEX,
                         obj=obj, sym=(wsym if obj is not None else None))
        results.append({"sym": sym, "addr": f"0x{addr:08x}", "module": mod,
                        "verdict": r["verdict"], "diffs": r.get("diffs", []),
                        "passenger": False})

    # Full-file: every OTHER function symbol this object emits that also owns a ROM
    # slot. Checked straight out of the object (a thunk has no source file of its own).
    if obj is not None:
        import probe_versions as PV
        try:
            emitted = set(PV.funcs_in(obj).keys())
        except Exception:
            emitted = set()
        for psym in sorted(emitted - {sym, wsym}):
            pslots = _resolve(psym, idx, ledger)
            if not pslots:
                continue  # emitted symbol with no ROM slot -- normal, nothing to check
            for addr, size, mod in pslots:
                r = LC.linkcheck(psym, addr, size, mod, _NAME_INDEX, obj=obj, sym=psym)
                results.append({"sym": psym, "addr": f"0x{addr:08x}", "module": mod,
                                "verdict": r["verdict"], "diffs": r.get("diffs", []),
                                "passenger": True})
    return {"file": path, "symbol": sym, "results": results, "note": ""}


def worst(results):
    order = ["WRONG", "NO-REPRO", "NO-SYM", "BLIND", "BENIGN", "VERIFIED"]
    verdicts = [x["verdict"].split("-")[0] if x["verdict"].startswith("BLIND")
                else x["verdict"] for x in results]
    for v in order:
        if v in verdicts:
            return v
    return "NONE"


_NAME_INDEX = None


def main():
    global _NAME_INDEX
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--base", default="origin/main", help="base ref for git diff")
    ap.add_argument("--pr", type=int, default=None, help="GitHub PR number (uses gh)")
    ap.add_argument("--files", nargs="*", help="explicit file list instead of a diff")
    ap.add_argument("--json", default=None)
    ap.add_argument("--md", default=None, help="write a markdown report (for PR comments)")
    ap.add_argument("--fail", action="store_true",
                    help="exit 1 if any file is WRONG-DEST or a non-reproducing near-miss")
    args = ap.parse_args()

    import reloc_audit as RA
    _NAME_INDEX = RA.build_name_index()
    idx = build_symbol_index()
    ledger = {}
    mp = REPO / "progress" / "matched.jsonl"
    if mp.exists():
        for l in mp.read_text().splitlines():
            if l.strip():
                e = json.loads(l)
                a = e["addr"]
                ledger[e["name"]] = {
                    "addr": int(a, 16) if isinstance(a, str) else a,
                    "size": e["size"], "module": e["module"]}

    files = changed_src_files(args)
    if not files:
        print("nothing compiled to check: no changed src/*.c|*.cpp, and no changed "
              "header with a source consumer")
        return
    print(f"link-checking {len(files)} changed src file(s) ...\n")

    # A PR "passes" only when every changed file reproduces the ROM with correct
    # relocation targets (VERIFIED/BENIGN, or BLIND where a slot is unverifiable).
    # WRONG-DEST *and* a non-reproducing NO-REPRO near-miss are both failures — a
    # near-miss is not a match and must not land.
    FAIL = {"WRONG", "NO-REPRO"}
    reports, bad = [], []
    for path in files:
        rep = check_file(path, idx, ledger)
        reports.append(rep)
        if rep["note"] == "unresolved":
            rep["worst"] = "UNRESOLVED"
            print(f"  ?      {path}  ({rep['symbol']}: not in config or ledger)")
            continue
        w = worst(rep["results"])
        rep["worst"] = w
        if w in FAIL:
            bad.append((path, w))
        mark = {"WRONG": "WRONG  ", "NO-REPRO": "NOREPRO", "BENIGN": "ok(ben)",
                "VERIFIED": "ok     ", "BLIND": "ok(bln)"}.get(w, w)
        npass = sum(1 for r in rep["results"] if r.get("passenger"))
        extra = f", +{npass} passenger" if npass else ""
        print(f"  {mark} {path}  ({len(rep['results'])} slot(s){extra})")
        for r in rep["results"]:
            if r["verdict"] == "WRONG":
                for d in r["diffs"]:
                    print(f"           {r['addr']} {r['module']}  {d.get('sym')} -> {d.get('target')}")

    if args.json:
        pathlib.Path(args.json).write_text(json.dumps(reports, indent=1))
    if args.md:
        pathlib.Path(args.md).write_text(render_md(reports, bad), encoding="utf-8")
    if bad:
        print("\nvalidation FAILED: " + "; ".join(f"{p} [{v}]" for p, v in bad))
        if args.fail:
            sys.exit(1)


# Verdict -> a human label for the PR comment. VERIFIED/BENIGN/BLIND are passes;
# NO-REPRO (near-miss) and WRONG-DEST are the two failures.
_LABEL = {
    "VERIFIED":   "✅ verified",
    "BENIGN":     "✅ benign (equivalent veneer/twin)",
    "BLIND":      "🔶 blind (a reloc slot could not be resolved)",
    "NO-REPRO":   "❌ near-miss (does NOT reproduce the ROM)",
    "WRONG":      "❌ wrong-dest (reloc links to the wrong symbol)",
    "NO-SYM":     "🔶 no-sym",
    "UNRESOLVED": "🔶 unresolved (symbol not in config/ledger)",
}


def render_md(reports, bad):
    n = len(reports)
    out = []
    if bad:
        kinds = ", ".join(sorted({v for _, v in bad}))
        out.append(f"**{len(bad)} of {n} changed file(s) do not match the ROM** ({kinds}).")
    else:
        out.append(f"**All {n} changed file(s) compile to the ROM byte-for-byte with correct relocation targets.**")
    out += ["", "| File | Symbol | Result | Slots checked |", "|---|---|---|---|"]
    for rep in reports:
        w = rep.get("worst", "?")
        npass = sum(1 for r in rep["results"] if r.get("passenger"))
        checked = f"{len(rep['results'])}" + (f" (+{npass} passenger)" if npass else "")
        out.append(f"| `{rep['file']}` | `{rep['symbol']}` | {_LABEL.get(w, w)} | {checked} |")
    # list the compiler-emitted passengers that were also verified (thunks / weak copies)
    for rep in reports:
        ps = sorted({r["sym"] for r in rep["results"] if r.get("passenger")})
        if ps:
            out += ["", f"- `{rep['file']}` also verified {len(ps)} emitted passenger(s): "
                        + ", ".join(f"`{p}`" for p in ps)]
    # spell out every wrong-dest reloc so a reviewer can see the exact bad link
    for rep in reports:
        for r in rep["results"]:
            if r["verdict"] == "WRONG":
                for d in r["diffs"]:
                    out += ["", f"- `{rep['file']}` `{r.get('sym')}` @ `{r['addr']}` ({r['module']}): "
                                f"wrong reloc `{d.get('sym')}` → `{d.get('target')}`"]
    return "\n".join(out) + "\n"


if __name__ == "__main__":
    main()
