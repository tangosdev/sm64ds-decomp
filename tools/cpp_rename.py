"""cpp_rename.py — Rename func_ADDR files to demangled C++ symbol names safely.

Renames single-function `func_ADDR.c|cpp` files to demangled `_ZN...` symbol names
when the symbol is verified/known, using `git mv` to preserve git history.

Usage:
    python tools/cpp_rename.py --dry-run          # preview proposed renames
    python tools/cpp_rename.py --apply            # apply renames + git mv
    python tools/cpp_rename.py --addr 0x020b00e8  # rename specific function
"""
import argparse
import json
import os
import pathlib
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

import demangle as DM
import srcpath as SP

def load_verified_symbols():
    """Load map of addr -> symbol_name from verified.tsv, actor_renames.tsv, and symbols.txt."""
    symbol_map = {}  # (module, addr_hex_str) -> new_symbol_name

    # 1. verified.tsv
    v_tsv = REPO / "symbols" / "verified.tsv"
    if v_tsv.is_file():
        for line in v_tsv.read_text(encoding="utf-8", errors="ignore").splitlines():
            parts = line.strip().split("\t")
            if len(parts) >= 2:
                addr_str, name = parts[0], parts[1]
                if name.startswith("_Z"):
                    try:
                        addr = int(addr_str, 16) if addr_str.startswith("0x") else int(addr_str)
                        symbol_map[f"0x{addr:08x}"] = name
                    except ValueError:
                        pass

    # 2. actor_renames.tsv
    a_tsv = REPO / "symbols" / "actor_renames.tsv"
    if a_tsv.is_file():
        for line in a_tsv.read_text(encoding="utf-8", errors="ignore").splitlines()[1:]:
            parts = line.strip().split("\t")
            if len(parts) >= 4:
                mod, addr_str, old_name, new_name = parts[0], parts[1], parts[2], parts[3]
                if new_name.startswith("_Z"):
                    try:
                        addr = int(addr_str, 16) if addr_str.startswith("0x") else int(addr_str)
                        symbol_map[f"0x{addr:08x}"] = new_name
                    except ValueError:
                        pass

    # 3. Arm9 symbols.txt
    symbols_txt = REPO / "config" / "arm9" / "symbols.txt"
    if symbols_txt.is_file():
        for line in symbols_txt.read_text(encoding="utf-8", errors="ignore").splitlines():
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split()
            if len(parts) >= 2:
                name = parts[0]
                if name.startswith("_Z"):
                    for p in parts[1:]:
                        if p.startswith("addr:"):
                            try:
                                addr = int(p[5:], 16)
                                symbol_map[f"0x{addr:08x}"] = name
                            except ValueError:
                                pass

    return symbol_map

def find_renamable_files(target_addr=None):
    verified_map = load_verified_symbols()
    renames = []  # list of dict(src_path, new_name, new_path, symbol, is_cpp)

    if not SP.SRC.is_dir():
        return renames

    for path in SP.iter_sources():
        if path.suffix not in (".c", ".cpp"):
            continue

        stem = path.stem
        # Match func_ADDR or func_mod_ADDR patterns
        m = None
        if stem.startswith("func_"):
            parts = stem.split("_")
            if len(parts) >= 2:
                last_part = parts[-1]
                if len(last_part) == 8 and all(c in "0123456789abcdefABCDEF" for c in last_part):
                    m = f"0x{last_part.lower()}"

        if not m:
            continue

        if target_addr:
            target_clean = f"0x{int(target_addr, 16):08x}" if target_addr.startswith("0x") else f"0x{int(target_addr):08x}"
            if m != target_clean:
                continue

        if m in verified_map:
            new_sym = verified_map[m]
            is_cpp = new_sym.startswith("_Z")
            new_ext = ".cpp" if is_cpp else path.suffix
            new_filename = f"{new_sym}{new_ext}"
            # A symbol rename must not flatten an already-organized subsystem.
            new_path = path.with_name(new_filename)

            if new_path != path:
                renames.append({
                    "old_path": path,
                    "new_path": new_path,
                    "symbol": new_sym,
                    "addr": m,
                    "is_cpp": is_cpp,
                })

    return renames

def update_match_provenance(old_rel_path, new_rel_path):
    prov_file = REPO / "config" / "match_provenance.jsonl"
    if not prov_file.is_file():
        return False

    lines = prov_file.read_text(encoding="utf-8", errors="ignore").splitlines()
    updated = False
    new_lines = []

    for line in lines:
        if not line.strip():
            continue
        try:
            obj = json.loads(line)
            if obj.get("srcPath") == old_rel_path:
                obj["srcPath"] = new_rel_path
                updated = True
            new_lines.append(json.dumps(obj) + "\n")
        except json.JSONDecodeError:
            new_lines.append(line + "\n")

    if updated:
        prov_file.write_text("".join(new_lines), encoding="utf-8")
    return updated

def verify_match(rel_path, symbol, addr_str):
    """Run tools/match.py to verify that the renamed file still byte-matches."""
    cmd = [
        sys.executable,
        str(REPO / "tools" / "match.py"),
        "--c", rel_path,
        "--func", symbol,
        "--addr", addr_str,
        "--version", "2004/b56"
    ]
    r = subprocess.run(cmd, capture_output=True, text=True, cwd=str(REPO))
    return "MATCHING VERSIONS: 2004/b56" in r.stdout

def apply_rename(item):
    old_p = item["old_path"]
    new_p = item["new_path"]
    sym = item["symbol"]
    addr = item["addr"]

    old_rel = str(old_p.relative_to(REPO)).replace("\\", "/")
    new_rel = str(new_p.relative_to(REPO)).replace("\\", "/")

    # 1. Git move file
    subprocess.run(["git", "mv", str(old_p), str(new_p)], check=True, cwd=str(REPO))
    SP.invalidate()

    # 2. Ensure //cpp header if it's a C++ file
    if item["is_cpp"]:
        content = new_p.read_text(encoding="utf-8", errors="ignore")
        if not content.startswith("//cpp"):
            new_p.write_text("//cpp\n" + content, encoding="utf-8")

    # 3. Update match_provenance.jsonl
    update_match_provenance(old_rel, new_rel)

    # 4. Verify match
    ok = verify_match(new_rel, sym, addr)
    return ok

def main():
    parser = argparse.ArgumentParser(description="Rename func_ADDR files to demangled symbol names")
    parser.add_argument("--dry-run", action="store_true", help="Preview proposed renames without executing")
    parser.add_argument("--apply", action="store_true", help="Apply renames and update git history")
    parser.add_argument("--addr", help="Rename single file matching target hex address")
    args = parser.parse_args()

    renames = find_renamable_files(target_addr=args.addr)

    if not renames:
        print("No renamable func_ADDR files found.")
        return

    print(f"Found {len(renames)} renamable file(s):\n")

    for item in renames:
        old_rel = str(item["old_path"].relative_to(REPO))
        new_rel = str(item["new_path"].relative_to(REPO))
        dem = DM.signature(item["symbol"]) or item["symbol"]
        print(f"  {old_rel:<35} -> {new_rel:<45} ({dem})")

    if args.dry_run or not args.apply:
        print("\nDry-run complete. Run with --apply to execute renames.")
        return

    print("\nApplying renames...")
    successful = 0
    failed = 0

    for item in renames:
        old_rel = str(item["old_path"].relative_to(REPO))
        new_rel = str(item["new_path"].relative_to(REPO))
        try:
            ok = apply_rename(item)
            if ok:
                print(f"  [OK] Renamed {old_rel} -> {new_rel}")
                successful += 1
            else:
                print(f"  [!] Renamed {old_rel} -> {new_rel} (VERIFICATION FAILED!)")
                failed += 1
        except Exception as e:
            print(f"  [X] Failed to rename {old_rel}: {e}")
            failed += 1

    print(f"\nFinished: {successful} successful, {failed} failed.")

if __name__ == "__main__":
    main()
