"""cpp_rename.py: rename func_ADDR files to demangled C++ symbol names safely.

Renames single-function `func_ADDR.c|cpp` files to demangled `_ZN...` symbol names
when the symbol is verified/known, using `git mv` to preserve git history.

Usage:
    python tools/cpp_rename.py --dry-run          # preview proposed renames
    python tools/cpp_rename.py --apply            # apply renames + git mv
    python tools/cpp_rename.py --addr 0x020b00e8  # rename specific function

Every applied rename is re-verified with tools/match.py against the function's own
module image. A rename that fails verification is rolled back in full (provenance,
content, git mv) and the batch stops there: a failure at this step means either the
checkout cannot verify at all (no extracted/, no compiler) or the rename map is
wrong, and both conditions poison every rename that would follow. Nothing stays
renamed that did not verify.
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
from stamp_provenance import FUNC_RE, matching_versions, module_target

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
            # A symbol rename must not flatten an already-organized subsystem, and must
            # not strand the file in a bucket its new name no longer belongs to -- a
            # src/unnamed/<mod>/ file that gains a class name has to leave. srcpath owns
            # both halves of that decision.
            new_path = SP.rename_target(path.with_suffix(new_ext), new_sym)

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

VERSION = "2004/b56"  # canonical matching compiler (match.py CANONICAL)

_size_cache = {}


def _sizes(module):
    """addr to size for every function in the module's config symbols.txt.

    The same file and line shape stamp_provenance.load_symbol reads, but keyed by
    address instead of name: right after a rename the file name and the symbols.txt
    name can legitimately disagree, while the address never does.
    """
    if module not in _size_cache:
        if module in ("arm9", "arm7"):
            sym = REPO / "config" / module / "symbols.txt"
        else:
            sym = REPO / "config" / "arm9" / "overlays" / module / "symbols.txt"
        table = {}
        if sym.is_file():
            for line in sym.read_text(errors="ignore").splitlines():
                m = FUNC_RE.match(line)
                if m:
                    table[int(m.group(3), 16)] = int(m.group(2), 16)
        _size_cache[module] = table
    return _size_cache[module]


def verify_match(rel_path, func_names, module, addr):
    """Whether rel_path still reproduces the ROM bytes at (module, addr).

    Returns (ok, reason); reason is "" on success. match.py requires --size, and the
    target image must be the symbol's own module (--module/--bin/--base), resolved
    through the same registry stamp_provenance.run_match uses. The previous version
    of this function passed none of that, so match.py exited 2 on argparse before
    compiling anything and every verification failed (see test_cpp_rename_verify.py).

    func_names are tried in order. Renaming the file does not rename the function
    inside it, so the compiled object usually still defines the old address name,
    while a file already migrated to a real C++ definition defines the mangled one.
    """
    size = _sizes(module).get(addr)
    if size is None:
        return False, f"no function size for {module}:{addr:#x} in config symbols.txt"
    target = module_target(module)
    if target is None:
        return False, f"no target binary for module {module!r} (needs extracted/)"
    bin_path, base = target
    if not bin_path.is_file():
        return False, f"module {module} binary missing: {bin_path}"
    reason = "no candidate function name"
    for func in func_names:
        cmd = [
            sys.executable,
            str(REPO / "tools" / "match.py"),
            "--c", rel_path,
            "--func", func,
            "--addr", hex(addr),
            "--size", hex(size),
            "--module", module,
            "--bin", str(bin_path),
            "--base", hex(base),
            "--version", VERSION,
            "--brief",
        ]
        try:
            r = subprocess.run(cmd, capture_output=True, text=True, cwd=str(REPO),
                               timeout=180)
        except subprocess.TimeoutExpired:
            return False, f"match.py timed out on {func}"
        out = (r.stdout or "") + (r.stderr or "")
        if r.returncode != 0:
            return False, f"match.py exited {r.returncode}"
        versions = matching_versions(out)
        if versions is None:
            return False, "match.py printed no MATCHING VERSIONS verdict"
        if VERSION in versions:
            return True, ""
        if f"symbol '{func}' not found in object" in out:
            reason = f"object defines none of: {', '.join(func_names)}"
            continue
        return False, f"{func} does not reproduce the target under {VERSION}"
    return False, reason


def apply_rename(item):
    """git mv, //cpp header, provenance rewrite, then verify. Returns (ok, reason).

    Verification runs last because it must judge the file's final state: the //cpp
    header flips match.py's compile mode from c99 to c++, so verifying the old path
    first would grade the wrong compile. On failure the whole rename is rolled back
    (provenance, content, git mv), so a failed apply leaves the tree as it was.
    """
    old_p = item["old_path"]
    new_p = item["new_path"]
    sym = item["symbol"]
    addr = int(item["addr"], 16)
    old_stem = old_p.stem
    module = SP.module_of(old_stem)
    if module is None:
        return False, f"cannot derive a module from {old_stem!r}"

    old_rel = str(old_p.relative_to(REPO)).replace("\\", "/")
    new_rel = str(new_p.relative_to(REPO)).replace("\\", "/")

    # 1. Git move file
    subprocess.run(["git", "mv", str(old_p), str(new_p)], check=True, cwd=str(REPO))
    SP.invalidate()

    # 2. Ensure //cpp header if it's a C++ file
    original = None
    if item["is_cpp"]:
        content = new_p.read_text(encoding="utf-8", errors="ignore")
        if not content.startswith("//cpp"):
            original = content
            new_p.write_text("//cpp\n" + content, encoding="utf-8")

    # 3. Update match_provenance.jsonl
    prov_updated = update_match_provenance(old_rel, new_rel)

    # 4. Verify match; roll back 3, 2, 1 in reverse order if it does not hold
    ok, reason = verify_match(new_rel, [old_stem, sym], module, addr)
    if not ok:
        if prov_updated:
            update_match_provenance(new_rel, old_rel)
        if original is not None:
            new_p.write_text(original, encoding="utf-8")
        subprocess.run(["git", "mv", str(new_p), str(old_p)], check=True, cwd=str(REPO))
        SP.invalidate()
    return ok, reason


def apply_all(renames):
    """Apply renames in order, stopping at the first verification failure.

    The failed rename is already rolled back by apply_rename; the rest are left
    un-attempted so a re-run after the investigation starts from a clean tree.
    A non-verification error (git mv refusing a rename, say a destination that
    already exists) touches nothing beyond its own file, so the batch continues.

    Returns (successful, failed, skipped).
    """
    successful = failed = 0
    for i, item in enumerate(renames):
        old_rel = str(item["old_path"].relative_to(REPO))
        new_rel = str(item["new_path"].relative_to(REPO))
        try:
            ok, reason = apply_rename(item)
        except Exception as e:
            print(f"  [X] Failed to rename {old_rel}: {e}")
            failed += 1
            continue
        if ok:
            print(f"  [OK] Renamed {old_rel} -> {new_rel}")
            successful += 1
        else:
            failed += 1
            print(f"  [!] {old_rel}: verification failed ({reason}); rename rolled back")
            skipped = len(renames) - i - 1
            if skipped:
                print(f"      Stopping here: {skipped} remaining rename(s) not attempted.")
            return successful, failed, skipped
    return successful, failed, 0

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
    successful, failed, skipped = apply_all(renames)

    line = f"\nFinished: {successful} successful, {failed} failed"
    if skipped:
        line += f", {skipped} skipped"
    print(line + ".")

if __name__ == "__main__":
    main()
