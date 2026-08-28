"""Compare the DATA a compiled src/ object emits against the cartridge.

WHY NOTHING CHECKED THIS
------------------------
Delink entries carve out functions. `tools/objisolate.py` then reduces each compiled
object to the single `.text` its entry names and zeroes every other content section,
rebinding the dropped symbols to the ROM's own carved-out addresses. That is correct
and load-bearing -- an object that kept defining `_ZTV4Coin` in a now-empty `.data`
would have every user of Coin's vtable bind to address 0 -- but it means the vtable,
typeinfo record and typeinfo-name string that mwcc emitted are thrown away unexamined.

So the ROM build's `106/106 exact` says nothing whatever about them. It cannot: those
bytes come from a gap object holding the cartridge's own data, compared against itself.
Roughly 26.7% of every module image is data in that position, and the fraction of it
this project has ever verified was, before this tool, exactly zero.

WHAT IT ANSWERS
---------------
For every data symbol a compiled source emits AND the ROM names in a `symbols.txt`:
does the emitted object, once its relocations are resolved the way the linker would,
equal the bytes at that address in the retail module?

A hit is a real verification the build was already computing and discarding. A miss is
a defect no existing gate can see -- a class whose vtable disagrees with the ROM's is a
wrong class model even when its destructor byte-matches perfectly, and `include/` is
full of generated flat headers that declare no virtuals at all.

WHY IT IS A MEASUREMENT AND NOT A PASS/FAIL GATE
------------------------------------------------
Because most of the tree fails it, for a reason that is understood. `BrickBlock`'s
generated header declares zero virtuals, so mwcc emits a 2-slot vtable stub where the
ROM has 31 slots; the emitted prefix is not even the same slots. Failing a merge on
that would fail nearly every C++ file in the tree for pre-existing modelling debt.

So the verdicts are counted, the count is reported, and `validate_merge` ratchets it:
the number of verified data symbols may rise freely and may not fall. That is the same
shape as langmode-ratchet and converted-ratchet, and it is the only shape that can
land green.

THE VTABLE PREAMBLE, AND WHY THE COMPARE IS OFFSET
--------------------------------------------------
mwcc's `_ZTV<C>` addresses the START of the vtable object -- offset-to-top, then
typeinfo, then the slots. This tree's `symbols.txt` uses the other convention: `_ZTV<C>`
IS the slot array, already past those two words. `objisolate.VTABLE_PREAMBLE` exists for
exactly this reason and is reused here rather than respelled, so the two cannot drift.

Usage:
    python tools/romdata_check.py                       # every enrolled source
    python tools/romdata_check.py --files src/a.cpp
    python tools/romdata_check.py --json build/romdata.json
"""
import argparse
import collections
import concurrent.futures
import io
import json
import os
import pathlib
import re
import subprocess
import sys
import tempfile
import threading

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
from elftools.elf.elffile import ELFFile  # noqa: E402
import linkcheck as LC  # noqa: E402
import objisolate as OI  # noqa: E402
import relocs as RL  # noqa: E402
import reloc_audit as RA  # noqa: E402
import reverify_corpus as RV  # noqa: E402
# `rombuild` is imported lazily inside _compile/main, never at module scope: rombuild
# imports THIS module so it can measure each object while it is still raw, and a
# top-level import here would close that cycle. Nothing above _compile needs it.

_SYM_RE = re.compile(r"^(\S+)\s+kind:(\S+?)[\s(].*?addr:0x([0-9a-fA-F]+)")

# VERIFIED  every byte of the ROM's extent for this symbol was compared and equal
# PARTIAL   everything compared was equal, but the coverage is short -- the object
#           emitted fewer bytes than the ROM's extent, or some words were blind
# DIFFERS   at least one compared byte disagreed
# UNNAMED   no symbols.txt entry gives this symbol an address; nothing to compare to
VERIFIED, PARTIAL, DIFFERS, UNNAMED = "VERIFIED", "PARTIAL", "DIFFERS", "UNNAMED"

_index_lock = threading.Lock()
_index = None
_names_lock = threading.Lock()
_names = None


def name_index():
    """`reloc_audit`'s symbol->address index, built once for the whole process.

    Cached because `check_object` runs once per compiled file inside the ROM build's
    thread pool, and rebuilding this per object would walk every symbols.txt eleven
    thousand times.
    """
    global _names
    with _names_lock:
        if _names is None:
            _names = RA.build_name_index()
        return _names


def rom_data_index():
    """{(module, name): (addr, extent)} for every symbol the ROM config names.

    `extent` is the distance to the next symbol in the same module, which is the only
    size information available: `symbols.txt` writes data as `kind:data(any)` with no
    size, while functions carry one. It is an upper bound on the object -- a trailing
    alignment gap belongs to nobody -- so a short compare is reported PARTIAL rather
    than being silently rounded up into a pass.
    """
    global _index
    with _index_lock:
        if _index is not None:
            return _index
        per_module = collections.defaultdict(list)
        for sym_path, label in RL.module_universe():
            for line in sym_path.read_text(encoding="utf-8", errors="ignore").splitlines():
                m = _SYM_RE.match(line.strip())
                if m:
                    per_module[label].append((int(m.group(3), 16), m.group(1)))
        index = {}
        for label, entries in per_module.items():
            entries.sort()
            for i, (addr, name) in enumerate(entries):
                nxt = next((entries[j][0] for j in range(i + 1, len(entries))
                            if entries[j][0] > addr), None)
                if nxt is not None:
                    index[(label, name)] = (addr, nxt - addr)
        _index = index
        return _index


def emitted_data_symbols(raw):
    """[(name, shndx, value, size)] for defined data objects in this object file.

    Every one of these is a section `objisolate` drops, because it keeps exactly one
    `.text` and zeroes the rest. Locals are skipped: a static table has no ROM symbol
    to be compared against and never collides with one.
    """
    elf = ELFFile(io.BytesIO(raw))
    symtab = elf.get_section_by_name(".symtab")
    if symtab is None:
        return []
    out = []
    for s in symtab.iter_symbols():
        if not s.name or s["st_shndx"] in ("SHN_UNDEF", "SHN_ABS"):
            continue
        if s["st_info"]["type"] != "STT_OBJECT" or not s["st_size"]:
            continue
        if s["st_info"]["bind"] == "STB_LOCAL":
            continue
        out.append((s.name, s["st_shndx"], s["st_value"], s["st_size"]))
    return out


def verify_data_symbol(raw, name, shndx, value, size, names=None):
    """One verdict dict for one emitted data symbol.

    The relocations are applied exactly as `linkcheck.link_function` applies them to a
    function body -- same helper, so a vtable slot and a `bl` target are resolved by one
    piece of code. Words it could not resolve come back as `blind` and are excluded from
    the comparison rather than counted as differences: an unresolvable slot is a word we
    have no opinion about, and calling it a mismatch would manufacture defects.
    """
    names = names if names is not None else name_index()
    resolved = RA.resolve_candidate(name, names)
    if not resolved or resolved[0] is None:
        return {"symbol": name, "verdict": UNNAMED, "bytes": 0}
    module, addr = resolved
    extent = rom_data_index().get((module, name), (addr, None))[1]

    elf = ELFFile(io.BytesIO(raw))
    body = elf.get_section(shndx).data()[value:value + size]
    # mwcc's _ZTV symbol starts at the vtable OBJECT; the ROM's starts at the slots.
    preamble = OI.VTABLE_PREAMBLE if name.startswith("_ZTV") else 0
    if len(body) <= preamble:
        return {"symbol": name, "verdict": PARTIAL, "bytes": 0, "module": module,
                "addr": addr, "note": "nothing past the vtable preamble"}
    linked, blind = LC.link_function(body, addr - preamble,
                                     LC.func_relocs_typed(raw, name, names) or [])
    linked = linked[preamble:]
    blind = {o - preamble for o in blind if o >= preamble}

    retail = RV.rom_bytes(module, addr, len(linked))
    if retail is None or len(retail) < len(linked):
        return {"symbol": name, "verdict": PARTIAL, "bytes": 0, "module": module,
                "addr": addr, "note": "module image shorter than the emitted symbol"}
    compared = differing = 0
    for off in range(0, len(linked) & ~3, 4):
        if off in blind:
            continue
        compared += 4
        if linked[off:off + 4] != retail[off:off + 4]:
            differing += 4
    rec = {"symbol": name, "module": module, "addr": addr, "bytes": compared,
           "emitted": len(linked), "romExtent": extent, "blindWords": len(blind)}
    if differing:
        rec["verdict"] = DIFFERS
        rec["differingBytes"] = differing
    elif compared and not blind and extent is not None and len(linked) >= extent:
        rec["verdict"] = VERIFIED
    else:
        rec["verdict"] = PARTIAL
    return rec


def check_object(obj_path, rel, names=None):
    """Verdicts for every data symbol in one compiled object.

    Called from `rombuild.compile_one` while the object is still RAW -- `objisolate`
    zeroes these sections moments later, which is the whole reason this has to happen
    inside the build rather than over the artifacts it leaves behind.
    """
    try:
        raw = pathlib.Path(obj_path).read_bytes()
        out = []
        for name, shndx, value, size in emitted_data_symbols(raw):
            rec = verify_data_symbol(raw, name, shndx, value, size, names)
            rec["src"] = rel
            out.append(rec)
        return out
    except Exception as exc:  # noqa: BLE001 - a measurement must never fail a build
        # This runs inside the ROM build. A malformed object, a module image this tool
        # cannot map, or any other surprise must cost its own measurement and nothing
        # else -- failing the link over a statistic would be strictly worse than not
        # having the statistic.
        return [{"src": rel, "symbol": "?", "verdict": UNNAMED, "bytes": 0,
                 "note": f"{type(exc).__name__}: {exc}"}]


def _symbol_key(r):
    """Identity a verdict record is deduped on.

    Vague linkage means the same cartridge data symbol -- a vtable, a typeinfo record --
    is independently emitted by every object that needs it, so counting RECORDS makes the
    headline a function of how many files the tree happens to be split into rather than
    how much data is actually proven. `symbol == "?"` is check_object's own catch-all for
    an object it could not even parse; it carries no real identity, so those never dedupe
    against each other -- keying them by `src` too would be wrong the other direction.
    """
    if r["symbol"] == "?":
        return (r.get("module"), r["symbol"], r["src"])
    return (r.get("module"), r["symbol"])


_VERDICT_RANK = {UNNAMED: 0, PARTIAL: 1, VERIFIED: 2}


def summarize(records):
    counts = collections.Counter(r["verdict"] for r in records)

    # One verdict per unique data symbol: DIFFERS wins outright over every other record
    # for that symbol -- a class whose vtable is right in one file and wrong in another
    # is still a wrong class model, so a correct copy must not hide a bad sibling. Absent
    # a DIFFERS record, the best of what is left (VERIFIED over PARTIAL over UNNAMED).
    best = {}
    for r in records:
        key = _symbol_key(r)
        cur = best.get(key)
        if cur is None:
            best[key] = r
        elif cur["verdict"] == DIFFERS:
            continue
        elif r["verdict"] == DIFFERS:
            best[key] = r
        elif _VERDICT_RANK[r["verdict"]] > _VERDICT_RANK[cur["verdict"]]:
            best[key] = r
    symbol_counts = collections.Counter(r["verdict"] for r in best.values())
    verified_bytes = sum(r.get("bytes", 0) for r in best.values()
                         if r["verdict"] == VERIFIED)
    partial_bytes = sum(r.get("bytes", 0) for r in best.values()
                        if r["verdict"] == PARTIAL)
    return {
        "symbols": len(best),
        "verified": symbol_counts[VERIFIED],
        "partial": symbol_counts[PARTIAL],
        "differs": symbol_counts[DIFFERS],
        "unnamed": symbol_counts[UNNAMED],
        "verifiedBytes": verified_bytes,
        "partialBytes": partial_bytes,
        # Per-object-record totals, kept for visibility -- these move with file topology
        # (a TU merge or a duplicate-file cleanup changes them without a symbol's verdict
        # changing) and are not what validate_merge ratchets.
        "verifiedRecords": counts[VERIFIED],
        "partialRecords": counts[PARTIAL],
        "differsRecords": counts[DIFFERS],
        "unnamedRecords": counts[UNNAMED],
        "totalRecords": len(records),
        # The actionable list: a class model whose vtable disagrees with the cartridge.
        # Left per-record (not deduped) so it still names every file carrying a bad copy.
        "differing": sorted(
            ({"src": r["src"], "symbol": r["symbol"], "module": r.get("module"),
              "addr": r.get("addr"), "differingBytes": r.get("differingBytes")}
             for r in records if r["verdict"] == DIFFERS),
            key=lambda r: (r["src"], r["symbol"])),
    }


def _compile(rel, tmpdir):
    """Compile one source the way the ROM build does, WITHOUT isolating it."""
    import rombuild as RB
    src = REPO / rel
    flags = RB.CFLAGS
    try:
        if src.read_text(encoding="utf-8").startswith("//cpp"):
            flags = flags.replace("-lang c99", "-lang c++")
    except OSError:
        return None
    version = RB.versions().get(pathlib.Path(rel).stem, RB.VERSION)
    obj = pathlib.Path(tmpdir) / (pathlib.Path(rel).stem + ".o")
    proc = subprocess.run(
        [*RB.launcher(), str(RB.MW / version / "mwccarm.exe"), *flags.split(),
         "-i", str(RB.INCLUDE), "-c", str(src), "-o", str(obj)],
        capture_output=True, text=True,
        env=dict(os.environ, LM_LICENSE_FILE=str(RB.LICENSE)), cwd=REPO)
    return obj if proc.returncode == 0 and obj.is_file() else None


def main():
    import rombuild as RB
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--files", nargs="*", help="sources to check (default: all enrolled)")
    ap.add_argument("-j", "--jobs", type=int, default=RB.default_jobs())
    ap.add_argument("--json", help="write the structured report here")
    ap.add_argument("--show", type=int, default=15, help="differing symbols to print")
    args = ap.parse_args()

    sources = args.files or [s for s in RB.enrolled() if s.startswith("src/")]
    names = name_index()
    rom_data_index()
    records = []
    with tempfile.TemporaryDirectory() as tmp:
        def one(rel):
            obj = _compile(rel, tmp)
            return check_object(obj, rel, names) if obj else []
        with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
            for got in ex.map(one, sources):
                records.extend(got)

    report = summarize(records)
    report["sourcesChecked"] = len(sources)
    print(f"data symbols emitted by {len(sources):,} enrolled source(s): "
          f"{report['symbols']:,}")
    print(f"  VERIFIED {report['verified']:,}  ({report['verifiedBytes']:,} bytes "
          f"equal to the cartridge)")
    print(f"  PARTIAL  {report['partial']:,}  ({report['partialBytes']:,} bytes equal, "
          f"coverage short of the ROM's extent)")
    print(f"  DIFFERS  {report['differs']:,}")
    print(f"  UNNAMED  {report['unnamed']:,}  (no symbols.txt address to compare against)")
    for r in report["differing"][:args.show]:
        print(f"    {r['symbol']:44s} {r['module'] or '?':6s} "
              f"0x{(r['addr'] or 0):08x}  {r['differingBytes']} byte(s)  {r['src']}")
    if len(report["differing"]) > args.show:
        print(f"    +{len(report['differing']) - args.show} more")
    if args.json:
        pathlib.Path(args.json).write_text(json.dumps(report, indent=2) + "\n",
                                           encoding="utf-8", newline="\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
