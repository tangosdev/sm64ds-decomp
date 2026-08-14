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
import json
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from enroll import read_delinks, sections, CONFIG, REPO  # noqa: E402

BUILD = REPO / "build" / "build"
EXTRACTED = REPO / "extracted" / "dsd"
DEFAULT_CONFIG_ROOT = CONFIG / "arm9"
ENTRY_SEC = re.compile(r"^\s+(\.\S+)\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)")
FUNC_RE = re.compile(r"^(\S+)\s+kind:function\((?:arm|thumb),size=0x([0-9a-fA-F]+)\)")


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


def complete_entries_text(text):
    """Return ``[(path, address, end)]`` for entries carrying ``complete``."""
    out, cur, done, sec = [], None, False, None
    for line in text.splitlines():
        if not line.strip():
            continue
        if not line[0].isspace():
            if cur and done and sec:
                out.append((cur, *sec))
            cur, done, sec = line.strip().rstrip(":"), False, None
        elif cur is not None:
            if line.strip() == "complete":
                done = True
            else:
                m = ENTRY_SEC.match(line)
                if m:
                    sec = (int(m.group(2), 16), int(m.group(3), 16))
    if cur and done and sec:
        out.append((cur, *sec))
    return out


def complete_entries(path):
    return complete_entries_text(path.read_text(encoding="utf-8", errors="ignore"))


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
    reproducing = reproducing_bytes = bad = bad_function_bytes = differing_source_bytes = 0

    for sym in sorted(config_root.rglob("symbols.txt")):
        d = sym.parent
        dl = d / "delinks.txt"
        if not dl.is_file():
            continue
        entries = complete_entries(dl)
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
        allowed_mod_ranges = [(addr - base, end - base) for rel, addr, end in entries
                              if rel.startswith("mods/")]
        module_diff, unexpected_diff = _diff_counts(built, retail, allowed_mod_ranges)
        module_results.append({
            "module": label,
            "comparedBytes": max(len(built), len(retail)),
            "differingBytes": module_diff,
            "unexpectedDifferingBytes": unexpected_diff,
            "exact": module_diff == 0,
            "expectedOnly": unexpected_diff == 0,
        })

        for rel, addr, end in entries:
            size = end - addr
            is_mod = rel.startswith("mods/")
            if is_mod:
                mod_functions += 1
                mod_bytes += size
            else:
                source_functions += 1
                source_bytes += size
            lo, hi = addr - base, end - base
            if lo < 0 or hi > len(retail) or hi > len(built):
                if not is_mod:
                    bad += 1
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
                bad += 1
                bad_function_bytes += size
                differing_source_bytes += nd
                failures.append({"module": label, "name": pathlib.Path(rel).stem,
                                 "addr": addr, "size": size, "differingBytes": nd})
                per_module_bad[label] += 1
            else:
                reproducing += 1
                reproducing_bytes += size

    total_functions, total_code_bytes = _code_totals(config_root)
    compared_module_bytes = sum(m["comparedBytes"] for m in module_results)
    differing_module_bytes = sum(m["differingBytes"] for m in module_results)
    unexpected_module_bytes = sum(m["unexpectedDifferingBytes"] for m in module_results)
    compiled_functions = source_functions + mod_functions
    compiled_bytes = source_bytes + mod_bytes
    mods_applied = all(m["differingBytes"] > 0 for m in intentional) if intentional else True
    passed = bool(module_results) and not bad and not missing_bins and not unexpected_module_bytes \
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
            "results": module_results,
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
    print(f"module fidelity: {mf['modulesExact']}/{mf['modulesChecked']} exact, "
          f"{mf['percent']:.6f}% of compared bytes")
    if report["missingModuleBinaries"]:
        print(f"missing module binaries: {report['missingModuleBinaries'][:8]}")
    for f in report["failures"][:show]:
        print(f"  {f['module']:26s} {f['name']:44s} 0x{f['addr']:08x} "
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
        pathlib.Path(args.out).write_text(
            "\n".join(sorted(f["name"] for f in report["failures"])) + "\n",
            encoding="utf-8", newline="\n")
    if args.json:
        pathlib.Path(args.json).write_text(
            json.dumps(report, indent=2) + "\n", encoding="utf-8", newline="\n")
    return 0 if report["passed"] else 1


if __name__ == "__main__":
    sys.exit(main())
