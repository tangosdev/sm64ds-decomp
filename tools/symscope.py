"""Does every symbol a source names exist in a module that can be resident with it?

## The false green this exists to stop

Overlays share RAM slots: ov014, ov018, ov019 and ov034 all load at 0x021111a0, so at
most one of them is ever in memory. Two of them can therefore place two different
functions at one address, and a source file that names the wrong overlay's function
still passes every existing gate:

  - `match.py` wildcards every relocated word, so it checks that a call is here,
    never what it calls.
  - `reloc_audit.py` resolves the candidate's destination ADDRESS and compares it to
    config's. Both spellings resolve to the same address, so it answers OK; its
    classify() documents dropping same-address module disagreements on purpose.
  - `linkcheck.py` links the address and byte-compares the result, which is identical
    either way for the same reason.
  - `eligible.py` rule 5 resolves references against every symbols.txt merged into
    one flat namespace, so a name from a never-coresident overlay counts as resolved.

Observed: `daPgMthr_c::Behavior`, enrolled in ov018 (in a per-function legacy
source at the time; now part of `src/game/actors/d_a_pg_mthr.cpp`), called
`_ZN13RacingPenguin16OnPendingDestroyEv`, which is ov019's 4-byte `bx lr` stub. The
ROM's call at 0x02112488 goes to func_ov018_0211235c, ov018's pointer-to-member
dispatcher, which sits at the same address 0x0211235c that ov019 gives the stub.
Byte gate, reloc audit and linkcheck all passed; the source still named a function
that cannot exist in memory when the caller runs.

## What this checks

For every enrolled source, compiled exactly the way the build compiles it (same
pin, same flags, shared object cache, objisolate applied), take the undefined
names the kept function actually references and resolve each one against the
symbols of the modules that can be resident when this module is: the module
itself, plus arm9, itcm and dtcm, which are always loaded. The index is built
per module from `config/arm9/**/symbols.txt`, never flattened.

  WRONG-MODULE   every definition of the name lives in an overlay whose address
                 span overlaps this module's, so the named function can never be
                 in memory alongside the caller. This is a wrong symbol, full
                 stop. Where config records a relocation for the site, the report
                 prints the symbol config actually targets, which is the fix.

  CROSS-OVERLAY  the name resolves only in overlays whose spans do not overlap
                 this module's. Disjoint overlays can be co-resident, so the
                 reference may be legitimate; reported for eyes, not failed.
                 Each site is cross-checked against config's recorded destination
                 (including the plural `overlays(2,7)` spelling dsd uses for an
                 address shared by same-base twins): a reference whose config
                 destination is a definition of the very name it uses is printed
                 as a count, and only the disagreeing remainder is listed. A
                 disagreeing site is usually an addend (symbol+offset resolves
                 away from the symbol's own address), which is reloc_audit.py's
                 domain, but it is where a wrong name hides when the span test
                 alone cannot condemn it.

Names no symbols.txt defines at all are eligible.py rule 5's problem and are not
repeated here. mwccarm's division helpers resolve through the same runtime alias
table `reloc_audit.py` uses, so `/` and `%` do not read as crossings.

Usage:
    python tools/symscope.py                    # audit every enrolled source
    python tools/symscope.py -j 16 --json build/symscope.json
    python tools/symscope.py src/Foo.cpp        # just these files
    python tools/symscope.py --families         # the shared-slot overlay groups
"""
import argparse
import collections
import concurrent.futures
import json
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

import objisolate as OI                     # noqa: E402
import reloc_audit as RA                    # noqa: E402
import relocs as R                          # noqa: E402
import rombuild as RB                       # noqa: E402
import rombuild_cache as RBK                # noqa: E402
from eligible import module_label           # noqa: E402
from enroll import candidates               # noqa: E402

ALWAYS = {"arm9", "itcm", "dtcm"}
_SECTION = re.compile(r"^\s+\.\S+\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)\b")
_MULTI = re.compile(r"overlays\((\d+(?:,\d+)*)\)")


def _dest_modules(to_mod):
    """Config's destination module(s) for one reloc, as normalized IDs.

    dsd writes `overlays(2,7)` when an address belongs to several same-base
    overlays and the loaded one decides; `relocs.normalize_module` passes that
    spelling through untouched, so expand it here. Everything else normalizes
    to a single ID."""
    m = _MULTI.fullmatch(str(to_mod).strip())
    if m:
        return {f"ov{int(x):03d}" for x in m.group(1).split(",")}
    return {R.normalize_module(to_mod)}


def module_spans():
    """{module: (lo, hi)} address span of every module, from its delinks.txt header.

    Only the header block is read: the indented section lines before the first file
    entry. The per-file `.text start:` lines further down all fall inside the same
    span, so reading them would change nothing; stopping at the first unindented
    line keeps that an invariant instead of a coincidence.
    """
    spans = {}
    for module, syms_path in R.iter_symbol_files(include_itcm_dtcm=True):
        delinks = syms_path.parent / "delinks.txt"
        if not delinks.is_file():
            continue
        lo = hi = None
        for line in delinks.read_text(encoding="utf-8", errors="ignore").splitlines():
            if line and not line[0].isspace():
                break
            m = _SECTION.match(line)
            if m:
                s, e = int(m.group(1), 16), int(m.group(2), 16)
                lo = s if lo is None else min(lo, s)
                hi = e if hi is None else max(hi, e)
        if lo is not None:
            spans[module] = (lo, hi)
    return spans


def _overlap(a, b):
    return a[0] < b[1] and b[0] < a[1]


def families(spans):
    """Overlays grouped by load base: the groups a human recognizes as one RAM slot.

    Display only. Exclusivity is a PAIRWISE property of address spans, never a
    transitive one: overlay bases chain-overlap across the whole address range, so
    a transitive closure collapses all hundred overlays into one useless group,
    while two overlays with disjoint spans can be co-resident even when a third
    overlaps both. The verdicts below therefore test span intersection per pair;
    this listing just names the same-base groups."""
    by_base = collections.defaultdict(list)
    for m in sorted(m for m in spans if m not in ALWAYS):
        by_base[spans[m][0]].append(m)
    return [g for _b, g in sorted(by_base.items()) if len(g) > 1]


def name_definitions():
    """name -> {(module, addr)}: every definition of every name, keyed by module.

    A set, not a first-wins dict: overlays legitimately reuse spellings, and which
    module defines a name is exactly the question this tool answers, so no
    definition may shadow another."""
    defs = collections.defaultdict(set)
    for module, path in R.iter_symbol_files(include_itcm_dtcm=True):
        for name, (mod, addr) in R.iter_syms_pairs(path, module):
            defs[name].add((mod, addr))
    return defs


def audit_one(row, ctx):
    """One file's findings. Compiles (or cache-fetches) the object the build links."""
    rel, name, addr, size, module = row
    _r, err, _outcome = RB.compile_one(rel, ctx["vers"], ctx["cache"],
                                       ctx["init_srcs"], ctx["syms"])
    obj = RB.BUILD / pathlib.Path(rel).with_suffix(".o")
    if err or not obj.is_file():
        return rel, name, module, [{"verdict": "NO-OBJECT",
                                    "detail": (err or "object not produced")[:200]}]
    raw = obj.read_bytes()
    try:
        live = OI.referenced_undefined(raw, name)
    except Exception as e:                                  # noqa: BLE001
        return rel, name, module, [{"verdict": "ELF-ERROR",
                                    "detail": type(e).__name__}]
    dests, _ = RA.object_reloc_dests(raw, name, ctx["name_index"])
    cfgmap = ctx["config_relocs"].get(module, {})
    spans = ctx["spans"]
    findings = []
    for n in sorted(live):
        dd = ctx["name_defs"].get(n)
        if not dd:
            alias = RA._RUNTIME_ALIASES.get(n)
            dd = ctx["name_defs"].get(alias) if alias else None
        if not dd:
            continue                    # defined nowhere: eligible rule 5's domain
        owners = {m for m, _a in dd}
        if module in owners or owners & ALWAYS:
            continue                    # visible where this file links
        span = spans.get(module)
        exclusive = {m for m in owners
                     if span and m in spans and _overlap(span, spans[m])}
        sites = []
        if isinstance(dests, list):
            for (off, symname, _m, _a) in dests:
                if symname != n:
                    continue
                cfg = cfgmap.get(addr + off)
                if cfg is None:
                    sites.append({"off": f"+0x{off:x}", "config": None,
                                  "should_be": None, "agrees": None})
                    continue
                _kind, to_addr, to_mod = cfg
                cms = _dest_modules(to_mod)
                should = next((ctx["sym_index"][(cm, to_addr)] for cm in sorted(cms)
                               if (cm, to_addr) in ctx["sym_index"]), None)
                sites.append({"off": f"+0x{off:x}",
                              "config": f"0x{to_addr:08x}:{to_mod}",
                              "should_be": should,
                              "agrees": any(dm in cms and da == to_addr
                                            for dm, da in dd)})
        checked = [s for s in sites if s["agrees"] is not None]
        findings.append({
            "verdict": "WRONG-MODULE" if owners == exclusive else "CROSS-OVERLAY",
            "symbol": n,
            "defined_in": sorted(f"{m}:0x{a:08x}" for m, a in dd),
            "config_agrees": (all(s["agrees"] for s in checked)
                              if checked else None),
            "sites": sites,
        })
    return rel, name, module, findings


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("-j", "--jobs", type=int, default=RB.default_jobs())
    ap.add_argument("--json", help="write the full report to this path")
    ap.add_argument("--families", action="store_true",
                    help="print the shared-slot overlay groups and exit")
    ap.add_argument("files", nargs="*", help="limit the audit to these source paths")
    args = ap.parse_args()

    spans = module_spans()
    fams = families(spans)
    print(f"{len(fams)} shared-slot overlay families (members never co-resident):")
    for g in fams:
        lo = min(spans[m][0] for m in g)
        hi = max(spans[m][1] for m in g)
        print(f"  0x{lo:08x}..0x{hi:08x}  {', '.join(g)}")
    if args.families:
        return 0

    ctx = {
        "spans": spans,
        "name_defs": name_definitions(),
        "name_index": RA.build_name_index(),
        "config_relocs": RA.build_config_relocs(),
        "sym_index": R.load_all_syms(),
        "vers": RB.versions(),
        "cache": RBK.ObjectCache(RB.BUILD / "objcache", REPO),
        "init_srcs": RB.init_section_sources(),
        "syms": RB.enrolled_symbols(),
    }
    want = ({pathlib.PurePath(f).as_posix() for f in args.files}
            if args.files else None)
    rows = []
    for (d, name, rel, addr, size, _sec) in candidates()[0]:
        rel = str(rel).replace("\\", "/")
        if want and rel not in want:
            continue
        rows.append((rel, name, addr, size, module_label(d)))
    rows.sort()
    print(f"auditing {len(rows)} enrolled source file(s) with -j{args.jobs} ...")

    report, counts = [], collections.Counter()
    done = 0
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
        for rel, name, module, findings in ex.map(
                lambda r: audit_one(r, ctx), rows):
            done += 1
            if done % 1000 == 0:
                print(f"  {done}/{len(rows)}", flush=True)
            if not findings:
                counts["clean"] += 1
                continue
            for f in findings:
                counts[f["verdict"]] += 1
            report.append({"file": rel, "name": name, "module": module,
                           "findings": findings})

    print()
    for k in ("clean", "WRONG-MODULE", "CROSS-OVERLAY", "NO-OBJECT", "ELF-ERROR"):
        if counts[k]:
            print(f"  {counts[k]:6d}  {k}")

    hard = [(e, f) for e in report for f in e["findings"]
            if f["verdict"] == "WRONG-MODULE"]
    soft = [(e, f) for e in report for f in e["findings"]
            if f["verdict"] == "CROSS-OVERLAY"]
    if hard:
        print(f"\nWRONG-MODULE, a symbol from a never-coresident overlay: {len(hard)}")
        for e, f in hard:
            print(f"  {e['file']} ({e['module']}) names {f['symbol']}")
            print(f"      defined in: {', '.join(f['defined_in'])}")
            for s in f["sites"]:
                fix = f" -> should name {s['should_be']}" if s["should_be"] else ""
                print(f"      site {s['off']}: config {s['config']}{fix}")
    if soft:
        agree = [(e, f) for e, f in soft if f["config_agrees"]]
        rest = [(e, f) for e, f in soft if not f["config_agrees"]]
        print(f"\nCROSS-OVERLAY, resolves only outside the module but the overlays "
              f"can co-load: {len(soft)}")
        print(f"  {len(agree)} agree with config's recorded destinations "
              f"(legitimate cross-overlay references; full list in --json)")
        if rest:
            print(f"  {len(rest)} disagree with config, or have no site to check:")
            for e, f in rest:
                print(f"    {e['file']} ({e['module']}) names {f['symbol']} "
                      f"[{', '.join(f['defined_in'])}]")
                for s in f["sites"]:
                    if s["agrees"] is False:
                        fix = (f" (config's symbol there: {s['should_be']})"
                               if s["should_be"] else "")
                        print(f"        site {s['off']}: config {s['config']}{fix}")

    if args.json:
        out = pathlib.Path(args.json)
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(json.dumps({"families": fams,
                                   "counts": dict(counts),
                                   "entries": report}, indent=2) + "\n",
                       encoding="utf-8", newline="\n")
        print(f"\nwrote {out}")
    return 1 if hard else 0


if __name__ == "__main__":
    sys.exit(main())
