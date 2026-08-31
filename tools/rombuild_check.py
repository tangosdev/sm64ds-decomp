"""Analyze a source-built ROM and emit human or machine-readable fidelity metrics.

``dsd check modules`` answers the binary gate.  This tool explains that answer and
separates three quantities that must not be collapsed into one percentage:

* module fidelity -- bytes in linked executable modules equal the retail modules;
* source reconstruction -- function bytes produced from verified ``src/`` files;
* intentional mods -- differing bytes confined to explicitly enrolled ``mods/`` ranges.

Usage:
    python tools/rombuild_check.py
    python tools/rombuild_check.py --json build/rombuild-report.json
    python tools/rombuild_check.py --config-root build/rombuild-config/stock/arm9
"""
import argparse
import collections
import hashlib
import json
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from enroll import read_delinks, sections, CONFIG, REPO  # noqa: E402
import srcpath as SP  # noqa: E402

BUILD = REPO / "build" / "build"
EXTRACTED = REPO / "extracted" / "dsd"
DEFAULT_CONFIG_ROOT = CONFIG / "arm9"
ENTRY_SEC = re.compile(r"^\s+(\.\S+)\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)")
FUNC_RE = re.compile(
    r"^(\S+)\s+kind:function\((?:arm|thumb),size=0x([0-9a-fA-F]+)\)"
    r"\s+addr:0x([0-9a-fA-F]+)"
)


def _arm9_relative(d, config_root):
    """Module path relative to ARM9, accepting config/ or config/arm9 roots."""
    rel = d.relative_to(config_root).as_posix()
    if rel == "arm9":
        return "."
    return rel.removeprefix("arm9/")


def module_label(d, config_root):
    rel = _arm9_relative(d, config_root)
    if rel == ".":
        return "arm9"
    m = re.fullmatch(r"overlays/(ov\d+)", rel)
    if m:
        return m.group(1)
    return rel


def module_binaries(d, config_root=CONFIG, build_root=None):
    """(built, retail) binary paths for a module config directory.

    `build_root` is where the link deposited its module images -- `build/build/`
    for the production build, and a scratch directory for an isolated link such as
    `tools/tubuild.py linkcheck`'s. The retail side always comes from `extracted/`.

    None means "this module's BUILD", resolved at CALL time and deliberately not as a
    default argument value: tools/test_rombuild_check.py rebinds `RBC.BUILD` to a
    temporary directory in setUp, and a default bound at def time would freeze the
    real one and silently compare the tests' fake config against this repository's
    actual module images.
    """
    build_root = pathlib.Path(build_root) if build_root is not None else BUILD
    rel = _arm9_relative(d, config_root)
    if rel == ".":
        return build_root / "arm9.bin", EXTRACTED / "arm9" / "arm9.bin"
    if rel in ("itcm", "dtcm"):
        return build_root / f"{rel}.bin", EXTRACTED / "arm9" / f"{rel}.bin"
    m = re.fullmatch(r"overlays/(ov\d+)", rel)
    if m:
        return (build_root / f"arm9_{m.group(1)}.bin",
                EXTRACTED / "arm9_overlays" / f"{m.group(1)}.bin")
    return None, None


def complete_entry_sections_text(text):
    """Return every ``(path, section, address, end)`` in complete entries."""
    out, cur, done, entry_sections = [], None, False, []

    def flush():
        if cur and done:
            out.extend((cur, name, start, end)
                       for name, start, end in entry_sections)

    for line in text.splitlines():
        if not line.strip():
            continue
        if not line[0].isspace():
            flush()
            cur, done, entry_sections = line.strip().rstrip(":"), False, []
        elif cur is not None:
            if line.strip() == "complete":
                done = True
            else:
                m = ENTRY_SEC.match(line)
                if m:
                    entry_sections.append((m.group(1), int(m.group(2), 16),
                                           int(m.group(3), 16)))
    flush()
    return out


def complete_entries_text(text):
    """Return complete code contributions as ``[(path, address, end)]``.

    Callers of this historical API measure function/source enrollment. Intact C++
    entries can also own data; retaining the section name prevents a trailing .data
    claim from replacing .text and being misreported as source code.
    """
    return [(rel, start, end)
            for rel, name, start, end in complete_entry_sections_text(text)
            if name in (".text", ".init")]


def complete_entries(path):
    return complete_entries_text(path.read_text(encoding="utf-8", errors="ignore"))


def complete_entry_sections(path):
    return complete_entry_sections_text(
        path.read_text(encoding="utf-8", errors="ignore"))


def _covered_bytes(ranges, base, size):
    """Union length of address ranges clipped to one linked module image."""
    clipped = sorted((max(0, lo - base), min(size, hi - base))
                     for lo, hi in ranges if hi > base and lo < base + size)
    total = end = 0
    for lo, hi in clipped:
        if hi <= lo:
            continue
        if lo > end:
            total += hi - lo
            end = hi
        elif hi > end:
            total += hi - end
            end = hi
    return total


def _code_totals(config_root):
    funcs = size = 0
    for sym in sorted(config_root.rglob("symbols.txt")):
        # Match the project's progress/Chaos Viewer universe: main + overlays.
        # itcm/dtcm are still byte-checked as modules, but are not part of the
        # published decompilation coverage denominator.
        rel = _arm9_relative(sym.parent, config_root)
        if rel != "." and not re.fullmatch(r"overlays/ov\d+", rel):
            continue
        for line in sym.read_text(encoding="utf-8", errors="ignore").splitlines():
            m = FUNC_RE.match(line)
            if m:
                funcs += 1
                size += int(m.group(2), 16)
    return funcs, size


def _module_code_bytes(sym_path):
    """Bytes this module's symbols.txt accounts for as functions.

    Everything else in the module image -- vtables, RTTI records, jump tables, string
    and data tables -- is DATA, and no part of this pipeline reconstructs it. Delink
    entries carve out functions; `objisolate` then reduces each compiled object to the
    single `.text` its entry names and zeroes every other content section, so even the
    `.data` a C++ TU does emit is dropped and re-imported from the ROM's gap object.

    Subtracting this from the module image size is what makes that boundary a number
    instead of an unstated assumption. Computed per module rather than from
    `_code_totals`, which excludes itcm/dtcm from the published denominator while
    `moduleFidelity` still compares them -- mixing the two would misattribute their
    whole size to data.
    """
    total = 0
    for line in sym_path.read_text(encoding="utf-8", errors="ignore").splitlines():
        m = FUNC_RE.match(line)
        if m:
            total += int(m.group(2), 16)
    return total


def _module_functions(sym_path):
    """``[(start, end, name)]`` for the module's function inventory."""
    out = []
    for line in sym_path.read_text(encoding="utf-8", errors="ignore").splitlines():
        match = FUNC_RE.match(line)
        if match:
            start = int(match.group(3), 16)
            out.append((start, start + int(match.group(2), 16), match.group(1)))
    return out


def _diff_counts(built, retail, allowed=()):
    common = min(len(built), len(retail))
    differing = unexpected = 0
    for i in range(common):
        if built[i] == retail[i]:
            continue
        differing += 1
        if not any(lo <= i < hi for lo, hi in allowed):
            unexpected += 1
    extra = abs(len(built) - len(retail))
    return differing + extra, unexpected + extra


def analyze(config_root=DEFAULT_CONFIG_ROOT, profile="stock", build_root=None):
    config_root = pathlib.Path(config_root).resolve()
    build_root = pathlib.Path(build_root) if build_root is not None else BUILD
    failures, intentional, module_results = [], [], []
    missing_bins = []
    per_module_bad = collections.Counter()
    source_functions = source_bytes = mod_functions = mod_bytes = 0
    source_data_bytes = 0
    source_data_claims = bad_data_claims = bad_data_bytes = 0
    reproducing_data_claims = reproducing_data_bytes = 0
    reproducing = reproducing_bytes = bad = bad_function_bytes = differing_source_bytes = 0
    module_set_digest = hashlib.sha256()

    for sym in sorted(config_root.rglob("symbols.txt")):
        d = sym.parent
        dl = d / "delinks.txt"
        if not dl.is_file():
            continue
        entry_sections = complete_entry_sections(dl)
        entries = [(rel, start, end) for rel, name, start, end in entry_sections
                   if name in (".text", ".init")]
        built_p, retail_p = module_binaries(d, config_root, build_root)
        label = module_label(d, config_root)
        if not built_p or not built_p.is_file() or not retail_p.is_file():
            missing_bins.append(label)
            continue
        header, _ = read_delinks(dl)
        secs = sections(header)
        if not secs:
            continue
        base = min(s[1] for s in secs)
        built, retail = built_p.read_bytes(), retail_p.read_bytes()
        label_bytes = label.encode("utf-8")
        module_set_digest.update(len(label_bytes).to_bytes(4, "big"))
        module_set_digest.update(label_bytes)
        module_set_digest.update(len(built).to_bytes(8, "big"))
        module_set_digest.update(built)
        allowed_mod_ranges = [(addr - base, end - base)
                              for rel, _name, addr, end in entry_sections
                              if rel.startswith("mods/")]
        source_data_bytes += _covered_bytes(
            [(addr, end) for rel, name, addr, end in entry_sections
             if rel.startswith("src/") and name not in (".text", ".init")],
            base, max(len(built), len(retail)))
        module_diff, unexpected_diff = _diff_counts(built, retail, allowed_mod_ranges)
        module_results.append({
            "module": label,
            "comparedBytes": max(len(built), len(retail)),
            "codeBytes": _module_code_bytes(sym),
            "differingBytes": module_diff,
            "unexpectedDifferingBytes": unexpected_diff,
            "exact": module_diff == 0,
            "expectedOnly": unexpected_diff == 0,
        })
        functions = _module_functions(sym)

        for rel, addr, end in entries:
            size = end - addr
            # A complete delinks entry is a physical object contribution, not a
            # function. Reconstructed production TUs legitimately cover several
            # symbols with one entry; count the symbols whose full ranges it owns so
            # source coverage stays invariant under consolidation.
            owned = [(start, stop - start, name) for start, stop, name in functions
                     if addr <= start and stop <= end]
            function_count = len(SP.definition_symbols(rel, owned))
            is_mod = rel.startswith("mods/")
            if is_mod:
                mod_functions += function_count
                mod_bytes += size
            else:
                source_functions += function_count
                source_bytes += size
            lo, hi = addr - base, end - base
            if lo < 0 or hi > len(retail) or hi > len(built):
                if not is_mod:
                    bad += function_count
                    bad_function_bytes += size
                    failures.append({"module": label, "name": pathlib.Path(rel).stem,
                                     "addr": addr, "size": size,
                                     "reason": "range outside built or retail module"})
                continue
            nd = sum(1 for x, y in zip(built[lo:hi], retail[lo:hi]) if x != y)
            if is_mod:
                intentional.append({"name": pathlib.Path(rel).stem, "module": label,
                                    "addr": addr, "size": size, "differingBytes": nd})
                continue
            if nd:
                bad += function_count
                bad_function_bytes += size
                differing_source_bytes += nd
                failures.append({"module": label, "name": pathlib.Path(rel).stem,
                                 "addr": addr, "size": size, "kind": "code",
                                 "section": None, "differingBytes": nd})
                per_module_bad[label] += 1
            else:
                reproducing += function_count
                reproducing_bytes += size

        # The loop above walks .text/.init only, because those are the entries that own
        # FUNCTIONS and every counter in it is a function counter. A source-owned .data,
        # .rodata or .bss claim is a byte claim with no function in it, so it fell out
        # of that loop entirely and produced no diagnostic row at all.
        #
        # The verdict was still correctly red -- _diff_counts above sees the differing
        # byte, it is outside every mods/ range, so unexpectedDifferingBytes is non-zero
        # and `passed` is False. But a red verdict with an empty `failures` list and
        # mismatchingFunctions == 0 says only "something, somewhere". That is the least
        # useful shape exactly when a data claim first goes wrong, so these claims get
        # their own rows here.
        #
        # They are counted separately rather than folded into `bad`: `bad` is reported
        # as "mismatching functions" and a data claim contributes no function to it.
        # `passed` gains the new counter explicitly rather than leaning on the module
        # diff to stay red on its behalf -- one fact, one gate.
        for rel, name, addr, end in entry_sections:
            if not rel.startswith("src/") or name in (".text", ".init"):
                continue
            size = end - addr
            source_data_claims += 1
            lo, hi = addr - base, end - base
            row = {"module": label, "name": pathlib.Path(rel).stem, "addr": addr,
                   "size": size, "kind": "data", "section": name}
            if lo < 0 or hi > len(retail) or hi > len(built):
                bad_data_claims += 1
                bad_data_bytes += size
                failures.append({**row,
                                 "reason": "range outside built or retail module"})
                per_module_bad[label] += 1
                continue
            nd = sum(1 for x, y in zip(built[lo:hi], retail[lo:hi]) if x != y)
            if nd:
                bad_data_claims += 1
                bad_data_bytes += size
                differing_source_bytes += nd
                failures.append({**row, "differingBytes": nd})
                per_module_bad[label] += 1
            else:
                reproducing_data_claims += 1
                reproducing_data_bytes += size

    total_functions, total_code_bytes = _code_totals(config_root)
    compared_module_bytes = sum(m["comparedBytes"] for m in module_results)
    module_code_bytes = sum(m["codeBytes"] for m in module_results)
    differing_module_bytes = sum(m["differingBytes"] for m in module_results)
    unexpected_module_bytes = sum(m["unexpectedDifferingBytes"] for m in module_results)
    compiled_functions = source_functions + mod_functions
    compiled_bytes = source_bytes + mod_bytes
    mods_applied = all(m["differingBytes"] > 0 for m in intentional) if intentional else True
    passed = bool(module_results) and not bad and not bad_data_claims \
        and not missing_bins and not unexpected_module_bytes \
        and (profile != "mods" or mods_applied)

    return {
        "schemaVersion": 1,
        "profile": profile,
        "passed": passed,
        "moduleFidelity": {
            "modulesChecked": len(module_results),
            "modulesExact": sum(1 for m in module_results if m["exact"]),
            "modulesExpectedOnly": sum(1 for m in module_results if m["expectedOnly"]),
            "comparedBytes": compared_module_bytes,
            "differingBytes": differing_module_bytes,
            "unexpectedDifferingBytes": unexpected_module_bytes,
            "percent": (100.0 * (compared_module_bytes - differing_module_bytes)
                        / compared_module_bytes) if compared_module_bytes else 0.0,
            "moduleSetSha256": (module_set_digest.hexdigest()
                                if module_results else None),
            "results": module_results,
        },
        # What the 106 module images are MADE OF, so the headline percentages cannot be
        # read as coverage of the cartridge. `moduleFidelity.percent` is 100% whenever
        # the build is green, but only `sourceBytes` of these bytes came from source --
        # every other byte is one dsd handed back from the ROM, compared against itself.
        # `dataBytes` is the part no delink entry can reach at all today.
        "moduleComposition": {
            "moduleBytes": compared_module_bytes,
            "codeBytes": module_code_bytes,
            "dataBytes": compared_module_bytes - module_code_bytes,
            "sourceDataBytes": source_data_bytes,
            "unownedDataBytes": max(0, compared_module_bytes - module_code_bytes
                                      - source_data_bytes),
            "sourceBytes": source_bytes,
            "sourceBytesOfModulePercent": (100.0 * source_bytes / compared_module_bytes
                                           if compared_module_bytes else 0.0),
            "dataBytesVerified": 0,
            "sourceDataBytesOfModulePercent": (
                100.0 * source_data_bytes / compared_module_bytes
                if compared_module_bytes else 0.0),
            "dataBytesOfModulePercent": (100.0 * (compared_module_bytes - module_code_bytes)
                                         / compared_module_bytes
                                         if compared_module_bytes else 0.0),
        },
        "sourceBuild": {
            "totalCodeFunctions": total_functions,
            "totalCodeBytes": total_code_bytes,
            "compiledFunctions": compiled_functions,
            "compiledBytes": compiled_bytes,
            "sourceFunctions": source_functions,
            "sourceBytes": source_bytes,
            "modFunctions": mod_functions,
            "modBytes": mod_bytes,
            "sourceBytesPercent": 100.0 * source_bytes / total_code_bytes if total_code_bytes else 0.0,
            "reproducingFunctions": reproducing,
            "reproducingBytes": reproducing_bytes,
            "mismatchingFunctions": bad,
            "mismatchingFunctionBytes": bad_function_bytes,
            "differingSourceBytes": differing_source_bytes,
            # Non-text source-owned claims, kept as their own counters: they carry no
            # functions, so folding them into mismatchingFunctions would report a
            # function count that no function is behind. differingSourceBytes is
            # shared, because a differing byte is a differing byte either way.
            "sourceDataClaims": source_data_claims,
            "reproducingDataClaims": reproducing_data_claims,
            "reproducingDataClaimBytes": reproducing_data_bytes,
            "mismatchingDataClaims": bad_data_claims,
            "mismatchingDataClaimBytes": bad_data_bytes,
        },
        "intentionalMods": intentional,
        "modsApplied": mods_applied,
        "missingModuleBinaries": missing_bins,
        "failures": failures,
        "mismatchesByModule": dict(per_module_bad),
    }


def print_report(report, show=12):
    sf, mf = report["sourceBuild"], report["moduleFidelity"]
    if report["intentionalMods"]:
        print(f"intentional divergences (mods/): {len(report['intentionalMods'])}")
        for m in report["intentionalMods"]:
            state = (f"{m['differingBytes']} byte(s) differ from the ROM" if m["differingBytes"]
                     else "!! identical to the ROM - the mod is NOT in the build")
            print(f"  {m['name']} ({m['module']}, 0x{m['addr']:08x}): {state}")
        print()
    print(f"source-built functions: {sf['sourceFunctions']:,}  "
          f"({sf['sourceBytes']:,} / {sf['totalCodeBytes']:,} code bytes, "
          f"{sf['sourceBytesPercent']:.2f}%)")
    print(f"  reproducing: {sf['reproducingFunctions']:,}")
    print(f"  mismatching: {sf['mismatchingFunctions']:,}")
    if sf.get("sourceDataClaims"):
        # Only printed when such a claim exists, so the stock line is unchanged while
        # the count is zero -- but once one exists, a mismatch in it must be as visible
        # as a mismatching function, not buried in the module byte delta.
        print(f"source-owned data claims: {sf['sourceDataClaims']:,}  "
              f"(reproducing {sf.get('reproducingDataClaims', 0):,}, "
              f"mismatching {sf.get('mismatchingDataClaims', 0):,})")
    print(f"module fidelity: {mf['modulesExact']}/{mf['modulesChecked']} exact, "
          f"{mf['percent']:.6f}% of compared bytes")
    mc = report.get("moduleComposition")
    if mc:
        # Printed right under the 100.000000%, because that figure is exact by
        # construction for every byte dsd supplies from the ROM and says nothing at all
        # about them. This line is what it is a percentage OF.
        print(f"  of {mc['moduleBytes']:,} module bytes: {mc['sourceBytes']:,} "
              f"({mc['sourceBytesOfModulePercent']:.1f}%) source-built code, "
              f"{mc.get('sourceDataBytes', 0):,} source-owned data, "
              f"{mc.get('unownedDataBytes', mc['dataBytes']):,} data bytes no complete "
              f"source entry reaches ({mc['dataBytesVerified']:,} verified)")
    if report["missingModuleBinaries"]:
        print(f"missing module binaries: {report['missingModuleBinaries'][:8]}")
    for f in report["failures"][:show]:
        tag = f" {f['section']}" if f.get("kind") == "data" and f.get("section") else ""
        print(f"  {f['module']:26s} {f['name'] + tag:44s} 0x{f['addr']:08x} "
              f"size 0x{f['size']:<5x} {f.get('differingBytes', f.get('reason'))}")
    print(f"ROM-build analysis: {'PASS' if report['passed'] else 'FAIL'}")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--out", help="write failing symbol names here")
    ap.add_argument("--json", help="write the complete structured report")
    ap.add_argument("--config-root", default=str(DEFAULT_CONFIG_ROOT),
                    help="ARM9 config directory used for the build")
    ap.add_argument("--profile", choices=("stock", "mods"), default="stock")
    ap.add_argument("--build-root", default=None,
                    help="where the link wrote its module images (default build/build)")
    ap.add_argument("--show", type=int, default=12)
    args = ap.parse_args()
    report = analyze(args.config_root, args.profile, args.build_root)
    print_report(report, args.show)
    if args.out:
        # Code rows only. This file is a list of FUNCTION names -- rombuild_versions.py
        # feeds it straight into a per-function compiler-version sweep -- and a data
        # claim's path stem names no function, so putting one here would send the sweep
        # after a symbol that does not exist. The data rows are not lost: they are in
        # the printed report, in --json, and they make `passed` False on their own.
        pathlib.Path(args.out).write_text(
            "\n".join(sorted(f["name"] for f in report["failures"]
                             if f.get("kind", "code") != "data")) + "\n",
            encoding="utf-8", newline="\n")
    if args.json:
        pathlib.Path(args.json).write_text(
            json.dumps(report, indent=2) + "\n", encoding="utf-8", newline="\n")
    return 0 if report["passed"] else 1


if __name__ == "__main__":
    sys.exit(main())
