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

  WRONG-MODULE   `overlay_residency.possible()` rules out every module that
                 defines the name, so the named function can never be in memory
                 alongside the caller. This is a wrong symbol, full stop. Where
                 config records a relocation for the site, the report prints the
                 symbol config actually targets, which is the fix.

  CROSS-OVERLAY  `overlay_residency.possible()` does not rule out at least one
                 module that defines the name (and it is not this module or an
                 always-resident one). Co-residency is possible, so the
                 reference may be legitimate; reported for eyes, not failed.
                 Each site is cross-checked against config's recorded destination
                 (including the plural `overlays(2,7)` spelling dsd uses for an
                 address shared by same-base twins): a reference whose config
                 destination is a definition of the very name it uses is printed
                 as a count, and only the disagreeing remainder is listed. A
                 disagreeing site is usually an addend (symbol+offset resolves
                 away from the symbol's own address), which is reloc_audit.py's
                 domain, but it is where a wrong name hides when residency alone
                 cannot condemn it.

  THE GROUND TRUTH. A raw address-span overlap is a real, provable exclusion --
  `overlay_residency.py`'s E2 cites `LoadOverlay` (src/LoadOverlay.c), which
  walks a 12-entry resident table and calls `Crash()` when an incoming
  overlay's [start, start+code+bss) intersects one already there: the game
  enforces this itself. But span overlap is not the only fact the ROM gives up,
  and it is not always the right one -- `dScStarSel_c::Render`'s 11
  `data_ov000_*` references span-overlap ov003 (ov000 is 0x020aa420..0x020bf4e0,
  ov003's family starts at 0x020ad660, inside it), yet ov000 is a BOOT-ONLY
  overlay (E1): the ROM's only `LoadOverlay(0)`/`UnloadOverlay(0)` sites are
  three consecutive lines in `func_0201a2f8` (src/func_0201a2f8.c:41-43), which
  unloads it again before any scene runs, so span overlap alone would have
  passed a caller that could not possibly still see it. `overlay_residency.py`
  encodes exactly these ROM-proven facts (E1-E6, its own module docstring) and
  is validated by `python tools/overlay_residency.py --check` against every
  relocation dsd resolved unambiguously: 0 of 2322 proven calls contradicted.
  Calling `possible(candidate_module, this_module, this_function)` per owner is
  therefore the verdict, never a bare span test.

Names no symbols.txt defines at all are eligible.py rule 5's problem and are not
repeated here. mwccarm's division helpers resolve through the same runtime alias
table `reloc_audit.py` uses, so `/` and `%` do not read as crossings.

  NO-OBJECT      the file would not compile down to the object this audit reads,
                 so nothing above ran for its candidates at all -- a coverage gap,
                 not a verdict. The overwhelmingly common cause is a promoted
                 consolidated TU: `rombuild.compile_one`'s production path passes
                 it `compiler_only_policies()` (the manifest's per-symbol
                 deadstrip/deadstrip-duplicate/deadstrip-data allow-list, keyed by
                 `config/tu_manifest.d/**/*.json`'s `compiler_only_output`) and
                 `intact_tu_policies()` (promoted `production_mode: intact-object`
                 entries) so `_isolate` can strip or admit exactly the RTTI/vtable
                 content the byte gate itself licenses before the text-only
                 multi-symbol check (`objisolate.isolate_many`) runs. Build these
                 same two policy dicts once here and pass them through, or every
                 multi-function TU whose compiled object still carries an
                 unlicensed `.data` section (a `_ZTI`/`_ZTS`/`_ZTV` record the
                 byte gate discards or admits by manifest) fails isolation and
                 every candidate in that file goes unaudited -- the exact gap
                 that hid crossing 21 (`d_a_pg_mthr.cpp`) from this tool.

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
import overlay_residency as OR              # noqa: E402
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


def audit_one(rel, name, addr, size, module, raw, ctx):
    """One candidate's findings, given the already-compiled bytes of its file."""
    try:
        live = OI.referenced_undefined(raw, name)
    except Exception as e:                                  # noqa: BLE001
        return rel, name, module, [{"verdict": "ELF-ERROR",
                                    "detail": type(e).__name__}]
    dests, _ = RA.object_reloc_dests(raw, name, ctx["name_index"])
    cfgmap = ctx["config_relocs"].get(module, {})
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
        # Ground-truthed, not numeric: an owner is excluded only when
        # overlay_residency.possible() -- the ROM's own loader facts (E1-E6),
        # not a bare address-span overlap -- rules it out for THIS caller.
        exclusive = {m for m in owners if not OR.possible(m, module, name)}
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


def audit_file(rel, group, ctx):
    """Every candidate in one enrolled source, sharing ONE compile.

    `group` is every row (name, addr, size, module) that `candidates()` reports for
    `rel`. A consolidated TU enrolls many functions under the same source, all
    resolving to the same `rombuild.compile_one` output path; compiling and reading
    it once per file (not once per candidate) is not just an optimization here, it
    is required for correctness under `ThreadPoolExecutor`. Two candidate rows for
    the same file used to become two independent `compile_one` calls that raced on
    the identical `build/<rel>.o` path -- one thread's cache-fetch or `_isolate`
    write landing mid-read of another thread's read of the same bytes, surfacing
    as `elftools...ELFError: Magic number does not match` (observed at -j8 and -j12,
    the tool's own default, on an unrelated ThreadPoolExecutor race, not a corrupt
    cache entry: the identical audit is race-free at -j1 over the same objcache).
    """
    module = group[0][3]
    _r, err, _outcome = RB.compile_one(rel, ctx["vers"], ctx["cache"],
                                       ctx["init_srcs"], ctx["syms"],
                                       compiler_only=ctx["compiler_only"],
                                       intact_tus=ctx["intact_tus"])
    obj = RB.BUILD / pathlib.Path(rel).with_suffix(".o")
    if err or not obj.is_file():
        detail = (err or "object not produced")[:200]
        return [(rel, name, module, [{"verdict": "NO-OBJECT", "detail": detail}])
                for name, _addr, _size, _module in group]
    raw = obj.read_bytes()
    return [audit_one(rel, name, addr, size, module, raw, ctx)
            for name, addr, size, _module in group]


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

    # The same two manifest-derived policy dicts the production build computes
    # once and threads through every `compile_one` call (rombuild.py main()),
    # so this audit's compile matches what the byte gate actually licenses
    # instead of always taking compile_one's bare text-only isolation path --
    # see the NO-OBJECT section of this module's docstring.
    enrolled_srcs = RB.enrolled()
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
        "compiler_only": RB.compiler_only_policies(enrolled_srcs),
        "intact_tus": RB.intact_tu_policies(enrolled_srcs),
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

    # One compile per FILE, not per candidate: a consolidated TU enrolls many
    # functions under the same rel, and dispatching each as its own executor task
    # raced two threads over the identical build/<rel>.o path (see audit_file).
    groups = collections.OrderedDict()
    for rel, name, addr, size, module in rows:
        groups.setdefault(rel, []).append((name, addr, size, module))

    report, counts = [], collections.Counter()
    done = 0
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
        for results in ex.map(
                lambda kv: audit_file(kv[0], kv[1], ctx), groups.items()):
            for rel, name, module, findings in results:
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
