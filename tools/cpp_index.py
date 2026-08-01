"""cpp_index.py — Class-level decompilation index tool.

Aggregates all C++ symbols across the codebase (from src/, config symbol files,
tsv symbol lists, and nearmiss/db.jsonl) and presents a structured view grouped by class.

Usage:
    python tools/cpp_index.py                      # all classes summary
    python tools/cpp_index.py --class Player       # inspect single class
    python tools/cpp_index.py --missing            # show classes with unmatched methods
    python tools/cpp_index.py --json report.json   # export machine-readable JSON
    python tools/cpp_index.py --md report.md       # export Markdown summary
"""
import argparse
import json
import os
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

import demangle as DM
import ledger as L

def load_all_symbols():
    """Load symbols from symbols.txt and overlay symbol files."""
    symbols = {}  # name -> dict(module, addr, size)
    
    # 1. Main ARM9 symbols.txt
    symbols_txt = REPO / "config" / "arm9" / "symbols.txt"
    if symbols_txt.is_file():
        for line in symbols_txt.read_text(encoding="utf-8", errors="ignore").splitlines():
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split()
            if len(parts) >= 2:
                name = parts[0]
                addr = None
                size = 0
                kind = "function"
                for p in parts[1:]:
                    if p.startswith("addr:"):
                        try: addr = int(p[5:], 16)
                        except ValueError: pass
                    elif p.startswith("size:"):
                        try: size = int(p[5:], 16)
                        except ValueError: pass
                    elif p.startswith("kind:"):
                        kind = p[5:].split("(")[0]
                if name and addr is not None:
                    symbols[name] = {"name": name, "module": "arm9", "addr": addr, "size": size, "kind": kind}

    # 2. Overlay symbol files
    overlays_dir = REPO / "config" / "arm9" / "overlays"
    if overlays_dir.is_dir():
        for ov_dir in overlays_dir.iterdir():
            ov_sym = ov_dir / "symbols.txt"
            if ov_sym.is_file():
                mod = ov_dir.name
                for line in ov_sym.read_text(encoding="utf-8", errors="ignore").splitlines():
                    line = line.strip()
                    if not line or line.startswith("#"):
                        continue
                    parts = line.split()
                    if len(parts) >= 2:
                        name = parts[0]
                        addr = None
                        size = 0
                        kind = "function"
                        for p in parts[1:]:
                            if p.startswith("addr:"):
                                try: addr = int(p[5:], 16)
                                except ValueError: pass
                            elif p.startswith("size:"):
                                try: size = int(p[5:], 16)
                                except ValueError: pass
                            elif p.startswith("kind:"):
                                kind = p[5:].split("(")[0]
                        if name and addr is not None:
                            symbols[name] = {"name": name, "module": mod, "addr": addr, "size": size, "kind": kind}

    # 3. TSV files for extra mapped symbols
    tsv_files = [REPO / "symbols" / "actor_renames.tsv"]
    for tsv in tsv_files:
        if tsv.is_file():
            for line in tsv.read_text(encoding="utf-8", errors="ignore").splitlines()[1:]:
                parts = line.split("\t")
                if len(parts) >= 4:
                    mod, addr_str, old_name, new_name = parts[0], parts[1], parts[2], parts[3]
                    if new_name and new_name.startswith("_Z"):
                        try:
                            addr = int(addr_str, 16) if addr_str.startswith("0x") else int(addr_str)
                            if new_name not in symbols:
                                symbols[new_name] = {"name": new_name, "module": mod, "addr": addr, "size": 0, "kind": "function"}
                        except ValueError:
                            pass

    return symbols

def load_src_matched(known_symbols):
    """Return dict of symbol_name -> filepath for matched src/ files."""
    matched = {}
    src_dir = REPO / "src"
    known_set = set(known_symbols)
    if src_dir.is_dir():
        for path in src_dir.iterdir():
            if path.suffix in (".c", ".cpp"):
                stem = path.stem
                rel_path = str(path.relative_to(REPO))
                if stem.startswith("_Z"):
                    matched[stem] = rel_path
                elif not stem.startswith("func_"):
                    # Consolidated multi-function class file (e.g., CylinderClsn.cpp, ActorDerived.cpp)
                    content = path.read_text(encoding="utf-8", errors="ignore")
                    # Extract all _Z mangled symbol references in the file
                    found_syms = set(re.findall(r"_Z[a-zA-Z0-9_]+", content))
                    for sym in found_syms:
                        if sym in known_set:
                            matched[sym] = rel_path
                    
                    # Also match methods of stem class defined via ClassName::MethodName
                    for sym in known_set:
                        if sym.startswith(f"_ZN{len(stem)}{stem}"):
                            d = DM.demangle(sym)
                            if d:
                                method = d.get("method")
                                if method:
                                    if f"{stem}::{method}" in content or (d.get("dtor") and f"~{stem}" in content) or (d.get("ctor") and f"{stem}::" in content):
                                        matched[sym] = rel_path
    return matched

def load_nearmisses():
    """Return dict of symbol_name -> dict(divergences, source, label) from nearmiss/db.jsonl."""
    db_file = REPO / "nearmiss" / "db.jsonl"
    nearmisses = {}
    if db_file.is_file():
        for line in db_file.read_text(encoding="utf-8", errors="ignore").splitlines():
            line = line.strip()
            if line:
                try:
                    obj = json.loads(line)
                    name = obj.get("name")
                    if name:
                        nearmisses[name] = {
                            "divergences": obj.get("divergences"),
                            "source": obj.get("source"),
                            "floor": obj.get("floor")
                        }
                except json.JSONDecodeError:
                    pass
    return nearmisses

def build_class_index(class_filter=None, missing_only=False):
    symbols = load_all_symbols()
    src_matched = load_src_matched(symbols.keys())
    nearmisses = load_nearmisses()

    classes = {}  # class_name -> dict(matched=[], unmatched=[], nearmiss=[])

    all_sym_names = set(symbols.keys()) | set(src_matched.keys())

    for sym_name in all_sym_names:
        if not sym_name.startswith("_Z"):
            continue
        d = DM.demangle(sym_name)
        if not d or not d.get("class"):
            continue
        
        cls_name = d["class"]
        if class_filter and class_filter.lower() not in cls_name.lower():
            continue

        if cls_name not in classes:
            classes[cls_name] = {
                "class": cls_name,
                "matched": [],
                "unmatched": [],
                "nearmiss": []
            }

        sym_info = symbols.get(sym_name, {
            "name": sym_name,
            "module": "unknown",
            "addr": 0,
            "size": 0,
            "kind": "function"
        })
        
        entry = {
            "symbol": sym_name,
            "demangled": DM.signature(sym_name) or d["qualified"],
            "method": d["method"],
            "ctor": d["ctor"],
            "dtor": d["dtor"],
            "thunk": d.get("thunk", False),
            "thunk_offset": d.get("thunk_offset"),
            "module": sym_info["module"],
            "addr": f"0x{sym_info['addr']:08x}" if isinstance(sym_info['addr'], int) else str(sym_info['addr']),
            "size": sym_info["size"],
        }

        if sym_name in src_matched:
            entry["file"] = src_matched[sym_name]
            classes[cls_name]["matched"].append(entry)
        elif sym_name in nearmisses:
            entry["nearmiss"] = nearmisses[sym_name]
            classes[cls_name]["nearmiss"].append(entry)
        else:
            classes[cls_name]["unmatched"].append(entry)

    if missing_only:
        classes = {k: v for k, v in classes.items() if v["unmatched"] or v["nearmiss"]}

    return classes

def print_cli_report(classes, single_class_filter=None):
    if not classes:
        print("No C++ classes found matching criteria.")
        return

    sorted_classes = sorted(classes.values(), key=lambda c: c["class"])

    def safe_print(text):
        try:
            print(text)
        except UnicodeEncodeError:
            print(text.encode("ascii", errors="replace").decode("ascii"))

    if single_class_filter and len(sorted_classes) == 1:
        cls = sorted_classes[0]
        n_matched = len(cls["matched"])
        n_unmatched = len(cls["unmatched"])
        n_nearmiss = len(cls["nearmiss"])
        safe_print(f"\n=== {cls['class']} ({n_matched} matched, {n_unmatched} unmatched, {n_nearmiss} near-miss) ===\n")

        if cls["matched"]:
            safe_print("  Matched Functions:")
            for e in sorted(cls["matched"], key=lambda x: x["symbol"]):
                safe_print(f"    [OK] {e['demangled']:<45} {e['symbol']:<35} {e['file']}")

        if cls["nearmiss"]:
            safe_print("\n  Near-Miss Attempts:")
            for e in sorted(cls["nearmiss"], key=lambda x: x["symbol"]):
                div = e['nearmiss'].get('divergences', '?')
                safe_print(f"    [!]  {e['demangled']:<45} {e['symbol']:<35} nearmiss (div {div})")

        if cls["unmatched"]:
            safe_print("\n  Unmatched Symbols:")
            for e in sorted(cls["unmatched"], key=lambda x: x["symbol"]):
                safe_print(f"    [X]  {e['demangled']:<45} {e['symbol']:<35} ({e['module']}:{e['addr']})")
        safe_print("")
    else:
        safe_print(f"\n{'Class Name':<35} {'Matched':<10} {'Near-Miss':<10} {'Unmatched':<10} {'Completion'}")
        safe_print("-" * 75)
        tot_m = tot_nm = tot_u = 0
        for cls in sorted_classes:
            m = len(cls["matched"])
            nm = len(cls["nearmiss"])
            u = len(cls["unmatched"])
            tot_m += m
            tot_nm += nm
            tot_u += u
            tot = m + nm + u
            pct = (m / tot * 100) if tot > 0 else 0
            safe_print(f"{cls['class']:<35} {m:<10} {nm:<10} {u:<10} {pct:5.1f}%")
        safe_print("-" * 75)
        tot_all = tot_m + tot_nm + tot_u
        pct_tot = (tot_m / tot_all * 100) if tot_all > 0 else 0
        safe_print(f"{'TOTAL (' + str(len(sorted_classes)) + ' classes)':<35} {tot_m:<10} {tot_nm:<10} {tot_u:<10} {pct_tot:5.1f}%\n")

def main():
    parser = argparse.ArgumentParser(description="Class-level decompilation index tool")
    parser.add_argument("--class", dest="cls_filter", help="Filter by class name (case-insensitive)")
    parser.add_argument("--missing", action="store_true", help="Only show classes with unmatched or near-miss symbols")
    parser.add_argument("--json", help="Export index report to JSON file")
    parser.add_argument("--md", help="Export summary report to Markdown file")
    args = parser.parse_args()

    classes = build_class_index(class_filter=args.cls_filter, missing_only=args.missing)

    if args.json:
        out_p = pathlib.Path(args.json)
        out_p.write_text(json.dumps(classes, indent=2), encoding="utf-8")
        print(f"Wrote JSON report to {out_p}")

    if args.md:
        out_p = pathlib.Path(args.md)
        lines = ["# C++ Class Decompilation Index Report\n",
                 "| Class Name | Matched | Near-Miss | Unmatched | Completion |",
                 "| :--- | :---: | :---: | :---: | :---: |"]
        for cls in sorted(classes.values(), key=lambda c: c["class"]):
            m = len(cls["matched"])
            nm = len(cls["nearmiss"])
            u = len(cls["unmatched"])
            tot = m + nm + u
            pct = (m / tot * 100) if tot > 0 else 0
            lines.append(f"| `{cls['class']}` | {m} | {nm} | {u} | {pct:.1f}% |")
        out_p.write_text("\n".join(lines) + "\n", encoding="utf-8")
        print(f"Wrote Markdown report to {out_p}")

    if not args.json and not args.md:
        print_cli_report(classes, single_class_filter=args.cls_filter)

if __name__ == "__main__":
    main()
